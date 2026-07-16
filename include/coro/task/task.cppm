/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <coroutine> // for coroutine magic
#include <memory> // for std::exception_ptr
#include <utility> // for std::exchange
#include <optional> // for std::optional
#include <assert.h> // for assert

export module coro.task;

import coro.pool_concept;

namespace coro {

template <typename P>
struct basic_promise {
    P* pool_{nullptr};
    std::coroutine_handle<> continuation_{nullptr};
    std::exception_ptr exception_{nullptr};
};

export
template <typename T, pool_type Pool>
class Task final {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type final : basic_promise<Pool> {
        using basic_promise<Pool>::exception_;
        using basic_promise<Pool>::continuation_;
        using basic_promise<Pool>::pool_;

        // Use std::optional here to avoid calling constructor of type T
        // while constructor of coroutine is called.
        std::optional<T> result_{};
        struct final_awaiter;

        auto get_return_object() noexcept {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() const noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() const noexcept {
            return final_awaiter{};
        }

        template <typename U>
        void return_value(U&& value) {
            result_.emplace(std::forward<U>(value));
        }

        void unhandled_exception() noexcept {
            exception_ = std::current_exception();
        }

        struct final_awaiter {
            bool await_ready() const noexcept { return false; }

            std::coroutine_handle<> await_suspend(const coro_handle_t handle) const noexcept {
                if (handle.promise().continuation_) {
                    return handle.promise().continuation_;
                }
                return std::noop_coroutine();
            }

            void await_resume() const noexcept {}
        };
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(const std::coroutine_handle<> continuation) noexcept {
        handle_.promise().continuation_ = continuation;

        if (handle_.promise().pool_) {
            handle_.promise().pool_->add_coro_handle(handle_);
        }
    }

    T await_resume() {
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }

        // Result must be constructed already.
        // Otherwise, UB.
        assert(handle_.promise().result_.has_value(),
            "Result must be constructed when coroutine resumes");
        return std::move(*handle_.promise().result_);
    }

    Task(coro_handle_t h) : handle_(h) {}
    Task(Task&& rhs, Pool& pool)
        : handle_(std::exchange(rhs.handle_, nullptr))
    {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
        }
    }
    ~Task() {
        if (handle_) handle_.destroy();
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& rhs) noexcept : handle_(std::exchange(rhs.handle_, nullptr)) {}
    Task& operator=(Task&& rhs) noexcept {
        if (this != &rhs) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(rhs.handle_, nullptr);
        }
        return *this;
    }

    void attach(Pool& pool) && {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
            pool.add_coro_handle(handle_);
            handle_ = nullptr;
        }
    }

private:
    coro_handle_t handle_;
};

template <typename Pool>
class Task<void, Pool> final {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type final : basic_promise<Pool> {
        using basic_promise<Pool>::exception_;
        using basic_promise<Pool>::continuation_;
        using basic_promise<Pool>::pool_;
        struct final_awaiter;

        auto get_return_object() noexcept {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() const noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() const noexcept {
            return final_awaiter{};
        }

        void return_void() const noexcept {}

        void unhandled_exception() noexcept {
            exception_ = std::current_exception();
        }

        struct final_awaiter {
            bool await_ready() const noexcept { return false; }

            std::coroutine_handle<> await_suspend(const coro_handle_t handle) const noexcept {
                if (handle.promise().continuation_) {
                    return handle.promise().continuation_;
                }
                return std::noop_coroutine();
            }

            void await_resume() const noexcept {}
        };
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(const std::coroutine_handle<> continuation) noexcept {
        handle_.promise().continuation_ = continuation;

        if (handle_.promise().pool_) {
            handle_.promise().pool_->add_coro_handle(handle_);
        }
    }

    void await_resume() {
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }
    }

    Task(const coro_handle_t h) : handle_(h) {}
    Task(Task&& rhs, Pool& pool)
        : handle_(std::exchange(rhs.handle_, nullptr))
    {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
        }
    }
    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Task& operator=(Task&& rhs) noexcept {
        if (this != &rhs) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(rhs.handle_, nullptr);
        }
        return *this;
    }

    void attach(Pool& pool) && {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
            pool.add_coro_handle(handle_);
            handle_ = nullptr;
        }
    }

private:
    coro_handle_t handle_;
};

} // namespace coro


