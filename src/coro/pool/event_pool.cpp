#include <coro/pool/event_pool.hpp>

namespace coro {

void EventPool::Schedule(const std::coroutine_handle<> handle) {
    ready_queue_.push(handle);
}

void EventPool::Wait_until(const std::coroutine_handle<> handle, const timepoint_t& wake_time) {
    waiting_timers_.push_back({wake_time, handle});
}

void EventPool::Run() {
    while (!ready_queue_.empty() || !waiting_timers_.empty()) {
        auto now = clock_t::now();

        // Going through suspended coroutines.
        for (auto it = waiting_timers_.begin(); it != waiting_timers_.end();) {
            // If it's ready (for simplicity, when its wake_time is ready)
            // we push it into ready_queue_ and remove it from waiting coroutines
            if (it->wake_time <= now) {
                ready_queue_.push(it->handle);
                it = waiting_timers_.erase(it);
            } else {
                ++it;
            }
        }

        // If there's some ready coroutines,
        // then resume them.
        if (!ready_queue_.empty()) {
            auto handle = ready_queue_.front();
            ready_queue_.pop();

            if (handle && !handle.done()) {
                handle.resume();
            }
        }

        else if (!waiting_timers_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

} // namespace coro

