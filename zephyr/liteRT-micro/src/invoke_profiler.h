/*
 * Lightweight invoke timing profiler.
 */

#ifndef HELLO_MNIST_INVOKE_PROFILER_H_
#define HELLO_MNIST_INVOKE_PROFILER_H_

#include <cstdint>

/* Record an Invoke duration (nanoseconds) and log rolling stats. */
void LogInvokeStats(uint64_t duration_ns);

#endif  /* HELLO_MNIST_INVOKE_PROFILER_H_ */
