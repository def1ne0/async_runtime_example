#include <coro/pool/event_pool.hpp>

namespace coro {

void EventPool::schedule(const std::coroutine_handle<> handle) {
    ready_queue_.push(handle);
}

void EventPool::wait_until(const std::coroutine_handle<> handle, const timepoint_t& wake_time) {
    waiting_timers_.push_back({wake_time, handle});
}

void EventPool::run() {
    while (!ready_queue_.empty() || !waiting_timers_.empty()) {
        auto now = clock_t::now();

        for (auto it = waiting_timers_.begin(); it != waiting_timers_.end();) {
            if (it->wake_time <= now) {
                ready_queue_.push(it->handle);
                it = waiting_timers_.erase(it);
            } else {
                ++it;
            }
        }

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

