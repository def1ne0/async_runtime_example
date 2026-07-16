/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <chrono>    // for std::chrono::steady_clock
#include <coroutine> // for std::coroutine_handle
#include <queue>     // for std::queue

export module coro.event_pool;

import coro.task_concept;

namespace coro {

export class EventPool final {
public:
    using clock_t = std::chrono::steady_clock;
    using timepoint_t = clock_t::time_point;

    // Adding coroutine descriptor into queue.
    void add_coro_handle(std::coroutine_handle<> handle);

    template <typename T, typename U = EventPool>
        requires task_type<T, U>
    void add_task(T&& task) {
        std::move(task).attach(*this);
    }

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

} // namespace coro