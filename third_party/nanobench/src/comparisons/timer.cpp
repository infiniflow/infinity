#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>

/*

To disable buffering in the input program, do e.g. this:


struct InitCout { InitCout() { std::cout.setf(std::ios::unitbuf); } };
InitCout initCout;

*/

int main(int, char**) {
    using Clock = std::chrono::high_resolution_clock;

    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);
    std::cout.precision(6);

    auto begin = Clock::now();

    std::string line;
    while (std::getline(std::cin, line)) {
        auto now = Clock::now();
        auto t = std::chrono::duration<double>(now - begin).count();
        std::cout << std::right << std::setw(12) << t << " | " << line << "\n";
    }

    auto now = Clock::now();
    auto t = std::chrono::duration<double>(now - begin).count();
    std::cout << "-------------|\n"
              << std::right << std::setw(12) << t << " | total" << std::endl;
}
