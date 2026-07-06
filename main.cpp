#include <print>
#include <coro/pool/event_pool.hpp>
#include <coro/task/task.hpp>

coro::Task<int> do_math(const int x) {
    std::println("[do_math] Начали считать...");
    co_await coro::AsyncSleep{std::chrono::milliseconds(100)};
    std::println("[do_math] Посчитали!");
    co_return x * 2;
}

coro::Task<void> main_logic() {
    std::println("[main] Старт");

    std::println("[main] Ждем результат do_math...");
    const auto result = co_await do_math(21);
    std::println("[main] Получили результат: {}", result);

    std::println("[main] Идем спать на 500мс...");
    co_await coro::AsyncSleep{std::chrono::milliseconds(500)};
    std::println("[main] Проснулись! Завершаемся.");

    co_return;
}

coro::Task<void> some() {
    std::println("[some] Начали считать...");
    co_await coro::AsyncSleep{std::chrono::milliseconds(100)};
    std::println("[some] Посчитали!");
    co_return;
}

int main() {
    auto task = main_logic();
    auto sm = some();

    coro::g_loop.schedule(task.Handle());
    coro::g_loop.schedule(sm.Handle());

    std::println("--- Запуск Event Loop ---");
    coro::g_loop.run();
    std::println("--- Event Loop завершен ---");
}