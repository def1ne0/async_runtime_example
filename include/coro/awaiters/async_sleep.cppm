/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <chrono>    // for std::chrono::milliseconds
#include <coroutine> // for coroutine magic

export module coro.async_sleep;

import coro.event_pool;

namespace coro {

export class AsyncSleep final {
    EventPool& pool_;
    std::chrono::microseconds duration_;

public:
    explicit AsyncSleep(std::chrono::milliseconds duration, EventPool& pool);

    bool await_ready() const noexcept;
    void await_suspend(std::coroutine_handle<> h) noexcept;
    void await_resume() noexcept;
};

} // namespace coro