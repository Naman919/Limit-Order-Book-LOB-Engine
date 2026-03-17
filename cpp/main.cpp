#include <iostream>
#include <map>
#include <unordered_map>
#include <list>
#include <cstdint>
#include <chrono>

struct Order {
    uint64_t id;
    int price;
    int qty;
    bool is_buy;
};

struct Limit {
    int price;
    int total_volume = 0;
    std::list<Order> orders;
};

class OrderBook {
private:
    bool DEBUG;

    std::map<int, Limit, std::greater<>> bids; // max heap
    std::map<int, Limit> asks;                 // min heap

    std::unordered_map<uint64_t, std::pair<bool, std::list<Order>::iterator>> order_map;

public:
    OrderBook(bool debug=false) : DEBUG(debug) {}

    // --- MATCH BUY ---
    void match_buy(Order& incoming) {
        while (incoming.qty > 0 && !asks.empty()) {
            auto best_it = asks.begin();

            if (incoming.price < best_it->first) break;

            auto& limit = best_it->second;

            while (incoming.qty > 0 && !limit.orders.empty()) {
                auto& resting = limit.orders.front();

                int trade_qty = std::min(incoming.qty, resting.qty);

                if (DEBUG) {
                    std::cout << "TRADE EXECUTED -> Incoming BUY matched with resting SELL | Qty: "
                              << trade_qty << " @ Price: " << limit.price << "\n";
                }

                incoming.qty -= trade_qty;
                resting.qty -= trade_qty;
                limit.total_volume -= trade_qty;

                if (resting.qty == 0) {
                    order_map.erase(resting.id);
                    limit.orders.pop_front();
                }
            }

            if (limit.orders.empty()) {
                asks.erase(best_it);
            }
        }

        if (incoming.qty > 0) add_order(incoming);
    }

    // --- MATCH SELL ---
    void match_sell(Order& incoming) {
        while (incoming.qty > 0 && !bids.empty()) {
            auto best_it = bids.begin();

            if (incoming.price > best_it->first) break;

            auto& limit = best_it->second;

            while (incoming.qty > 0 && !limit.orders.empty()) {
                auto& resting = limit.orders.front();

                int trade_qty = std::min(incoming.qty, resting.qty);

                if (DEBUG) {
                    std::cout << "TRADE EXECUTED -> Incoming SELL matched with resting BUY | Qty: "
                              << trade_qty << " @ Price: " << limit.price << "\n";
                }

                incoming.qty -= trade_qty;
                resting.qty -= trade_qty;
                limit.total_volume -= trade_qty;

                if (resting.qty == 0) {
                    order_map.erase(resting.id);
                    limit.orders.pop_front();
                }
            }

            if (limit.orders.empty()) {
                bids.erase(best_it);
            }
        }

        if (incoming.qty > 0) add_order(incoming);
    }

    // --- ADD ORDER ---
    void add_order(const Order& order) {
        if (order.is_buy) {
            if (!bids.count(order.price))
                bids[order.price] = Limit{order.price};

            auto& limit = bids[order.price];
            limit.orders.push_back(order);
            limit.total_volume += order.qty;

            auto it = --limit.orders.end();
            order_map[order.id] = {true, it};
        } else {
            if (!asks.count(order.price))
                asks[order.price] = Limit{order.price};

            auto& limit = asks[order.price];
            limit.orders.push_back(order);
            limit.total_volume += order.qty;

            auto it = --limit.orders.end();
            order_map[order.id] = {false, it};
        }

        if (DEBUG) {
            std::cout << "ORDER ADDED -> ID: " << order.id
                      << " | " << (order.is_buy ? "BUY" : "SELL")
                      << " | Qty: " << order.qty
                      << " | Price: " << order.price << "\n";
        }
    }

    // --- LIMIT ORDER ---
    void limit_order(uint64_t id, int price, int qty, bool is_buy) {
        if (DEBUG) {
            std::cout << "\nIncoming Order -> ID: " << id
                      << " | " << (is_buy ? "BUY" : "SELL")
                      << " | Qty: " << qty
                      << " | Price: " << price << "\n";
        }

        Order incoming{id, price, qty, is_buy};

        if (is_buy) match_buy(incoming);
        else match_sell(incoming);

        if (DEBUG) {
            if (incoming.qty == 0)
                std::cout << "Order " << incoming.id << " fully executed\n";
            else
                std::cout << "Order " << incoming.id
                          << " partially filled, remaining qty: "
                          << incoming.qty << "\n";
        }
    }

    // --- CANCEL ---
    void cancel_order(uint64_t id) {
        if (!order_map.count(id)) {
            if (DEBUG)
                std::cout << "Cancel Failed: Order not found or already filled\n";
            return;
        }

        auto [is_buy, it] = order_map[id];

        if (is_buy) {
            int price = it->price;
            auto& limit = bids[price];

            limit.total_volume -= it->qty;
            limit.orders.erase(it);

            if (limit.orders.empty())
                bids.erase(price);
        } else {
            int price = it->price;
            auto& limit = asks[price];

            limit.total_volume -= it->qty;
            limit.orders.erase(it);

            if (limit.orders.empty())
                asks.erase(price);
        }

        order_map.erase(id);

        if (DEBUG)
            std::cout << "ORDER CANCELLED -> ID: " << id << "\n";
    }

    // --- TOP OF BOOK ---
    void top_of_book() {
        std::cout << "\n--- TOP OF BOOK ---\n";

        if (!bids.empty())
            std::cout << "Best BID: " << bids.begin()->first
                      << " | Volume: " << bids.begin()->second.total_volume << "\n";
        else
            std::cout << "Best BID: None\n";

        if (!asks.empty())
            std::cout << "Best ASK: " << asks.begin()->first
                      << " | Volume: " << asks.begin()->second.total_volume << "\n";
        else
            std::cout << "Best ASK: None\n";

        std::cout << "--------------------\n";
    }
};

// --- MAIN ---
int main() {
    OrderBook ob(true); // Use DEBUG OFF(OrderBook ob(true);) for benchmarking

    int order_id = 1;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 50000; i++) {
        ob.limit_order(order_id++, 100 + (i % 5), 10 + (i % 3), true);
    }

    for (int i = 0; i < 50000; i++) {
        ob.limit_order(order_id++, 100 + (i % 5), 8 + (i % 4), false);
    }

    for (int i = 1; i <= 20000; i += 3) {
        ob.cancel_order(i);
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "\nExecution Time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";

    std::cout << "\n=== FINAL BOOK STATE ===\n";
    ob.top_of_book();

    return 0;
}
