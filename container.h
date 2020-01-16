//
// Created by maxwell on 2020-01-14.
//

#ifndef COMPILER_CONTAINERS_CONTAINER_H
#define COMPILER_CONTAINERS_CONTAINER_H

#include <utility>
#include <any>
#include "value_pair.h"
#include "counter.h"
#include "demangle_names.h"
#include <tuple>

template <auto Value>
struct value_type
{
    constexpr static auto value = Value;
    constexpr value_type() = default;
};

inline namespace generic{
    struct counter_tag{};
    struct secondary_tag{};

    template <auto ID1, auto ID2>
    struct type_encode_tag_t
    {
        constexpr type_encode_tag_t() = default;
        constexpr type_encode_tag_t(auto id1, auto id2){}

        constexpr bool operator==(const type_encode_tag_t) const
        {
            return true;
        }

        template <auto ID1_other, auto ID2_other>
        constexpr bool operator==(const type_encode_tag_t<ID1_other, ID2_other>) const
        {
            return false;
        }
    };
    //type_encode_tag_t(auto val1, auto val2) -> type_encode_tag_t<val1, val2>;

    template <auto ID1, auto ID2>
    inline constexpr type_encode_tag_t<ID1, ID2> type_tag = type_encode_tag_t<ID1, ID2>{};
}

using namespace generic;

template <ul ObjID = Meta::TaggedCounter<generic::counter_tag>::NextValue()>
struct GenericContainer
{
    template <bool B = false, ul N = Meta::CheckerWrapper<GenericContainer<ObjID>, 0>::currentval()>
    constexpr static ul counter()
    {
        if constexpr(B)
            return Meta::Next<GenericContainer, N>::value() + 1ul;
        else
            return N;
    }

    std::any stored_value = {};
    std::any* any_ptr = &stored_value;

    template <ul count = counter()>
    using stored_type = decode_type((type_tag<ObjID, count>));

    GenericContainer() = default;

    template <
            ul OtherID,
            ul count = counter<true>(),
            typename other_type = typename GenericContainer<OtherID>::template stored_type<>,
            bool encoded_type = encode_type((type_tag<ObjID, count>), other_type)>
    GenericContainer(const GenericContainer<OtherID>& other)
    {
        if constexpr(!encoded_type)
            printf("Failed in %s\n", __func__);
        other_type copy_val = *other;
        stored_value = copy_val;
    }

    template <typename T,
            ul count = counter<true>(),
            bool encoded_type = encode_type((type_tag<ObjID, count>), T)>
    GenericContainer(T&& value)
    {
        if constexpr(!encoded_type)
            printf("Failed in %s\n", __func__);
        stored_value = value;
    }

    /*template <typename T1,
            typename ...TN,
            ul count = counter<true>(),
            typename TupleT = std::tuple<T1, TN...>,
            bool encoded_type = encode_type((type_tag<ObjID, count>), TupleT)>
    GenericContainer(T1&& value, TN&& ...args) :
        GenericContainer(std::tuple{std::forward<T1>(value), std::forward<TN>(args)...}){}
*/

    template <
            ul OtherID,
            ul count = counter<true>(),
            typename other_type = typename GenericContainer<OtherID>::template stored_type<>,
            bool encoded_type = encode_type((type_tag<ObjID, count>), other_type)>
    GenericContainer& operator=(GenericContainer<OtherID>& other)
    {
        if constexpr(!encoded_type)
            printf("Failed in %s\n", __func__);
        other_type copy_val = *other;
        stored_value = copy_val;
        return *this;
    }

    template <typename T,
            ul count = counter<true>(),
            bool encoded_type = encode_type((type_tag<ObjID, count>), T)>
    GenericContainer& operator=(T&& value) &
    {
        if constexpr(!encoded_type)
            printf("Failed in %s\n", __func__);

        stored_value = std::forward<T>(value);
        return *this;
    }

    template <ul count = counter(),
            typename T = decode_type((type_tag<ObjID, count>))>
    operator T()
    {
        T copied_val = *std::any_cast<T>(any_ptr);
        return copied_val;
    }

    template <ul count = counter(),
            typename T = decode_type((type_tag<ObjID, count>))>
    operator T&()
    {
        return *std::any_cast<T>(any_ptr);
    }

    template <ul count = counter(),
            typename T = decode_type((type_tag<ObjID, count>))>
    operator T*()
    {
        return std::any_cast<T>(any_ptr);
    }

    template <ul count = counter(),
            typename T = decode_type((type_tag<ObjID, count>))>
    auto& operator*() const
    {
        return *std::any_cast<T>(any_ptr);
    }

    template <ul count = counter(),
            typename T = decode_type((type_tag<ObjID, count>))>
    constexpr T* operator->() const
    {
        return std::any_cast<T>(any_ptr);
    }
};

/*template <ul ID>
std::any GenericContainer<ID>::stored_value = {};*/

#define var GenericContainer<>



#endif //COMPILER_CONTAINERS_CONTAINER_H
