#include <print>
#include <coro/pool/event_pool.hpp>
#include <coro/task/task.hpp>
#include <coro/awaiters/async_sleep.hpp>

coro::EventPool pool;

coro::Task<void, coro::EventPool> first() {
    std::println("[first] started");

    std::println("[first] large calculation started");
    co_await coro::AsyncSleep{std::chrono::milliseconds{100}};
    std::println("[first] large calculation finished");

    std::println("[first] finished");
}

coro::Task<int, coro::EventPool> third(const int value) {
    std::println("[third] started");

    std::println("[third] large calculation started");
    co_await coro::AsyncSleep{std::chrono::milliseconds{300}};
    std::println("[third] large calculation finished");

    std::println("[third] finished");

    co_return value * 2;
}

coro::Task<void, coro::EventPool> second() {
    std::println("[second] started");

    std::println("[second] calling third with 26");
    auto res = co_await coro::Task{third(26), &pool};
    std::println("[second] third finished with result: {}", res);

    std::println("[second] finished");
}

int main() {
    const auto fst = coro::Task{first(), &pool};
    const auto snd = coro::Task{second(), &pool};

    pool.Schedule(fst);
    pool.Schedule(snd);

    std::println("Event pool started");
    pool.Run();
    std::println("Event pool finished");
}