# Limit-Order-Book-LOB-Engine

A high-performance Limit Order Book (LOB) implementation designed to demonstrate the trade-offs between **ultra-low latency execution (C++)** and **quantitative research flexibility (Python)**.

## 📌 Project Overview
Limit-Order-Book-LOB-Engine implements a **Price-Time Priority (FIFO)** matching engine. It is designed to handle high-throughput order flow while maintaining deterministic $O(1)$ complexity for core operations like placement and cancellation.

### Key Components
* **`/cpp`**: The "Hot Path" execution engine. Focused on memory locality and zero-garbage-collection latency.
* **`/python`**: The "Research Layer." Optimized for strategy backtesting and rapid prototyping using lazy-cancellation logic.
* **`/benchmarks`**: Comparative analysis of throughput and latency across both implementations.

## 📁 Project Structure
```
limit-order-book/
│
├── cpp/
│   ├── main.cpp                # High-performance C++ matching engine
│   └── README.md               # Technical deep-dive into C++ optimizations
│
├── python/
│   ├── orderbook.py            # Research-oriented Python matching engine
│   └── README.md               # Explanation of Pythonic "Lazy Cancellation"
│
├── benchmarks/
│   ├── benchmark_results.txt   # Raw performance output data
│   └── analysis.md             # Detailed C++ vs. Python latency report
│
├── requirements.txt
├── README.md                   # MAIN project description
└── .gitignore'
```

## 📊 Performance at a Glance
Results based on 100,000 randomized order operations (50k Buys, 50k Sells, ~6.6k Cancels):

| Metric | C++ (O3 Optimized) | Python (Amortized) |
| :--- | :--- | :--- |
| **Execution Time** | `66` ms | `0.1497` s |
| **Complexity** | Strict $O(1)$ | Amortized $O(1)$ |
| **Memory Strategy** | Pointer Mapping | Lazy-Flagging |

## 🚀 Quick Start
1.  **Clone the repo:** `git clone https://github.com/Naman919/Limit-Order-Book-LOB-Engine.git`
2.  **Navigate into the project directory:** `cd Limit-Order-Book-LOB-Engine`
3.  **Install Python deps:** `pip install -r requirements.txt`
4.  **Run C++ Benchmark:** See `cpp/README.md`
5.  **Run Python Research:** See `python/README.md`

## License

MIT License

---
*Developed as part of a Quantitative Engineering Portfolio.*
