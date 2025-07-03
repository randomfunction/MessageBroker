# Lock-Free Message Broker in C++

A high-performance, thread-safe message broker using a custom lock-free ring buffer. Built with modern C++ concurrency features for real-time applications requiring low-latency inter-thread communication.

## Features

- **Custom Lock-Free Ring Buffer** using atomics
- **Thread-safe Publish-Subscribe Broker** with per-topic queues
- **Multi-threaded simulation** of producers and consumers
- **Nanosecond-precision timestamping**
- **Minimal latency overhead** with zero dynamic allocations

## Components

- `RingBuffer.hpp`: Lock-free, fixed-size FIFO buffer
- `MessageBroker`: Manages subscriptions, message delivery
- `main.cpp`: Demo with latency logging and multithreaded test
- `LatencyTest`: Measures publish-to-consume latency

## Benchmark Results (Single Producer & 2 Consumers, 10,000 messages)

*Measured using high-resolution timestamps (nanoseconds) from `publish()` to `consume()`:*

| Metric | Value (ns)    | Value (ms) |
|--------|---------------|------------|
| Total  | 10,000        | –          |
| Min    | 785,000       | 0.79 ms    |
| Median | 15,912,000    | 15.91 ms   |
| P95    | 17,844,000    | 17.84 ms   |
| P99    | 19,068,000    | 19.07 ms   |
| Max    | 50,939,000    | 50.94 ms   |
| Average| 13,071,898    | 13.07 ms   |


## How to Run

```bash
g++ -std=c++20 -O2 -pthread main.cpp -o broker
./broker
