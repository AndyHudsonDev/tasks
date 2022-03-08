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

/* The following function will work with 2 threads only. If more threads are added in thread pool, then ca.counter.load
 * can satisfy 2 threads who process only even numbers or only odd numbers, so they both increment and assert will interrupt the app.
 * So this atomic only to properly sync 2 threads, where 1 thread increments atomic counter if it is even
 * and another thread increments if it is odd.
 * The lack of this approach is also 99-100% load on 2 cores, as busy load/read is done.
*/
    void even(uint64_t mod, uint64_t max_val) {
        while (counter < max_val) {
            while ((counter.load(std::memory_order_relaxed) % 2) == mod);
            assert(counter % 2 == (mod == 1 ? 0 : 1));
            if (counter >= max_val) {
                break;
            }
            atomic_fetch_add_explicit(&counter, 1, std::memory_order_relaxed);
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
    constexpr uint64_t thread_count = 2;
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
