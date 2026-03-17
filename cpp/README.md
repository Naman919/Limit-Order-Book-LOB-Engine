# C++ Execution Core

This directory contains the high-performance core of the matching engine. It is written in **C++17** with a focus on minimizing heap allocations and maximizing CPU cache hits.

## 🛠 Technical Implementation
* **Price Levels:** We use `std::map<int, Limit>` to maintain a sorted order of price levels. The Bid side uses `std::greater<int>` to ensure the "Best Bid" is always at `begin()`.
* **Time Priority:** Within each price level, orders are stored in a `std::list`. This provides **true $O(1)$ removal** for cancellations.
* **The Lookup Map:** An `std::unordered_map` stores `Order ID -> iterator` pairs. This allows the engine to "snip" a cancelled order out of the middle of a list without traversing it.

## 🏗 Build & Run
To achieve the benchmark speeds, you **must** compile with high optimization levels:

```bash
# Compile with O3 and Native Architecture optimizations
g++ -std=c++17 -O3 -march=native main.cpp -o engine

# Run the benchmark
./engine
