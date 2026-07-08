#pragma once

#include <coroutine>
#include <memory>
#include <optional>

namespace coro {

class EventPool; // No need to include full include/coro/pool/event_pool.hpp header.

struct basic_promise {
    EventPool* pool_{nullptr};
    std::coroutine_handle<> continuation_{nullptr};
    std::exception_ptr exception_;
};

template <typename T>
class Task {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type final : basic_promise {
        std::optional<T> result_;

        struct final_awaiter;

        auto get_return_object() {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept {
            return final_awaiter{};
        }

        template <typename U>
        void return_value(U&& value) {
            result_.emplace(std::forward<U>(value));
        }

        void unhandled_exception() {
            exception_ = std::current_exception();
        }

        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(const coro_handle_t handle) noexcept {
                if (handle.promise().continuation_) {
                    return handle.promise().continuation_;
                }
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(const std::coroutine_handle<> continuation) noexcept {
        auto caller_promise = std::coroutine_handle<basic_promise>::from_address(
                                    continuation.address())
                                        .promise();

        handle_.promise().pool_ = caller_promise.pool_;
        handle_.promise().continuation_ = continuation;
        handle_.promise().pool_->Schedule(handle_);
    }

    T await_resume() {
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }

        return std::move(handle_.promise().result_.value());
    }

    Task(coro_handle_t h) : handle_(h) {}
    Task(Task&& rhs, EventPool* pool)
        : handle_(std::exchange(rhs.handle_, nullptr))
    {
        if (handle_) {
            handle_.promise().pool_ = pool;
        }
    }
    ~Task() { if (handle_) handle_.destroy(); }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    void Attach(EventPool& pool) const {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
            pool.Schedule(handle_);
        }
    }

private:
    coro_handle_t handle_;
};

template <>
class Task<void> {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type final : basic_promise {
        struct final_awaiter;

        auto get_return_object() {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept {
            return final_awaiter{};
        }

        void return_void() noexcept {}

        void unhandled_exception() {
            exception_ = std::current_exception();
        }

        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(const coro_handle_t handle) noexcept {
                if (handle.promise().continuation_) {
                    return handle.promise().continuation_;
                }
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(const std::coroutine_handle<> continuation) noexcept {
        auto caller_promise = std::coroutine_handle<basic_promise>::from_address(
                                    continuation.address())
                                        .promise();

        handle_.promise().pool_ = caller_promise.pool_;
        handle_.promise().continuation_ = continuation;
        handle_.promise().pool_->Schedule(handle_);
    }

    void await_resume() {
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }
    }

    Task(const coro_handle_t h) : handle_(h) {}
    Task(Task&& rhs, EventPool* pool)
        : handle_(std::exchange(rhs.handle_, nullptr))
    {
        if (handle_) {
            handle_.promise().pool_ = pool;
        }
    }
    ~Task() { if (handle_) handle_.destroy(); }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    void Attach(EventPool& pool) const {
        if (handle_) {
            handle_.promise().pool_ = std::addressof(pool);
            pool.Schedule(handle_);
        }
    }

private:
    coro_handle_t handle_;
};

} // namespace coro


