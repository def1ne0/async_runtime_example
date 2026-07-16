/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <coroutine> // for std::coroutine_handle
#include <chrono>    // for std::chrono::steady_clock

module coro.async_sleep;

import coro.task;
import coro.event_pool;

namespace coro {

bool AsyncSleep::await_ready() const noexcept {
    return duration.count() <= 0;
}

void AsyncSleep::await_suspend(const std::coroutine_handle<> h) noexcept {
    const auto promise = std::coroutine_handle<Task<void, EventPool>::promise_type>::from_address(
                                h.address())
                                    .promise();
    const auto wake_time = EventPool::clock_t::now() + duration;
    promise.pool_->wait_until(h, wake_time);
}

void AsyncSleep::await_resume() noexcept {

}

} // namespace coro
