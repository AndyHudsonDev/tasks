#include <iostream>
#include <functional>
#include <optional>
#include <tuple>
#include <cassert>
#include <algorithm>
#include <queue>

/* 
    Suppose we have a framework that allows us to register functions to be called on price ticks to do some calculations.
    The functions are stateless but the framework can keep some state if required and pass a reference to state variable
    to each call to the function. The way functions provide results of calculations is via a callback provided.
    For example the following function will submit the previous seen value of price to the result callback:
*/

void prev(double price, const std::function<void(double)>& result, std::pair<bool, double>& state) {
    /*
        Here the framework will default initialise and keep a copy of std::pair<bool, double> and pass a non-const
        reference to it to every call into prev. As you can see, the function keeps a boolean flag and a double
        value in the state to provide its funcitonality.
    */
    auto& [ticked, value] = state;

            if (ticked)
            result(value);

            ticked = true;
            value = price;
}

            /*
                    Another example, this one will call result with the first few (as defined by size parameter) price values it is called with
                */

            void take(double price, const std::function<void(double)>& result, size_t& state, const size_t& size) {
        if (state++ < size) {
            result(price);
        }
    }

    /*
    The rest of the file contains a very simplistic test framework and some tests for those functions. Every test consist
    of instantiation of the test_framework object and then running it provided price values and expected result values.
    An expected value of std::nullopt means the function under test is not supposed to make a result call when called
    with the respective value.

    At the end of the file is a main funciton so this file can be compiled and run as a console application to execute the tests.

    ====================================================================================
    Problem Statement
    ====================================================================================

    Your task is to implement function mwa - 'moving window average' that will calculate an average of values in a window
    of given size. For example if size is 3 the function should calculate average of the 3 last price values it was called
    with and call into the result callback with the result. There is already a test for it (test3) so the only thing you
    need to do is to replace comments in mwa declaration with code (and maybe write more tests if you feel like that)

    Please try to provide as optimal implementation as practical and consider your solution's performance and memory usage.\
    
    ====================================================================================
*/

    // Complexity analysis:
    // While every mwa call costs O(1) time complexity and O(1) space,
    // every test case provided with N prices, its complexity is:
    // O(N) time, and O(size) space, where size is a parameter, passed in mwa
    void mwa(double price, const std::function<void(double)>& result,
             std::pair <std::queue<double>, double> & state, const size_t& size) {

        if(size == 0) {
            return;
        }
        if(state.first.empty()) {
            state.second = 0.0;
        }
        state.first.push(price);
        state.second += price;
        if(state.first.size() == size) {
            result(state.second/size);
            state.second -= state.first.front();
            state.first.pop();
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // simple test framework for functions

    namespace test_framework {
    template<typename T, typename... Args>
    bool run(
                std::string_view name,
                const std::vector<double>& input,
                const std::vector<std::optional<double>>& expected,
                void(*function_to_test)(double, const std::function<void(double)>&, T&, const Args&...), Args... args)
    {
        assert(input.size() == expected.size());

        size_t step {0};
        std::optional<double> expected_output;
        size_t step_checked {std::numeric_limits<size_t>::max()};
        size_t failures_detected {0};

        auto receive_result = [&](double r) {
            if (expected_output) {
                if (abs(*expected_output - r) > 2 * std::numeric_limits<double>::epsilon() * std::max(*expected_output, r)) {
                    std::cout << "FAILURE: " << name << ": expected value in step " << step << " was " << *expected_output << " but received " << r << std::endl;
                    ++failures_detected;
                }
            } else {
                std::cout << "FAILURE: " << name << ": no value was expected in step " << step << " but received " << r << std::endl;
                ++failures_detected;
            }
            step_checked = step;
        };

        auto function_to_test_invoker = [=, cb = std::function<void(double)>{receive_result}, state = T{}](double p) mutable {
            function_to_test(p, cb, state, args...);
        };

        for (step = 0; step < input.size(); ++step) {
            expected_output = expected[step];

            function_to_test_invoker(input[step]);

            if (expected_output && step_checked != step) {
                std::cout << "FAILURE: " << name << ": value " << *expected_output << " was expected in step " << step << " but none received" << std::endl;
                ++failures_detected;
            }
        }

        if (failures_detected == 0)
            std::cout << "SUCCESS: " << name << std::endl;

        return failures_detected == 0;
    }
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // sample tests

    void test1()
    {
        test_framework::run("test1",
        {100., 100.1, 100.2, 100.3},
        {std::nullopt, 100., 100.1, 100.2},
                            prev);
    }

    void test2()
    {
        test_framework::run("test2",
        {100., 100.1, 100.2, 100.3, 100.4},
        {100., 100.1, 100.2, std::nullopt, std::nullopt},
                            take, size_t{3});
    }

    void test3() {
        test_framework::run("test3",
        {100., 100.1, 100.2, 100.3, 100.4, 100.5, 100.4, 100.3, 100.2, 100.1},
        {std::nullopt, std::nullopt, std::nullopt, std::nullopt, 100.2, 100.3, 100.36, 100.38, 100.36, 100.3},
                            mwa, size_t{5});
    }

    void test4() {
        test_framework::run("test4",
        {100., 100.1, 100.2, 100.3, 100.4, 100.5, 100.4, 100.3, 100.2, 100.1},
        {100., 100.1, 100.2, 100.3, 100.4, 100.5, 100.4, 100.3, 100.2, 100.1},
                            mwa, size_t{1});
    }

    void test5() {
        test_framework::run("test5",
        {100., 100.1, 100.2, 100.3},
        {std::nullopt, 100.05, 100.15, 100.25},
                            mwa, size_t{2});
    }

    void test6() {
        test_framework::run("test6",
        {100., 100.2, 100.2, 100.},
        {std::nullopt, std::nullopt, std::nullopt, 100.1},
                            mwa, size_t{4});
    }

    void test7() {
        test_framework::run("test7",
        {100., 100.2, 100.2, 100.},
        {std::nullopt, std::nullopt, std::nullopt, std::nullopt},
                            mwa, size_t{5});
    }

    void test8() {
        test_framework::run("test8",
        {100., 100.2, 100.2},
        {std::nullopt, std::nullopt, std::nullopt},
                            mwa, size_t{0});
    }

    int main() {
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
        test7();
        test8();
    }
