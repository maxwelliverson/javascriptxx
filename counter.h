//
// Created by maxwell on 2020-01-14.
//

#ifndef COMPILER_CONTAINERS_COUNTER_H
#define COMPILER_CONTAINERS_COUNTER_H

using ul = unsigned long;

template <typename Type, auto T = []{}>
constexpr auto ConstantEval(){
    struct unique_type{
        using type = Type;
    } val;
    return val;
}

namespace Meta
{
    template <typename T> class Flag
    {
        struct Dummy
        {
            constexpr Dummy() {}
            friend constexpr void adl_flag(Dummy);
        };

        template <bool> struct Writer
        {
            friend constexpr void adl_flag(Dummy) {}
        };

        template <class Dummy, ul = (adl_flag(Dummy{}),0ul)>
        static constexpr bool Check(ul)
        {
            return true;
        }

        template <class Dummy>
        static constexpr bool Check(short)
        {
            return false;
        }

    public:
        template <class Dummy = Dummy, bool Value = Check<Dummy>(0ul), ul = sizeof(Writer<Value && 0>)>
        static constexpr ul ReadSet()
        {
            return Value;
        }

        template <class Dummy = Dummy, bool Value = Check<Dummy>(0ul)>
        static constexpr ul Read()
        {
            return Value;
        }
    };

    template <typename PrimaryTag, ul primary_id>
    struct FunctionGenerator
    {
        template <ul secondary_id>
        struct FlagCheck
        {
            template <typename TagType>
            constexpr friend auto function_lookup(FlagCheck, TagType);
        };

        template <ul secondary_id>
        struct FlagGet
        {
            template <typename TagType>
            constexpr friend auto function_lookup(FlagGet, TagType);
        };

        template <ul secondary_id, auto Value>
        struct FunctionWriter
        {
            template <typename TagType>
            constexpr friend auto function_lookup(FlagGet<secondary_id>, TagType){return Value;}

            template <typename TagType>
            constexpr friend auto function_lookup(FlagCheck<secondary_id>, TagType){return 0ul;};
        };

        template <
                ul secondary_id,
                typename TagType,
                typename = decltype(function_lookup(FlagCheck<secondary_id>{}, TagType{}))
        >
        constexpr static bool exists(ul){return true;}

        template <
                ul secondary_id,
                typename TagType
        >
        constexpr static bool exists(float){return false;}

        template <
                ul secondary_id,
                typename TagType,
                typename = decltype(function_lookup(FlagCheck<secondary_id>{}, typename decltype(ConstantEval<TagType>)::type{}))
        >
        constexpr static auto fetch(ul)
        {
            return function_lookup(FlagGet<secondary_id>{}, typename decltype(ConstantEval<TagType>)::type{});
        }

        template <
                ul secondary_id,
                typename TagType>
        constexpr static auto fetch(float)
        {
            return false;
        }
    };

    template <typename T,ul I> struct Tag {};

    template<typename T,ul N,bool B>
    struct Checker{
        static constexpr ul currentval() noexcept{
            return N;
        }
    };

    template<typename T,ul N>
    struct CheckerWrapper{
        template<bool B=Flag<Tag<T,N>>::Read(),ul M=Checker<T,N,B>::currentval()>
        static constexpr ul currentval(){
            return M;
        }
    };

    template<typename T,ul N>
    struct Checker<T,N,true>{
        template<ul M=CheckerWrapper<T,N+1>::currentval()>
        static constexpr ul currentval() noexcept{
            return M;
        }
    };

    template<typename T,ul N,bool B=Flag<Tag<T,N>>::ReadSet()>
    struct Next{
        static constexpr ul value() noexcept{
            return N;
        }
    };

    template <typename T> class TaggedCounter
    {
    public:
        template <ul N=CheckerWrapper<T,0>::currentval()> static constexpr ul NextValue(){
            return Next<T,N>::value();
        }
    };


    template <typename TagType, ul ID>
    struct ControlledCounter
    {
        template <
                bool Increment = false,
                typename Unique_ = decltype(ConstantEval<ControlledCounter>()),
                typename T = typename Unique_::type,
                ul N = CheckerWrapper<T, 0>::currentval()>
        constexpr static ul CurrentValue()
        {
            if constexpr(Increment){
                return Next<T, N>::value() + 1ul;
            } else{
                return N;
            }
        }
    };
}

#endif //COMPILER_CONTAINERS_COUNTER_H
