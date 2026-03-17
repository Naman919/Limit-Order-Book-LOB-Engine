from collections import defaultdict, deque
import time

class Order:
    __slots__ = ['id', 'price', 'qty', 'is_buy', 'active']

    def __init__(self, oid, price, qty, is_buy):
        self.id = oid
        self.price = price
        self.qty = qty
        self.is_buy = is_buy
        self.active = True

class OrderBook:
    def __init__(self, debug=False):
        self.DEBUG = debug
        self.bids = defaultdict(deque)
        self.asks = defaultdict(deque)
        self.bid_prices = set()
        self.ask_prices = set()
        self.order_map = {}

    # --- BEST PRICES ---
    def best_bid(self):
        return max(self.bid_prices) if self.bid_prices else None

    def best_ask(self):
        return min(self.ask_prices) if self.ask_prices else None

    # --- MATCHING ENGINE ---
    def match(self, incoming):
        while incoming.qty > 0:
            best_price = self.best_ask() if incoming.is_buy else self.best_bid()
            if best_price is None:
                break
              
            # Price condition
            if incoming.is_buy and incoming.price < best_price:
                break
            if not incoming.is_buy and incoming.price > best_price:
                break
              
            book = self.asks if incoming.is_buy else self.bids
            queue = book[best_price]

            while queue and incoming.qty > 0:
                order = queue[0]

                if not order.active:
                    queue.popleft()
                    continue

                trade_qty = min(incoming.qty, order.qty)
                # Identify sides
                if incoming.is_buy:
                    buy_id = incoming.id
                    sell_id = order.id
                else:
                    buy_id = order.id
                    sell_id = incoming.id

                if self.DEBUG:
                    print(f"TRADE -> BuyID: {buy_id} | SellID: {sell_id} | Qty: {trade_qty} | Price: {best_price}")

                incoming.qty -= trade_qty
                order.qty -= trade_qty

                if order.qty == 0:
                    order.active = False
                    self.order_map.pop(order.id, None)
                    queue.popleft()

            if not queue:
                book.pop(best_price)
                if incoming.is_buy:
                    self.ask_prices.remove(best_price)
                else:
                    self.bid_prices.remove(best_price)

        if incoming.qty > 0:
            self.add_order(incoming)

        # Execution summary
        if self.DEBUG:
            if incoming.qty == 0:
                print(f"Order {incoming.id} fully executed")
            else:
                print(f"Order {incoming.id} partially filled, remaining qty: {incoming.qty}")

    # --- ADD ORDER ---
    def add_order(self, order):
        book = self.bids if order.is_buy else self.asks
        price_set = self.bid_prices if order.is_buy else self.ask_prices

        book[order.price].append(order)
        price_set.add(order.price)
        self.order_map[order.id] = order

        if self.DEBUG:
            print(f"ORDER ADDED -> ID: {order.id} | {'BUY' if order.is_buy else 'SELL'} | Qty: {order.qty} | Price: {order.price}")

    # --- LIMIT ORDER ---
    def limit_order(self, oid, price, qty, is_buy):
        if self.DEBUG:
            print(f"\nIncoming Order -> ID: {oid} | {'BUY' if is_buy else 'SELL'} | Qty: {qty} | Price: {price}")

        order = Order(oid, price, qty, is_buy)
        self.match(order)

    # --- CANCEL ---
    def cancel_order(self, oid):
        if oid in self.order_map:
            self.order_map[oid].active = False
            del self.order_map[oid]

            if self.DEBUG:
                print(f"ORDER CANCELLED -> ID: {oid}")
        else:
            if self.DEBUG:
                print(f"Cancel Failed -> ID: {oid} (Already executed or does not exist)")

    # --- TOP OF BOOK ---
    def top_of_book(self):
        print("\n--- TOP OF BOOK ---")

        if self.bid_prices:
            best_bid = self.best_bid()
            volume = sum(o.qty for o in self.bids[best_bid] if o.active)
            print(f"Best BID: {best_bid} | Volume: {volume}")
        else:
            print("Best BID: None")

        if self.ask_prices:
            best_ask = self.best_ask()
            volume = sum(o.qty for o in self.asks[best_ask] if o.active)
            print(f"Best ASK: {best_ask} | Volume: {volume}")
        else:
            print("Best ASK: None")

        print("--------------------")


# --- MAIN (BENCHMARK) ---
if __name__ == "__main__":
    ob = OrderBook(debug=True)  # DEBUG OFF(OrderBook(debug=False)) for benchmarking

    order_id = 1

    start = time.time()

    # BUY orders
    for i in range(50000):
        ob.limit_order(order_id, 100 + (i % 5), 10 + (i % 3), True)
        order_id += 1

    # SELL orders
    for i in range(50000):
        ob.limit_order(order_id, 100 + (i % 5), 8 + (i % 4), False)
        order_id += 1

    # Cancellations
    for i in range(1, 20000, 3):
        ob.cancel_order(i)

    end = time.time()

    print("\nExecution Time:", round(end - start, 4), "seconds")

    print("\n=== FINAL BOOK STATE ===")
    ob.top_of_book()
