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

#include "rolling_median.h"

#include <complex>
#include <initializer_list>
#include <iostream>
#include <random>
#include <vector>

template <typename T>
struct ComplexAbs
{
    explicit ComplexAbs(std::complex<T> const& z)
        : value(z), abs(std::abs(z)) {}
    ComplexAbs() = default;

    [[nodiscard]] T getAbs() const { return abs; }

    std::complex<T> value{};
    T abs{};
};

template <typename T>
ComplexAbs<T> operator+(const ComplexAbs<T>& a, const ComplexAbs<T>& b) {
    return ComplexAbs<T>{a.value + b.value};
}

template <typename T>
ComplexAbs<T> operator/(const ComplexAbs<T>& a, double divisor) {
    return ComplexAbs<T>{a.value / divisor};
}

template <typename T>
struct CompareAbs
{
    bool operator()(const ComplexAbs<T>& a, const ComplexAbs<T>& b) const {
        return a.getAbs() < b.getAbs();
    }
};

template <typename T>
using Distribution = std::conditional_t<std::is_integral_v<T>,
                                        std::uniform_int_distribution<T>,
                                        std::uniform_real_distribution<T>>;

using data_type = double;

template <kallkod::Mode M = kallkod::Mode::Auto>
using ComplexMedian = kallkod::RollingMedian<
    M,
    ComplexAbs<data_type>,
    5,
    CompareAbs<data_type>
>;

int main()
{

    // Static and Dynamic explicitly
    ComplexMedian<kallkod::Mode::Dynamic> median;

    std::initializer_list data = {
        ComplexAbs<data_type>{{1, 0}},
        ComplexAbs<data_type>{{0, 2}},
        ComplexAbs<data_type>{{3, 0}},
        ComplexAbs<data_type>{{0, 4}},
        ComplexAbs<data_type>{{-5, 0}},
    };

    for (const auto& d : data)
    {
        median.update(d);
    }
    std::cout << "Median value: " << median.median().value << std::endl;

    constexpr int N = 4'000'000;
    std::vector<ComplexAbs<data_type>> numbers;
    numbers.reserve(N);

    std::random_device rd;
    std::mt19937 gen(rd());
    Distribution<data_type> dist(-1'000'000, 1'000'000);
    for (int i = 0; i < N; ++i) {
        numbers.emplace_back(std::complex<data_type>{dist(gen), dist(gen)});
    }

    for (const auto& d : numbers)
    {
        median.update(d);
    }
    std::cout << "Median value: " << median.median().value << std::endl;
}