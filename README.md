#  Rolling Median Library

A high-performance C++ header-only library for calculating the rolling (sliding window) median.

---

##  Features

- Header-only — easy to include in any project
- Multiple modes:
  - `Static`: fast for small, fixed-size windows
  - `Dynamic`: scalable for large windows
  - `Auto`: auto-selects based on size
- Supports custom types and comparators
- Benchmark suite included

---

##  Public API

```cpp
template<
    typename T,             // value type
    std::size_t SIZE,       // window size
    typename Compare = std::less<T>,
    typename Equal = std::equal_to<T>,
    kallkod::Mode mode = kallkod::Mode::Auto
>
class RollingMedian;
```


##  Method Table

| Method                                | Description                        |
|--------------------------------------|------------------------------------|
| `void update(const T& v)`            | Add a new value into the window    |
| `T median() const`                   | Return the current median          |
| `std::size_t size() const`           | Return the number of values stored |
| `void clear()`                       | Reset all internal state           |

---

##  Example

```cpp
#include "rolling_median.h"
#include <iostream>

int main() {
    kallkod::RollingMedian<int, 5> median;

    for (int value : {3, 1, 4, 1, 5}) {
        median.update(value);
        std::cout << "Current median: " << median.median() << '\n';
    }

    return 0;
}
```
