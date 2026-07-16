/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

#include <print> // for std::println
#include <chrono> // for std::chrono::milliseconds
#include <coroutine> // for std::coroutine_traits

import coro.task;
import coro.event_pool;
import coro.async_sleep;

coro::EventPool pool;

coro::Task<void, coro::EventPool> first() {
    std::println("[first] started");

    std::println("[first] large calculation started");
    co_await coro::AsyncSleep{std::chrono::milliseconds{100'00}, pool};
    std::println("[first] large calculation finished");

    std::println("[first] finished");
}

coro::Task<int, coro::EventPool> third(const int value) {
    std::println("[third] started");

    std::println("[third] large calculation started");
    co_await coro::AsyncSleep{std::chrono::milliseconds{300'00}, pool};
    std::println("[third] large calculation finished");

    std::println("[third] finished");

    co_return value * 2;
}

coro::Task<void, coro::EventPool> second() {
    std::println("[second] started");

    std::println("[second] calling third with 26");
    auto res = co_await coro::Task{third(26), pool};
    std::println("[second] third finished with result: {}", res);

    std::println("[second] finished");
}

int main() {
    auto fst = coro::Task{first(), pool};
    auto snd = coro::Task{second(), pool};

    pool.add_task(fst);
    pool.add_task(snd);

    std::println("Event pool started");
    pool.run();
    std::println("Event pool finished");
}