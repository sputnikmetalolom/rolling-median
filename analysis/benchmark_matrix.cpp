#include "rolling_median.h"

#include <complex>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <type_traits>

// Wrap complex numbers and cache their absolute value
template <typename T>
struct ComplexAbs {
    explicit ComplexAbs(const std::complex<T>& z)
        : value(z), abs(std::abs(z)) {}

    ComplexAbs() = default;

    [[nodiscard]] T getAbs() const { return abs; }

    std::complex<T> value{};
    T abs{};
};

// Arithmetic and I/O support for ComplexAbs
template <typename T>
ComplexAbs<T> operator+(const ComplexAbs<T>& a, const ComplexAbs<T>& b) {
    return ComplexAbs<T>{a.value + b.value};
}

template <typename T>
ComplexAbs<T> operator/(const ComplexAbs<T>& a, double divisor) {
    return ComplexAbs<T>{a.value / divisor};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const ComplexAbs<T>& ca) {
    os << "(" << ca.value.real() << "," << ca.value.imag() << ")";
    return os;
}

// Comparison for sorting by absolute value
template <typename T>
struct CompareAbs {
    bool operator()(const ComplexAbs<T>& a, const ComplexAbs<T>& b) const {
        return a.getAbs() < b.getAbs();
    }
};

// Uniform distribution type based on value type
template <typename T>
using distribution = std::conditional_t<
    std::is_integral_v<T>,
    std::uniform_int_distribution<T>,
    std::uniform_real_distribution<T>
>;

// Generate complex numbers with uniform components
template <typename T>
std::vector<ComplexAbs<T>> make_numbers(std::size_t count) {
    std::vector<ComplexAbs<T>> numbers;
    numbers.reserve(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    distribution<T> dist(-1'000'000, 1'000'000);

    for (std::size_t i = 0; i < count; ++i) {
        numbers.emplace_back(std::complex<T>{dist(gen), dist(gen)});
    }

    return numbers;
}

// Benchmark a specific mode and size
template <std::size_t SIZE, kallkod::Mode MODE>
void run_benchmark(const std::vector<ComplexAbs<double>>& numbers, std::ofstream& out) {
    using T = double;
    using ComplexMedian = kallkod::RollingMedian<MODE, ComplexAbs<T>, SIZE, CompareAbs<T>>;

    ComplexMedian median;

    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& d : numbers) {
        median.update(d);
        [[maybe_unused]] volatile auto m = median.median();
    }
    auto end = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(end - start).count();

    const char* mode_str =
        MODE == kallkod::Mode::Static   ? "Static" :
        MODE == kallkod::Mode::Dynamic  ? "Dynamic" :
        MODE == kallkod::Mode::Reference ? "Reference" : "Unknown";

    out << SIZE << "," << numbers.size() << "," << mode_str << "," << elapsed << "\n";
    std::cout << "Last median: " << median.median().value << std::endl;
}

// Sanity check that different modes give the same median
template <std::size_t SIZE>
void verify_modes_match(const std::vector<ComplexAbs<double>>& numbers) {
    using T = double;
    using V = ComplexAbs<T>;

    kallkod::RollingMedian<kallkod::Mode::Static, V, SIZE, CompareAbs<T>> static_median;
    kallkod::RollingMedian<kallkod::Mode::Dynamic, V, SIZE, CompareAbs<T>> dynamic_median;
    kallkod::RollingMedian<kallkod::Mode::Reference, V, SIZE, CompareAbs<T>> reference_median;

    for (const auto& d : numbers) {
        static_median.update(d);
        dynamic_median.update(d);
        reference_median.update(d);
    }

    auto s = static_median.median();
    auto d = dynamic_median.median();
    auto r = reference_median.median();

    std::cout << " Static: " << s.value << "\n";
    std::cout << " Dynamic: " << d.value << "\n";
    std::cout << " Reference: " << r.value << "\n";
}

int main() {
    using T = double;

    // Example: verify_modes_match<9>(make_numbers<T>(21000));

    std::ofstream out("benchmark_results.csv");
    out << "SIZE,DataCount,Mode,ElapsedSeconds\n";

    #include "benchmark_calls.inc"  // This should contain run_benchmark<SIZE, MODE>(...);

    out.close();
    return 0;
}
