#pragma once

#include <chrono>
#include <coroutine>
#include <queue>
#include <bits/this_thread_sleep.h>

namespace coro {

class EventPool {
public:
    using clock_t = std::chrono::steady_clock;
    using timepoint_t = clock_t::time_point;

    // Adding coroutine descriptor into queue.
    void schedule(std::coroutine_handle<> handle);

    // Pushes coroutine descriptor into waiting coroutines
    // with their wake_time.
    void wait_until(std::coroutine_handle<> handle, const timepoint_t& wake_time);

    void run();

private:
    std::queue<std::coroutine_handle<>> ready_queue_;

    struct TimeEntry {
        timepoint_t wake_time;
        std::coroutine_handle<> handle;
    };

    std::vector<TimeEntry> waiting_timers_;
};

inline EventPool g_loop;

} // namespace coro