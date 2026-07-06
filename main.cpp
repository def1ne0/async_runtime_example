#include <iostream>
#include <coro/pool/event_pool.hpp>
#include <coro/task/task.hpp>

coro::Task<int> do_math(const int x) {
    std::cout << "[do_math] Начали считать...\n";
    co_await coro::AsyncSleep{std::chrono::milliseconds(100)};
    std::cout << "[do_math] Посчитали!\n";
    co_return x * 2;
}

coro::Task<int> main_logic() {
    std::cout << "[main] Старт\n";

    std::cout << "[main] Ждем результат do_math...\n";

    const auto result = co_await do_math(21);

    std::cout << "[main] Получили результат: " << result << "\n";

    std::cout << "[main] Идем спать на 500мс...\n";
    co_await coro::AsyncSleep{std::chrono::milliseconds(500)};

    std::cout << "[main] Проснулись! Завершаемся.\n";

    co_return 0;
}

coro::Task<char> some() {
    std::cout << "[some] Начали считать...\n";
    co_await coro::AsyncSleep{std::chrono::milliseconds(100)};
    std::cout << "[some] Посчитали!\n";
    co_return 'A';
}

int main() {
    auto task = main_logic();
    auto sm = some();

    coro::g_loop.schedule(task.Handle());
    coro::g_loop.schedule(sm.Handle());

    std::cout << "--- Запуск Event Loop ---\n";
    coro::g_loop.run();
    std::cout << "--- Event Loop завершен ---\n";
}