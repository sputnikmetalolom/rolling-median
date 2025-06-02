#pragma once

#include <algorithm>
#include <array>
#include <format>
#include <string>


namespace kallkod {

template <typename Value, std::size_t SIZE, typename Compare = std::less<Value>>
requires std::is_default_constructible_v<Value>
struct RollingMedian
{
    using value_type = Value;
    static constexpr unsigned window_length = SIZE;
    static_assert(window_length % 2);

    std::array<value_type, window_length> oldToNew{};
    unsigned index{0};

    std::array<value_type, window_length> smallToBig{};

    Compare compare{};

    explicit RollingMedian(const std::array<value_type, window_length>& values, Compare compare = Compare{})
        : oldToNew(values),
          smallToBig(values),
          compare(compare)
    {
        std::ranges::sort(smallToBig, compare);
    }

    RollingMedian() = default;

    using api_type = std::conditional_t<std::is_arithmetic_v<value_type>, value_type, const value_type&>;

    [[nodiscard]] api_type median() const { return smallToBig.at(window_length / 2); }

    void update(api_type newValue)
    {
        const value_type oldValue = oldToNew.at(index);
        oldToNew.at(index) = newValue;
        index = (index + 1) % window_length;

        if (compare(oldValue, newValue))
        {
            unsigned i = 0;
            while (i < window_length && compare(smallToBig[i], oldValue))
                ++i;
            while (i < window_length - 1 && compare(smallToBig[i], newValue) && compare(smallToBig[i + 1], newValue))
            {
                smallToBig[i] = smallToBig[i + 1]; // memmove
                ++i;
            }
            smallToBig[i] = newValue;
        }
        else if (compare(newValue, oldValue))
        {
            unsigned i = window_length - 1;
            while (i > 0 && compare(oldValue, smallToBig[i]))
                --i;
            while (i > 0 && compare(newValue, smallToBig[i]) && compare(newValue, smallToBig[i - 1]))
            {
                smallToBig[i] = smallToBig[i - 1];
                --i;
            }
            smallToBig[i] = newValue;
        }
    }

    [[nodiscard]] std::string toString() const
    {
        std::string result{"oldToNew: {"};
        for (const auto n:oldToNew)
        {
            std::format_to(std::back_inserter(result), "{}, ", n);
        }
        result += "}\nsmallToBig: {";
        for (const auto n:smallToBig)
        {
            std::format_to(std::back_inserter(result), "{}, ", n);
        }
        result += "}\n";

        return result;
    }
};

} // namespace kallkod
