#include <coro/awaiters/async_sleep.hpp>
#include <coro/task/task.hpp>

namespace coro {

bool AsyncSleep::await_ready() const noexcept {
    return duration.count() <= 0;
}

void AsyncSleep::await_suspend(const std::coroutine_handle<> h) noexcept {
    const auto promise = std::coroutine_handle<Task<void, EventPool>::promise_type>::from_address(
                                h.address())
                                    .promise();
    const auto wake_time = EventPool::clock_t::now() + duration;
    promise.pool_->Wait_until(h, wake_time);
}

void AsyncSleep::await_resume() noexcept {

}

} // namespace coro
