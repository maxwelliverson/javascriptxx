//
// Created by maxwell on 2020-01-15.
//

#ifndef COMPILER_CONTAINERS_USER_DEDUCTION_H
#define COMPILER_CONTAINERS_USER_DEDUCTION_H

template <auto& Val>
struct ValType
{

};

template <typename T>
ValType(T& value) -> ValType<value>;

#endif //COMPILER_CONTAINERS_USER_DEDUCTION_H
