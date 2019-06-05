#include "tp/async.h"
#include <algorithm>
#include <cstdlib>
#include <future>
#include <iostream>
#include <list>
#include <thread>
#include <type_traits>

void poolFun()
{
    int x = 0;
    for (auto j = 0; j < 1000; ++j) {
        std::vector<std::future<int>> futures;
        for (auto i = 0; i < 100; ++i) {
            futures.emplace_back(tp::async([=] { return x + i + 42; }));
            x = i;
        }
    }
}

void asyncFun()
{
    int x = 0;
    for (auto j = 0; j < 1000; ++j) {
        for (auto i = 0; i < 100; ++i) {
            std::vector<std::future<int>> futures;
            futures.emplace_back(std::async(std::launch::async, [=] { return x + i + 42; }));
            x = i;
        }
    }
}


int main()
{
    {
        auto begin = std::chrono::steady_clock::now();
        asyncFun();
        auto end = std::chrono::steady_clock::now();
        std::cout << "std::async time elapsed (ms): "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                  << std::endl;
    }
    {
        auto begin = std::chrono::steady_clock::now();
        poolFun();
        auto end = std::chrono::steady_clock::now();
        std::cout << "tp::async time elapsed (ms): "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                  << std::endl;
    }

    return 0;
}