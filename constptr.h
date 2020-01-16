//
// Created by maxwell on 2020-01-14.
//

#ifndef COMPILER_CONTAINERS_CONSTPTR_H
#define COMPILER_CONTAINERS_CONSTPTR_H

#include <memory_resource>
#include <stdexcept>
#include "concept_types.h"
#include "counter.h"

//#include <initializer_list>
#include <span>

namespace pointers
{
    using id_type = const int *;

#ifndef get_unique_id
    template <typename T>
    struct unique_id
    {
        constexpr static int i = 0;
        constexpr unique_id(const T&){}
        constexpr operator id_type(){return &i;}
    };
        #define get_unique_id unique_id([]{})
    #endif

    template <id_type ID>
    struct id_value_type
    {
        constexpr id_value_type() = default;
        constexpr operator id_type(){return ID;}
        constexpr static id_type value = ID;
    };


    template <typename T, std::size_t s, id_type ID>
    struct ConstPtr;

    template <typename T, std::size_t s, id_type ID>
    ConstPtr<T, s, ID> fetch_pointer(id_value_type<ID>);


    template <typename PtrObj>
    struct ConstPtrReverse;

    template <typename PtrObj, bool forwards = true>
    struct ConstPtrIterator : PtrObj
    {
        using T = typename PtrObj::BaseType;
        constexpr static std::size_t size = PtrObj::s_;
        static T* const ptr;

        T& operator*() const
        {
            if(current_position < 0 || current_position >= size)
                throw std::out_of_range(std::string("Out of bounds error: Attempted to access array of size ") +
                                        std::to_string(size) +
                                        std::string(" at index ") +
                                        std::to_string(current_position));
            //printf("ptr + current_position: %p\nValue of ptr + current_position: %.2f\n", ptr + current_position*alignof(T), *(ptr + current_position*alignof(T)));
            return *(ptr + current_position);
        }
        T* operator->() const
        {
            if(current_position < 0 || current_position >= size)
                throw std::out_of_range(std::string("Out of bounds error: Attempted to access array of size ") +
                                        std::to_string(size) +
                                        std::string(" at index ") +
                                        std::to_string(current_position));

            return ptr + current_position;
        }
        T& operator[](int i) const
        {
            return static_cast<PtrObj&>(*this).operator[](i + current_position);
        }

        ConstPtrIterator& operator--() requires (size > 1)
        {
            if constexpr (forwards){
                current_position -= 1;
                return *this;
            } else {
                current_position += 1;
                return *this;
            }
        }
        ConstPtrIterator& operator++() requires (size > 1)
        {
            if constexpr(forwards){
                current_position += 1;
                return *this;
            } else {
                current_position -= 1;
                return *this;
            }
        }

        auto operator-(const ConstPtrIterator& other) const requires (size > 1)
        {
            return current_position - other.current_position;
        }

        bool operator !=(const ConstPtrIterator& other) const
        {
            return current_position != other.current_position;
        }
        bool operator ==(const ConstPtrIterator& other) const
        {
            return current_position == other.current_position;
        }

        [[nodiscard]] int position() const
        {
            return current_position;
        }

        friend PtrObj;
        friend ConstPtrReverse<PtrObj>;
    private:
        ConstPtrIterator(int i) : PtrObj(dont_initialize), current_position(i)
        {
#ifndef NDEBUG
            printf("Constructing Iterator for object %s\n", typeid(PtrObj).name());
#endif
        }
        int current_position;
    };

    template <typename PtrObj>
    struct ConstPtrReverse : PtrObj
    {
        ConstPtrIterator<PtrObj, false> begin() const
        {
            return ConstPtrIterator<PtrObj, false>(PtrObj::s_ -1);
        }
        ConstPtrIterator<PtrObj, false> end() const
        {
            return ConstPtrIterator<PtrObj, false>(-1);
        }
        ConstPtrIterator<PtrObj> rbegin() const
        {
            return ConstPtrIterator<PtrObj>(0);
        }
        ConstPtrIterator<PtrObj> rend() const
        {
            return ConstPtrIterator<PtrObj>(PtrObj::s_);
        }

        friend PtrObj;
    private:
        ConstPtrReverse() : PtrObj(dont_initialize){};
        ConstPtrReverse(PtrObj&) : PtrObj(dont_initialize){};
    };

    struct Created{};
    struct Destroyed{};

    template <typename T, id_type ID>
    struct ReferenceCounter
    {
        constexpr static id_type id = ID;
        using CreatedCount = Meta::ControlledCounter<Created, ID>;
        using DestroyedCount = Meta::ControlledCounter<Destroyed, ID>;

        template <id_type = get_unique_id>
        constexpr static long count = long(CreatedCount::CurrentValue()) - long(DestroyedCount::CurrentValue());
        constexpr ReferenceCounter()
        {
            CreatedCount::CurrentValue<true>();
        }
        ~ReferenceCounter()
        {
            if constexpr(!count) {
                T::dealloc();
            }
            static_assert(count >= 0, "Reference count is negative.\n");
        }
    };

