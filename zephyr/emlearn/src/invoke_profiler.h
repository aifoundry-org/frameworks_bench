/*
 * Lightweight invoke timing profiler.
 */

#ifndef EMLEARN_MLP_MNIST_INVOKE_PROFILER_H_
#define EMLEARN_MLP_MNIST_INVOKE_PROFILER_H_

#include <cstdint>

/* Record an Invoke duration (nanoseconds) and log rolling stats. */
void LogInvokeStats(uint64_t duration_ns);

#endif  /* EMLEARN_MLP_MNIST_INVOKE_PROFILER_H_ */
