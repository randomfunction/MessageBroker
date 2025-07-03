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

## How to Run

```bash
g++ -std=c++20 -O2 -pthread main.cpp -o broker
./broker
