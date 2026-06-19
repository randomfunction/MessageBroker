# Low-Latency Message Broker

A highly concurrent, thread-safe message broker optimized for low-latency communication. The architecture utilizes fine-grained locking and a wait-free Single-Producer Single-Consumer (SPSC) queue to maximize throughput and minimize latency on the critical path.

## Design Decisions

1. **Lock-Free Queuing**: The core messaging mechanism is implemented via an atomic, wait-free ring buffer. This prevents priority inversion and thread contention during high-throughput message processing.
2. **Fine-Grained Locking**: Concurrency control uses a per-topic mutex. This isolation ensures operations on distinct topics do not block each other, facilitating horizontal scalability.
3. **Zero-Allocation Critical Path**: Dynamic memory allocation is strictly avoided during message publishing and consumption. Messages are structured with fixed-size payload buffers.
4. **Hardware Optimization**: Features CPU affinity capabilities to pin producer and consumer threads to specific cores, reducing CPU cache misses and operating system context switching overhead.
5. **Non-Blocking Polling**: Message consumers utilize cooperative yielding to maintain responsive consumption channels without blocking the execution thread.

## Implementation Details

* **`MessageBroker`**: Manages topics and subscriber mappings, implementing the primary API for subscribing, publishing, and consuming messages. Includes support for batch consumption.
* **`RingBuffer`**: A templated queue utilizing atomic variables with memory order acquisition and release semantics for thread-safe operations.
* **`LatencyTest`**: A dedicated benchmarking utility that measures end-to-end publish-to-consume latency with nanosecond precision.

## Performance Benchmarks

*Test Parameters: 1 Producer, 1 Consumer, 50,000 Messages on a single machine.*

| Metric | Value (ns) | Value (µs) |
| :--- | ---: | ---: |
| **Median** | **8,000** | **8.0** |
| Min | 1,000 | 1.0 |
| P95 | 128,000 | 128.0 |
| P99 | 307,000 | 307.0 |
| Max | 52,688,000 | 52,688.0 |

*Note: Results are subject to OS jitter. For strict real-time constraints, kernel-level core isolation (`isolcpus`) is recommended.*

## Build Instructions

The project uses CMake for build configuration. A C++ compiler supporting the C++17 standard is required.

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Executables

1. **`main_test`**: Validates basic producer-consumer correctness and routing.
   ```bash
   ./main_test
   ```

2. **`LatencyTest`**: Executes the high-performance benchmark suite.
   ```bash
   ./LatencyTest
   ```
