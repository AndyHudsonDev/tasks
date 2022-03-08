#include <vector>
#include <thread>
#include <atomic>
#include <cassert>
#include <iostream>
#include <cstdlib>

// Following code is for task 1 and 1a only. Task 1b will be in a separate file.
namespace {

    std::atomic <uint64_t> counter{0}; // no false sharing, just 1 atomic is handled, so no padding
    uint64_t max_value;

//Thread 1 : even, init = 0, increment init, expecting next = 2,4,6...max_val
//Thread 2 : odd,  init = 1, increment init, expecting next = 3,5,7...max_val
    void even(uint64_t mod, uint64_t max_val) {
        uint64_t init = mod;
        while (init < max_val) {
            auto tmp = init;
            while (!counter.compare_exchange_weak(init, init + 1)) {
                init = tmp; // restore old value, if exchanged failed and init was set to counter
            };
            init += 2;
            if (init >= max_val) {
                break;
            }
        }
    }
}

int main(int argc, char * argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: Mill1a number (min 100, max 10`000`000`000).\n";
        return 1;
    }
    uint64_t param = std::atoll(argv[1]);
    if((param < 100) || (param > 10000000000)) {
        std::cout << "Number exceeds range [100, 10`000`000`000], setting to default 100.\n";
        max_value = 100;
    } else {
        max_value = param;
    }

    std::cout << "Started.\n";
    constexpr auto thread_count = 2;
    std::vector <std::thread> threadPool;
    threadPool.reserve(thread_count);

    for(uint64_t i = 1; i <= thread_count; ++i) {
        threadPool.emplace_back(std::thread(even, (i%2), max_value));
    }
    for(int i = 0; i < threadPool.size(); ++i) {
        threadPool[i].join();
    }

    assert(counter == max_value);

    std::cout << "Finished.\n";
    return 0;
}

