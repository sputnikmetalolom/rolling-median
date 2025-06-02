/*
Copyright 2025 Kallkod Oy

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <random>

#include "rolling_median.h"
#include <gtest/gtest.h>

using namespace kallkod;

TEST(RollingMedian, DefaultConstructor)
{
    constexpr RollingMedian<int, 5> uut;
    EXPECT_EQ(uut.median(), 0);
}

TEST(RollingMedian, Initialization)
{
    const RollingMedian<int, 5> uut{{1, 2, 3, 4, 5}};
    EXPECT_EQ(uut.median(), 3);

    static_assert(std::is_same_v<decltype(uut)::api_type, int>);
}

TEST(RollingMedian, Increase)
{
    RollingMedian<int, 5> uut5{};
    RollingMedian<int, 3> uut3{};
    for (const int n: {1, 2, 3, 4, 5, 6})
    {
        uut5.update(n);
        uut3.update(n);
    }
    EXPECT_EQ(uut5.median(), 4);
    EXPECT_EQ(uut3.median(), 5);
}

TEST(RollingMedian, Invert)
{
    [[maybe_unused]] auto compare = [](int a, int b) { return b < a; };
    RollingMedian<int, 5, decltype(compare)> uut5;
    for (const int n: {1, 2, 3, 4, 5, 6})
    {
        uut5.update(n);
    }
    EXPECT_EQ(uut5.median(), 4);
}

TEST(RollingMedian, Decrease)
{
    RollingMedian<int, 5> uut5{};
    RollingMedian<int, 3> uut3{};
    for (const int n: {6, 5, 4, 3, 2, 1, 0})
    {
        uut5.update(n);
        uut3.update(n);
    }
    EXPECT_EQ(uut5.median(), 2);
    EXPECT_EQ(uut3.median(), 1);
}

TEST(RollingMedian, Random)
{
    constexpr int N = 1'000'000;
    std::vector<int> numbers;
    numbers.reserve(N);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-1'000'000, 1'000'000);
    for (int i = 0; i < N; ++i) {
        numbers.push_back(dist(gen));
    }

    constexpr size_t WINDOW_SIZE = 11;
    std::array<int, WINDOW_SIZE> data{};
    std::copy(numbers.end() - WINDOW_SIZE, numbers.end(), data.begin());
    std::ranges::sort(data);

    RollingMedian<int, WINDOW_SIZE> uut;
    for (const auto n: numbers)
    {
        uut.update(n);
    }

    EXPECT_EQ(uut.median(), data.at(WINDOW_SIZE / 2)) << uut.toString();
}

struct CustomType
{
    explicit CustomType(int v) : value(v) {}
    CustomType() = default;
    int value{};
};

struct CompareCustomType
{
    bool operator()(const CustomType& lhs, const CustomType& rhs) const
    {
        return lhs.value < rhs.value;
    }
};

TEST(RollingMedian, CustomType)
{
    RollingMedian<CustomType, 5, CompareCustomType> uut;
    for (const int n: {0, 1, 1, 2, 3, 5, 8})
    {
        uut.update(CustomType{n});
    }
    EXPECT_EQ(uut.median().value, 3);
}
