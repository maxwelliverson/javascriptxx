//
// Created by maxwell on 2020-01-14.
//

#ifndef COMPILER_CONTAINERS_VALUE_PAIR_H
#define COMPILER_CONTAINERS_VALUE_PAIR_H

#include <iostream>

using id_type = const int*;

template <typename T>
struct unique_id
{
    constexpr static int i = 0;
    constexpr unique_id(const T&){}
    constexpr operator id_type(){return &i;}
};

#define get_unique_id unique_id([]{})

struct NullValue
{
    template <typename T>
    constexpr bool operator==(const T) const {return false;}
    constexpr bool operator==(const NullValue) const {return true;}

    friend std::ostream& operator<<(std::ostream& out_stream, const NullValue&)
    {
        out_stream << "NullValue";
        return out_stream;
    }
};

namespace Inner {

    template <auto Value>
    struct ValueDeclarator
    {
        constexpr friend auto value_fetch(ValueDeclarator<Value>);
    };

    template<auto Val1, auto Val2>
    struct ValMapper {
        constexpr static bool written = true;
        constexpr friend auto value_fetch(ValueDeclarator<Val1>) { return Val2; }
        //constexpr friend auto value_fetch(ValueDeclarator<Val2>) { return Val1; }
    };

    template <auto Value, int = sizeof(decltype(value_fetch(ValueDeclarator<Value>{})))>
    constexpr bool value_check(int){return true;}

    template <auto Value>
    constexpr bool value_check(ushort){return false;}

    template <
            auto V1,
            auto V2,
            int = sizeof(ValueDeclarator<V1>),
            int = sizeof(ValueDeclarator<V2>),
            int = sizeof(ValMapper<V1, V2>)>
    struct ValWriter
    {
        constexpr static ValMapper<V1, V2> obj = {};
        constexpr operator auto(){return obj;}
    };

    template <auto Value, bool = value_check<Value>(-1)>
    struct ValueAccessor;

    template <auto Value>
    struct ValueAccessor<Value, true>
    {
        constexpr static auto value = value_fetch(ValueDeclarator<Value>{});
    };

    template <auto Value>
    struct ValueAccessor<Value, false>
    {
        constexpr static auto value = NullValue{};
    };

    template <auto Value1, auto Value2>
    constexpr auto write_pair()
    {
        if constexpr(value_check<Value1>(-1))
        {
            return false;
        }
        else
        {
            constexpr auto O_w_O = ValWriter<Value1, Value2>::obj;
            return O_w_O.written;
        }
    }

    template <auto Value, auto RetValue = ValueAccessor<Value>::value, bool = std::is_same_v<decltype(RetValue), NullValue>>
    constexpr auto get_paired_value = RetValue;
}

template <typename T>
struct type_value
{
    using type = T;
    constexpr operator T(){return T{};}
    type value();
    static type static_value();

    constexpr type_value() = default;
    constexpr type_value(T){};

    constexpr bool operator ==(const type_value&){return true;};
    template <typename T2>
    constexpr bool operator ==(const type_value<T2>&){return false;}
};


/*
template <auto& lvalue_ref>
struct lvalue_type
{
    using type = decltype(lvalue_ref);
    constexpr operator type(){return lvalue_ref;}
    constexpr lvalue_type() = default;

    template <typename T>
    constexpr lvalue_type(const T& value){}

    static decltype(lvalue_ref) static_value(){return lvalue_ref;}

    constexpr bool operator ==(lvalue_type){return true;};
    template <auto& ref2>
    constexpr bool operator ==(lvalue_type<ref2>){return false;}
};*/

template <auto Val, typename T>
constexpr bool encode_type_func()
{
    constexpr auto encoded_type = type_value<T>{};
    return Inner::write_pair<Val, encoded_type>();
}

template <auto Val>
constexpr auto decode_type_func()
{
    constexpr auto ret_val = Inner::get_paired_value<Val>;
    if constexpr(ret_val == NullValue{})
    {
        return NullValue{};
    }
    else
    {
        return ret_val.static_value();
    }
}

/*template <auto Val, auto& ref>
constexpr bool encode_ref_func()
{
    constexpr auto encoded_ref = lvalue_type(ref);
    return Inner::write_pair<Val, encoded_ref>();
}

template <auto Val>
constexpr auto decode_ref_func()
{
    constexpr auto ret_val = Inner::get_paired_value<Val>;
    if constexpr(ret_val == NullValue{})
    {
        return;
    }
    else
    {
        return ret_val.static_value();
    }
}

template <typename T>
lvalue_type(T& ref) -> lvalue_type<ref>;*/

#define encode_type(value, type) encode_type_func<value, type>()
#define decode_type(value) decltype(decode_type_func<value>())

//#define encode_ref(value__, ref__) encode_ref_func<value__, ref__>()
//#define decode_ref(value__) decode_ref_func<value__>()

#endif //COMPILER_CONTAINERS_VALUE_PAIR_H
