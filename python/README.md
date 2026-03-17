# Python Research Layer

The Python implementation serves as the prototype for strategy testing. While it cannot match C++ for raw execution speed, it utilizes advanced Pythonic optimizations to remain efficient for backtesting scenarios.

## 💡 Implementation Details
* **Data Structures:** Uses `collections.deque` for FIFO time priority at each price level.
* **Memory Efficiency:** Uses `__slots__` in the `Order` class to prevent the creation of `__dict__` for every instance, significantly reducing memory footprint during large-scale simulations.
* **Lazy Cancellation:** To avoid $O(N)$ deletions in a deque, we use a **Soft-Delete** strategy. Orders are marked `active = False` in the `order_map` and are only popped when they hit the front of the matching queue.

## 🐍 Usage
Ensure you have installed the requirements from the root directory.

```bash
# Run the matching engine simulation
python orderbook.py
