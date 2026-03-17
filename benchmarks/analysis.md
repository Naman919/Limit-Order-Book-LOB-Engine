# Benchmark Analysis

## Overview
This project benchmarks a Limit Order Book implemented in C++ and Python using 100,000+ order operations.

## Results
- C++: ~66 ms
- Python: ~149 ms

## Key Observations

### 1. Performance
C++ outperforms Python due to:
- Compiled execution
- Lower memory overhead
- Efficient iterator handling

### 2. Scaling Behavior
As workload increases:
- Both implementations scale near-linearly
- Performance gap reduces (~2x at higher loads)

### 3. Deterministic Matching
Both implementations produce identical:
- Trade executions
- Final book state

This validates correctness of:
- Price-time priority
- FIFO matching

### 4. Data Structure Impact
- C++ uses `std::map` + `std::list` for O(log N) + O(1)
- Python uses `dict` + `deque` with lazy deletion

## Conclusion
The project demonstrates how language choice affects performance, while algorithmic design ensures correctness and scalability.
