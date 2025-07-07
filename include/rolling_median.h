#pragma once

#include <algorithm>
#include <array>
#include <queue>
#include <set>
#include <string>
#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <fmt/format.h>
#include <iostream>

namespace kallkod {

enum class Mode { Auto, Static, Dynamic, Reference };

template <
    Mode mode,
    typename Value,
    std::size_t SIZE,
    typename Compare = std::less<Value>>
struct RollingMedian;

template <
    typename Value,
    std::size_t SIZE,
    typename Compare >
struct RollingMedian<Mode::Static, Value, SIZE, Compare>
{
    using value_type = Value;
    static constexpr std::size_t window_length = SIZE;
    static constexpr std::size_t threshold = 570;

    std::array<value_type, window_length> oldToNew{};
    std::array<value_type, window_length> smallToBig{};
    unsigned index{0};

    Compare compare{};

    explicit RollingMedian(const std::array<value_type, window_length>& values,
                           Compare compare = Compare{})
        : oldToNew(values),
          smallToBig(values),
          compare(compare)
    {
        std::sort(smallToBig.begin(), smallToBig.end(), this->compare);
    }

    RollingMedian() = default;

    using input_type = std::conditional_t<std::is_arithmetic_v<value_type>, value_type, const value_type&>;
    using return_type = value_type;

    [[nodiscard]] return_type median() const {
        if constexpr (window_length % 2 == 1) {
            return smallToBig.at(window_length / 2);
        } else {
            return (smallToBig.at(window_length / 2 -1) + smallToBig.at(window_length / 2)) / 2;
        }
    }

    void shiftAndInsert(const value_type& oldValue, const value_type& newValue) {
        if (compare(oldValue, newValue)) {
            unsigned i = 0;
            while (i < window_length && compare(smallToBig[i], oldValue)) ++i;
            while (i < window_length - 1 &&
                   compare(smallToBig[i], newValue) &&
                   compare(smallToBig[i + 1], newValue)) {
                smallToBig[i] = smallToBig[i + 1];
                ++i;
            }
            smallToBig[i] = newValue;
        } else if (compare(newValue, oldValue)) {
            unsigned i = window_length - 1;
            while (i > 0 && compare(oldValue, smallToBig[i])) --i;
            while (i > 0 &&
                   compare(newValue, smallToBig[i]) &&
                   compare(newValue, smallToBig[i - 1])) {
                smallToBig[i] = smallToBig[i - 1];
                --i;
            }
            smallToBig[i] = newValue;
        }
    }

    void update(input_type newValue) {
        const value_type oldValue = oldToNew.at(index);
        oldToNew.at(index) = newValue;
        index = (index + 1) % window_length;
        shiftAndInsert(oldValue, newValue);
    }

    [[nodiscard]] std::string toString() const {
        std::string result{"oldToNew: {"};
        for (const auto& n : oldToNew)
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        result += "}\nsmallToBig: {";
        for (const auto& n : smallToBig)
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        result += "}\n";
        return result;
    }
};

template <
    typename Value,
    std::size_t SIZE,
    typename Compare >
struct RollingMedian<Mode::Dynamic, Value, SIZE, Compare>
{
    using value_type = Value;
    static constexpr std::size_t window_length = SIZE;
    static constexpr std::size_t threshold = 71;

    std::multiset<value_type, Compare> multisetWindow;
    std::queue<value_type> history;
    typename std::multiset<value_type>::iterator medianIt;

    Compare compare{};

    bool is_equal(const Value& a, const Value& b) const {
        return !compare(a, b) && !compare(b, a);
    }

    RollingMedian() = default;

    using input_type = std::conditional_t<std::is_arithmetic_v<value_type>, value_type, const value_type&>;
    using return_type = value_type;

    [[nodiscard]] return_type median() const {
        if (multisetWindow.empty() || window_length < 2)
            throw std::runtime_error("Multiset is empty or too small");

        if (multisetWindow.size() % 2 == 1) {
            return *medianIt;
        } else {
            return (*medianIt + *(std::next(medianIt))) / 2;
        }
    }

    void update(input_type newValue) {
        if (multisetWindow.empty()) {
            multisetWindow.insert(newValue);
            medianIt = multisetWindow.begin();
            history.push(newValue);
            return;
        }
        multisetWindow.insert(newValue);
        history.push(newValue);

        if (history.size() > SIZE) {
            value_type old = history.front();
            history.pop();

            bool left_old = compare(old, *medianIt);                // old <  medianIt
            bool left_newValue = compare(newValue, *medianIt);      // newValue < medianIt
            bool right_old = compare(*medianIt, old);                // old > medianIt
            bool right_newValue = compare(*medianIt, newValue);      // newValue > medianIt

            if(is_equal(old, *medianIt)){
                medianIt = multisetWindow.erase(medianIt);
                if(left_newValue){
                    medianIt = std::prev(medianIt);
                }
            }
            else {
                multisetWindow.erase(multisetWindow.find(old));
                if(left_old && right_newValue) ++medianIt;
                if(left_newValue && right_old) --medianIt;
            }
        } else {
            if (compare(newValue, *medianIt)) {
                if (multisetWindow.size() % 2 == 0) --medianIt; 
            } else {
                if (multisetWindow.size() % 2 == 1) ++medianIt;  
            }
        }
    }

    [[nodiscard]] std::string toString() const {
        std::string result{"MultisetWindow: {"};
        for (const auto& n : multisetWindow)
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        result += "}\n";
        return result;
    }
};

template <
    typename Value,
    std::size_t SIZE,
    typename Compare>
struct RollingMedian<Mode::Auto, Value, SIZE, Compare>
    : std::conditional_t<
        (SIZE < RollingMedian<Mode::Static, Value, SIZE, Compare>::threshold),
        RollingMedian<Mode::Static, Value, SIZE, Compare>,
        RollingMedian<Mode::Dynamic, Value, SIZE, Compare>
      >
{
    using Base = std::conditional_t<
        (SIZE < RollingMedian<Mode::Static, Value, SIZE, Compare>::threshold),
        RollingMedian<Mode::Static, Value, SIZE, Compare>,
        RollingMedian<Mode::Dynamic, Value, SIZE, Compare>
    >;

    using Base::Base;  // inherit constructors
    using Base::update;
    using Base::median;
    using Base::toString;
};

template <
    typename Value,
    std::size_t SIZE,
    typename Compare>
struct RollingMedian<Mode::Reference, Value, SIZE, Compare>
{
    using value_type = Value;
    static constexpr std::size_t window_length = SIZE;

    std::deque<value_type> buffer;

    using input_type = std::conditional_t<std::is_arithmetic_v<value_type>, value_type, const value_type&>;
    using return_type = value_type;

    RollingMedian() = default;

    void update(input_type value) {
        if (buffer.size() >= SIZE) {
            buffer.pop_front();
        }
        buffer.push_back(value);
    }

    [[nodiscard]] return_type median() const {
        if (buffer.empty()) {
            throw std::runtime_error("Multiset is empty or too small");
        }

        std::vector<value_type> sorted(buffer.begin(), buffer.end());
        std::sort(sorted.begin(), sorted.end(), Compare{});

        std::size_t n = sorted.size();
        if (n % 2 == 1) {
            return sorted[n / 2];
        } else {
            return (sorted[n / 2 - 1] + sorted[n / 2]) / 2;
        }
    }

    [[nodiscard]] std::string toString() const {
        std::string result{"Buffer: {"};
        for (const auto& n : buffer)
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        result += "}\n";
        return result;
    }
};

} // namespace kallkod

