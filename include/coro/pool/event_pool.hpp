#pragma once

#include <chrono>
#include <coroutine>
#include <queue>
#include <bits/this_thread_sleep.h>

#include <coro/task/task_concept.hpp>

namespace coro {

class EventPool {
public:

    using clock_t = std::chrono::steady_clock;
    using timepoint_t = clock_t::time_point;

    // Adding coroutine descriptor into queue.
    void Schedule(std::coroutine_handle<> handle);

    template <typename T, typename U = EventPool>
        requires task_type<T, U>
    void Schedule(T&& task) {
        task.Attach(*this);
    }

    // Pushes coroutine descriptor into waiting coroutines
    // with their wake_time.
    void Wait_until(std::coroutine_handle<> handle, const timepoint_t& wake_time);

    void Run();

private:
    std::queue<std::coroutine_handle<>> ready_queue_;

    struct TimeEntry {
        timepoint_t wake_time;
        std::coroutine_handle<> handle;
    };

    std::vector<TimeEntry> waiting_timers_;
};

} // namespace coro