#pragma once

#include <algorithm>
#include <array>
#include <type_traits>
#include <string>

#include <set>
#include <queue>
#include <fmt/format.h>
#include <stdexcept>
#include <iterator>

namespace kallkod {

enum class Mode { Auto, Static, Dynamic };

template <
    typename Value,
    std::size_t SIZE,
    typename Compare = std::less<Value>,
    typename Equal = std::equal_to<Value>,
    Mode mode = Mode::Auto,
    typename = std::enable_if_t<std::is_default_constructible_v<Value>>>
struct RollingMedian
{

    static constexpr std::size_t threshold = 570;
    using value_type = Value;
    static constexpr unsigned window_length = SIZE;
    //static_assert(window_length % 2);

    std::array<value_type, window_length> oldToNew{};
    unsigned index{0};

    std::array<value_type, window_length> smallToBig{};

    std::multiset<value_type, Compare> multisetWindow;
    std::queue<value_type> history;
    typename std::multiset<value_type>::iterator medianIt;

    Compare compare{};
    Equal equal{};

    explicit RollingMedian(const std::array<value_type, window_length>& values, Compare compare = Compare{})
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
        if constexpr(mode == Mode::Static || (mode == Mode::Auto && window_length < threshold)){
            if constexpr (window_length % 2 == 1){
                return smallToBig.at(window_length / 2); 
            } else {
                return (smallToBig.at(window_length / 2 - 1) + smallToBig.at(window_length / 2)) / 2;
            }
        } else {
            if(multisetWindow.empty() || window_length < 2){
                throw std::runtime_error("Multiset is empty or less than 2");
            }
            if(window_length % 2 == 1){
                return *medianIt;
            } else {
                return (*medianIt + *(std::next(medianIt))) / 2;
            }
        }
        
    }
    void shiftAndInsert(const value_type& oldValue, const value_type& newValue) {
        if (compare(oldValue, newValue)) {
            unsigned i = 0;
            while (i < window_length && compare(smallToBig[i], oldValue))
                ++i;
            while (i < window_length - 1 &&
                compare(smallToBig[i], newValue) &&
                compare(smallToBig[i + 1], newValue)) {
                smallToBig[i] = smallToBig[i + 1];
                ++i;
            }
            smallToBig[i] = newValue;
        } else if (compare(newValue, oldValue)) {
            unsigned i = window_length - 1;
            while (i > 0 && compare(oldValue, smallToBig[i]))
                --i;
            while (i > 0 &&
                compare(newValue, smallToBig[i]) &&
                compare(newValue, smallToBig[i - 1])) {
                smallToBig[i] = smallToBig[i - 1];
                --i;
            }
            smallToBig[i] = newValue;
        }
    }

    void updateStatic(input_type newValue) {
        const value_type oldValue = oldToNew.at(index);
        oldToNew.at(index) = newValue;
        index = (index + 1) % window_length;

        shiftAndInsert(oldValue, newValue);
    }

    void updateDynamic(input_type newValue) {
        if (multisetWindow.empty()) {
            multisetWindow.insert(newValue);
            medianIt = multisetWindow.begin();
            history.push(newValue);
            return;
        }

        multisetWindow.insert(newValue);
        history.push(newValue);

        if (compare(newValue, *medianIt)) {
            if (multisetWindow.size() % 2 == 0)
                --medianIt;
        } else {
            if (multisetWindow.size() % 2 == 1)
                ++medianIt;
        }

        if (history.size() > SIZE) {
            value_type old = history.front();
            history.pop();

            if (equal(old, *medianIt)) {
                medianIt = multisetWindow.erase(medianIt);
            } else {
                bool left = compare(old, *medianIt);
                multisetWindow.erase(multisetWindow.find(old));
                if (left)
                    ++medianIt;
                else if (multisetWindow.size() % 2 == 0)
                    --medianIt;
            }
        }
    }


    void update(input_type newValue) {
        if constexpr (mode == Mode::Static || (mode == Mode::Auto && window_length < threshold)) {
            updateStatic(newValue);
        } else {
            updateDynamic(newValue);
        }
    }


    [[nodiscard]] std::string toString() const
    {
        std::string result{"oldToNew: {"};
        for (const auto n:oldToNew)
        {
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        }
        result += "}\nsmallToBig: {";
        for (const auto n:smallToBig)
        {
            fmt::format_to(std::back_inserter(result), "{}, ", n);
        }
        result += "}\n";

        return result;
    }
};

} // namespace kallkod
