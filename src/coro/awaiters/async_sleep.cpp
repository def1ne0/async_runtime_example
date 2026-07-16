/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <coroutine> // for std::coroutine_handle
#include <chrono>    // for std::chrono::milliseconds

module coro.async_sleep;

import coro.task;
import coro.event_pool;

namespace coro {

AsyncSleep::AsyncSleep(const std::chrono::milliseconds duration, EventPool& pool)
    : pool_(pool), duration_(duration) {}

bool AsyncSleep::await_ready() const noexcept {
    return duration_.count() <= 0;
}

void AsyncSleep::await_suspend(const std::coroutine_handle<> h) noexcept {
    const auto wake_time = EventPool::clock_t::now() + duration_;
    pool_.wait_until(h, wake_time);
}

void AsyncSleep::await_resume() noexcept {}

} // namespace coro
