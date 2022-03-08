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

    void eve3
    (const uint64_t mod, const uint64_t max_val, const unsigned int thread_count) {
        uint64_t init = mod;
        while (init < max_val) {
            auto tmp = init;
            while (!counter.compare_exchange_weak(init, init + 1)) {
                init = tmp; // restore old value, if exchanged failed and init was set to counter
            };
            init += thread_count;
            if (init >= max_val) {
                break;
            }
        }
    }
}

int main(int argc, char * argv[])
{
    try {
        unsigned int thread_count = 2;
        auto max_thread_count = std::max(std::thread::hardware_concurrency(), thread_count) ;

        if (argc != 3) {
            std::cerr << "Usage: 1b_cas number (min 100, max 10`000`000`000) thread_count (min 2, max " << max_thread_count << ").\n";
            return 1;
        }
        uint64_t param = std::atoll(argv[1]);
        if((param < 100) || (param > 10000000000)) {
            std::cout << "Number exceeds range [100, 10`000`000`000], setting to default 100.\n";
            max_value = 100;
        } else {
            max_value = param;
        }

        unsigned int param_thread_count = std::atol(argv[2]);

        if ((param_thread_count < 2) || (param_thread_count > max_thread_count) || (param_thread_count % 2==1)) {
            std::cout << "thread_count must be even (2,4,8...) and not exceed range [ 2, " << max_thread_count << " ], setting to default 2.\n";
        } else {
            thread_count = param_thread_count;
        }

        std::cout << "Started.\n";

        std::vector <std::thread> threadPool;
        threadPool.reserve(thread_count);

        for(uint64_t i = 0; i < thread_count; ++i) {
            threadPool.emplace_back(std::thread(even, i, max_value, thread_count));
        }
    for(int i = 0; i < threadPool.size(); ++i) {
        threadPool[i].join();
    }

    assert(counter == max_value);

    std::cout << "Finished.\n";
    return 0;
}
    catch (std::exception &exc) {
        std::cerr << "Exception has been thrown:" << exc.what();
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception has been thrown:";
        return 1;
    }
}

