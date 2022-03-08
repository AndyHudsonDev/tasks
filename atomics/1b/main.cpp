#include <vector>
#include <thread>
#include <atomic>
#include <cassert>
#include <iostream>
#include <cstdlib>

/* Following code is for task 1b. Task 1a is in separate file, though task 1a can be solved with this solution as well,
* just with thread = 2 parameter provided.
*/

namespace {

    // Test and test-and-set (TTAS) lock
    class spinlock {
    private:
        std::atomic<bool> locked = {false};
    public:
        void lock() {
            for (;;) {
                if (!locked.exchange(true, std::memory_order_acquire)) {
                    break;
                }
                while (locked.load(std::memory_order_relaxed));
            }
        }
        void unlock() {
            locked.store(false, std::memory_order_release);
        }
    };

    uint64_t max_value{10000000}; // passed to thread by copy (const specifier added)
    volatile uint64_t counter{0}; // to be incremented

    spinlock  sl;

    /*  function even increments based on mod parameter, so there threads that
    *   pass mod = 1 or 0, based on it and if counter variable is even or odd
    *   counter is incremented or not
    */
    void even(const uint64_t mod, const uint64_t max_val) {
        while (counter < max_val) {
            if((counter % 2) == mod) {
                sl.lock();
                if ((counter % 2) == mod && counter < max_val) {
                    ++counter;
                    assert(counter % 2 == (mod == 1 ? 0 : 1));
                }
                sl.unlock();
            }
            if(!(counter < max_val)) {
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
        std::cerr << "Usage: Mill1b number (min 100, max 10`000`000`000) thread_count (min 2, max " << max_thread_count << ").\n";
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

    if ((param_thread_count < 2) || (param_thread_count > max_thread_count)) {
        std::cout << "thread_count exceeds range [ 2, " << max_thread_count << " ], setting to default 2.\n";
    } else {
        thread_count = param_thread_count;
    }

    std::cout << "Started.\n";

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
catch (std::exception &exc) {
    std::cerr << "Exception has been thrown:" << exc.what();
    return 1;
}
catch (...) {
    std::cerr << "Unknown exception has been thrown:";
    return 1;
}
}