    template <typename T, std::size_t size = 1, id_type ID = get_unique_id>
    struct ConstPtr : ReferenceCounter<ConstPtr<T, size, ID>, ID>
    {
        using RefCounter = ReferenceCounter<ConstPtr<T, size, ID>, ID>;
        using ThisPointer = ConstPtr<T, size, ID>;
        using BaseType = T;

        ConstPtr()
        {
            for(auto i = ptr; i != ptr + size; ++i)
            {
                allocator.construct(i);
            }
#ifndef NDEBUG
            printf("Value of ptr: %p\n", ptr);
#endif
        }
        ConstPtr(const T& obj)
        {
            for(auto i = ptr; i != ptr + size; ++i)
            {
                allocator.construct(i, obj);
            }
        }
        ConstPtr(T&& obj)
        {
            for(auto i = ptr; i != ptr + size; ++i)
            {
                allocator.construct(i, std::forward<T>(obj));
            }
        }

        ConstPtr(ConstPtr&){}
        ConstPtr(const ConstPtr&){}
        ConstPtr(ConstPtr&&) noexcept {}
        ConstPtr(const ConstPtr&&) noexcept {}

        template <typename ...Args>
        ConstPtr(Args&& ...args) requires Constructable<T, Args...>
        {
            for(T* i = ptr; i != ptr + size; ++i)
            {
                allocator.construct(i, std::forward<Args>(args)...);
            }
        }

        T& operator*() const
        {
            return *ptr;
        }
        T* operator->() const{
            return ptr;
        }
        T& operator[](int i) const
        {
            if(i < 0 || i >= size)
                throw std::out_of_range(std::string("Out of bounds error: Attempted to access array of size ") + std::to_string(size) + std::string(" at index ") + std::to_string(i));
            else
                return ptr[i];
        }

        ConstPtrIterator<ThisPointer> begin() requires (size > 1)
        {
            return ConstPtrIterator<ThisPointer>(0);
        }
        ConstPtrIterator<ThisPointer> end() requires (size > 1)
        {
            return ConstPtrIterator<ThisPointer>(int(size));
        }
        ConstPtrIterator<ThisPointer, false> rbegin() requires (size > 1)
        {
            return ConstPtrIterator<ThisPointer, false>(size - 1);
        }
        ConstPtrIterator<ThisPointer, false> rend() requires (size > 1)
        {
            return ConstPtrIterator<ThisPointer, false>(-1);
        }


        /**
         * operator !=
         */
        constexpr bool operator!=(const ConstPtr& other) const
        {
            return false;
        }
        constexpr bool operator!=(const ConstPtr&& other) const
        {
            return false;
        }
        template <typename T2, std::size_t s, id_type ID2>
        constexpr bool operator!=(const ConstPtr<T2, s, ID2>&) const
        {
            return true;
        }
        template <typename T2, std::size_t s, id_type ID2>
        constexpr bool operator!=(const ConstPtr<T2, s, ID2>&&) const
        {
            return true;
        }

        /**
         * operator ==
         */
        constexpr bool operator==(const ConstPtr& other) const
        {
            return true;
        }
        constexpr bool operator==(const ConstPtr&& other) const
        {
            return true;
        }
        template <typename T2, std::size_t s, id_type ID2>
        constexpr bool operator==(const ConstPtr<T2, s, ID2>&) const
        {
            return false;
        }
        template <typename T2, std::size_t s, id_type ID2>
        constexpr bool operator==(const ConstPtr<T2, s, ID2>&&) const
        {
            return false;
        }



        [[nodiscard]] constexpr std::size_t length() const
        {
            return size;
        }
        [[nodiscard]] auto reverse() const
        {
            return ConstPtrReverse(*this);
        }

        friend id_value_type<ID>;
        //friend ConstPtr fetch_pointer(id_value_type<ID>){return ConstPtr();}

        friend RefCounter;

    protected:
        //const int array_position = 0;
        ConstPtr(const NoInit&){}

        constexpr static std::size_t s_ = size;
        static void dealloc()
        {
            printf("Deallocating memory.");
            allocator.deallocate(ptr, size);
        }
        static std::pmr::polymorphic_allocator<T> allocator;
        static T* const ptr;
    };



    template <typename ...ValueLambdas>
    struct ConstexprInitValues
    {
        using ValueTypes =
        //template <typename ...Types>


        operator std::tuple<decltype(ValueLambdas{}())...>()
        {
            constexpr auto values = {ValueLambdas{}()...};
            return std::tuple{values};
        }
    };


    template <typename T, typename Initializer, id_type ID = get_unique_id>
    struct UniqueStorage : ReferenceCounter<UniqueStorage<T, ID>, ID>
    {
        constexpr UniqueStorage(){}


        template <ul>
        friend struct GenericContainer;
    private:


        static std::span<T> data;
    };


}

#endif //COMPILER_CONTAINERS_CONSTPTR_H
