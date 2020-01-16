//#include <functional>
#include <vector>
#include "container.h"


struct
{
    static void log(auto... args)
    {
        (std::cout << ... << args) << std::endl;
    }
}console;


template <typename T>
struct TestType
{
    const T val;
    constexpr TestType() : val(0){};
    constexpr TestType(T v) : val(v){}
    constexpr bool operator==(const TestType& other)
    {
        return val == other.val;
    }
    //constexpr auto operator<=>(const TestType&) const = default;

    void set_ptr(T* ptr)
    {
        other_val = ptr;
    }
    T* get_ptr()
    {
        return other_val;
    }

    friend std::ostream& operator<<(std::ostream& out_stream, const TestType& tt)
    {
        out_stream << "TestType{" << tt.val << "}";
        return out_stream;
    }

private:
    T* other_val = nullptr;
};

template <id_type = get_unique_id, ul Val = Meta::TaggedCounter<int>::NextValue()>
constexpr ul counter() {
    return Val;
}

int main() {

    var A = TestType{"Hello"};
    var B = 3;
    var C = 2.4;

    console.log("Value of A: ", *A);
    console.log("Value of B: ", *B);
    console.log("Value of C: ", *C);
    print_variable_type(*A);
    print_variable_type(*B);
    print_variable_type(*C);
    console.log("\n");

    ////

    A = 2.4;
    B = TestType{"Hello"};
    C = 3;

    console.log("Value of A: ", *A);
    console.log("Value of B: ", *B);
    console.log("Value of C: ", *C);
    print_variable_type(*A);
    print_variable_type(*B);
    print_variable_type(*C);
    console.log("\n");

    ////

    var V = std::tuple{3.4, 7, "Testing Testing", "12", '3'};

    A = std::vector{5, std::pair{0x5f, -0.0293f}};
    B = *V;
    C = B;

    console.log("Value of C[0]: ", std::get<0>(*C));
    console.log("Value of C[1]: ", std::get<1>(*C));
    console.log("Value of C[2]: ", std::get<2>(*C));

    print_variable_type(*A);
    print_variable_type(*B);
    print_variable_type(*C);

    A = TestType{*B};
    A->set_ptr(V);
    auto test = *A;

    console.log("Value of &*V: ", &*V);
    console.log("Value of test: ", test.get_ptr());


    return 0;
}
