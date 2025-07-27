
# Low-Latency C++ Message Broker

A high-performance, thread-safe message broker designed for low-latency applications. This project combines a fine-grained locking strategy with an underlying lock-free SPSC ring buffer to achieve high concurrency and throughput. It incorporates several techniques common in High-Frequency Trading (HFT) systems to minimize latency on the critical path.

## Features

  - **Fine-Grained Locking**: Uses a per-topic mutex, allowing different topics to be accessed concurrently without contention.
  - **Lock-Free SPSC Queue**: Employs a wait-free, single-producer, single-consumer ring buffer for the underlying message queue.
  - **Allocation-Free Critical Path**: Messages use a fixed-size payload (`char[]`) to eliminate dynamic memory allocation (`new`/`delete`) during publish or consume operations.
  - **CPU Affinity**: Includes logic to pin producer and consumer threads to specific CPU cores, reducing cache misses and OS context switching.
  - **Low-Latency Polling**: Uses `std::this_thread::yield()` for responsive, non-blocking message consumption.
  - **Nanosecond Benchmarking**: A dedicated latency test measures end-to-end performance with high precision.

## Components

  - `ringbuffer.hpp`: The header for the lock-free SPSC ring buffer.
  - `message_broker.hpp`: The interface for the `MessageBroker` class.
  - `message_broker.cpp`: The implementation of the `MessageBroker`'s methods.
  - `main_test.cpp`: A simple functional test demonstrating basic publish/subscribe functionality.
  - `latency_test.cpp`: A high-performance benchmark that measures publish-to-consume latency.

## Benchmark Results

The following results were captured on a standard consumer machine and are subject to OS jitter. For true low-latency, a real-time kernel or kernel-level core isolation (`isolcpus`) would be required to minimize the maximum latency spikes.

*Test Parameters: 1 Producer, 1 Consumer, 50,000 Messages*

| Metric | Value (ns) | Value (µs) |
| :--- | ---: | ---: |
| **Median** | **8,000** | **8.0** |
| Min | 1,000 | 1.0 |
| P95 | 128,000 | 128.0 |
| P99 | 307,000 | 307.0 |
| Max | 52,688,000 | 52,688.0 |

## How to Build and Run

You will need a C++ compiler that supports C++17 (e.g., g++ or Clang).

### 1\. Run the Functional Test

This test demonstrates the basic producer-consumer logic with console output.

```bash
# Compile
g++ main_test.cpp message_broker.cpp -o main_test -std=c++17 -pthread -O3

# Run
./main_test
```

### 2\. Run the Latency Benchmark

This test measures performance and should be run without other CPU-intensive applications in the background for best results.

```bash
# Compile
g++ latency_test.cpp message_broker.cpp -o latency_test -std=c++17 -pthread -O3

# Run
./latency_test
```
