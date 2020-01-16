//
// Created by maxwell on 2020-01-14.
//

#ifndef COMPILER_CONTAINERS_CONCEPT_TYPES_H
#define COMPILER_CONTAINERS_CONCEPT_TYPES_H

#include <type_traits>

namespace pointers
{

    template <bool B>
    constexpr static bool constant_bool_value = B;

    template <bool B>
    constexpr static bool constant_true_value = true;

    template <typename T1, typename T2>
    concept IsTagType = std::is_convertible_v<T1, T2> && (std::is_empty_v<T1> || std::is_empty_v<T2>);

    template <typename T>
    concept ConceptType = requires
    {
        std::is_convertible_v<T, bool> || constant_true_value<T::value>;
        std::is_empty_v<T>;
    };

    template <typename T>
    struct NoConstraints
    {
        constexpr static bool value = true;
    };

    template <typename T>
    constexpr bool SatisfiedConstraint = false;

    template <ConceptType T>
    constexpr bool SatisfiedConstraint<T> = true;




    struct Tag1{};
    struct Tag2{};
    struct NonEmptyTag{int i; double d;};

    template <template <typename> typename Concept> requires ConceptType<Concept<void>>
    struct RestrainedType
    {
        int i;

        template <typename T2>
        RestrainedType(T2 val) requires SatisfiedConstraint<Concept<T2>>
        {
            i = val;
        }
    };

    template <typename B1, typename B2, typename ...BN>
    constexpr bool same_as_any = []()constexpr -> bool
{
    if constexpr(std::is_same_v<B1, B2>)
{
    return true;
} else
{
if constexpr(sizeof...(BN) == 0)
{
return false;
} else
{
return same_as_any<B2, BN...>;
}
}
}();


template <typename Result, typename Func, typename ...Args>
concept Invokable = requires(Func f, Args ...args)
{
    std::is_convertible<Result, decltype(f(args...))>::value;
};

template <typename Constructor, typename ...Args>
concept Constructable = requires
{
    Invokable<Constructor, Constructor(Args...), Args...>;
};

template <typename T1, typename T2>
concept NotType = !std::is_same_v<T1, T2>;

template <typename T1, typename ...T2>
concept DifferentTypes = !same_as_any<T1, T2...>;

struct NoInit{};

constexpr NoInit dont_initialize = NoInit{};

}

#endif //COMPILER_CONTAINERS_CONCEPT_TYPES_H
