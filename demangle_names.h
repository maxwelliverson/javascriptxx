//
// Created by maxwell on 2020-01-15.
//

#ifndef COMPILER_CONTAINERS_DEMANGLE_NAMES_H
#define COMPILER_CONTAINERS_DEMANGLE_NAMES_H

#include <cxxabi.h>
#include <utility>
#include <memory>
#include <iostream>

std::shared_ptr<char> cppDemangle(const char *abiName)
{
    int status;
    char *ret = abi::__cxa_demangle(abiName, 0, 0, &status);

    /* NOTE: must free() the returned char when done with it! */
    std::shared_ptr<char> retval;
    retval.reset( (char *)ret, [](char *mem) { if (mem) free((void*)mem); } );
    return retval;
}

#define CLASS_NAME(obj) ((const char *) cppDemangle(typeid(obj).name()).get())
#define CLASS_NAME_PTR(somePointer) ((const char *) cppDemangle(typeid(*somePointer).name()).get())
#define VARIABLE_NAME(value) #value

template <typename T>
void print_typename(T a, const char* name)
{
    printf("Type of %s: %s\n", name, CLASS_NAME(a));
}

#define print_variable_type(var) print_typename(var, VARIABLE_NAME(var))




#endif //COMPILER_CONTAINERS_DEMANGLE_NAMES_H
