#include "tp/async.h"
#include <algorithm>
#include <cstdlib>
#include <future>
#include <iostream>
#include <list>
#include <thread>
#include <type_traits>

template<typename, typename = void>
struct HasSplice : std::false_type {
};

template<typename Container>
struct HasSplice<
    Container,
    std::void_t<decltype(std::declval<Container>().splice(
        std::declval<typename Container::const_iterator>(), std::declval<Container>()))>>
    : std::true_type {
};

template<typename Container, typename Iter>
std::enable_if_t<!HasSplice<Container>::value>
transfer(Container& destCont, Iter destIt, Container& fromCont, Iter begin, Iter end)
{
    std::move(begin, end, std::inserter(destCont, destIt));
    fromCont.erase(begin, end);
}

template<typename Container, typename Iter>
std::enable_if_t<HasSplice<Container>::value>
transfer(Container& destCont, Iter destIt, Container& fromCont, Iter begin, Iter end)
{
    destCont.splice(destIt, fromCont, begin, end);
}


template<typename AsyncInvoke, typename Container>
Container parallel_quicksort(Container input)
{
    if (input.empty()) {
        return input;
    }
    Container result;
    transfer(result, result.begin(), input, input.begin(), std::next(input.begin()));
    const auto& pivot = *result.begin();

    auto mid_point = std::partition(std::begin(input), std::end(input),
                                    [&](auto&& item) { return item < pivot; });

    Container lower_items;
    transfer(lower_items, lower_items.begin(), input, input.begin(), mid_point);

    AsyncInvoke async;
    auto lower = async(parallel_quicksort<AsyncInvoke, Container>, std::move(lower_items));
    auto higher = parallel_quicksort<AsyncInvoke>(std::move(input));

    auto lower_data = lower.get();
    transfer(result, result.begin(), lower_data, lower_data.begin(), lower_data.end());
    transfer(result, result.end(), higher, higher.begin(), higher.end());
    std::mutex mtx;
    return result;
}

struct TPAsyncInvoker {
    template<typename Fun, typename... Args>
    auto operator()(Fun&& fun, Args&&... args) const
    {
        return tp::async(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }
};

struct StdAsyncInvoker {
    template<typename Fun, typename... Args>
    auto operator()(Fun&& fun, Args&&... args) const
    {
        return std::async(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }
};

struct SeqInvoker {
    template<typename Fun, typename... Args>
    auto operator()(Fun&& fun, Args&&... args) const
    {
        using result_type = decltype(std::forward<Fun>(fun)(std::forward<Args>(args)...));
        std::promise<result_type> prom;
        auto fut = prom.get_future();
        prom.set_value(std::forward<Fun>(fun)(std::forward<Args>(args)...));
        return fut;
    }
};


int main()
{
    std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << std::endl;
    for (int j = 0; j < 10; ++j) {
        std::vector<int> x;
        for (int i = 0; i < 400000; ++i) {
            x.emplace_back(25 + (std::rand() % (63 - 25 + 1)));
        }

        {
            auto begin = std::chrono::steady_clock::now();
            auto z = parallel_quicksort<SeqInvoker>(x);
            auto end = std::chrono::steady_clock::now();
            std::cout << "Seq quicksort \t\t" << z.size() << " items |\ttime elapsed (ms): "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                      << std::endl;
        }
        {
            std::random_shuffle(x.begin(), x.end());
            auto begin = std::chrono::steady_clock::now();
            auto z = parallel_quicksort<StdAsyncInvoker>(x);
            auto end = std::chrono::steady_clock::now();
            std::cout << "std::async quicksort \t" << z.size() << " items |\ttime elapsed (ms): "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                      << std::endl;
        }

        {
            std::random_shuffle(x.begin(), x.end());
            auto begin = std::chrono::steady_clock::now();
            auto z = parallel_quicksort<TPAsyncInvoker>(x);
            auto end = std::chrono::steady_clock::now();
            std::cout << "tp::async quicksort \t" << z.size() << " items |\ttime elapsed (ms): "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                      << std::endl;
        }
        std::cout << "-----------------" << std::endl;
    }
    return 0;
}