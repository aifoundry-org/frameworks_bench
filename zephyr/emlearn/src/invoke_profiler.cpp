/*
 * Simple rolling profiler for interpreter->Invoke().
 */

#include "invoke_profiler.h"

#include <zephyr/sys/printk.h>
#include <algorithm>
#include <array>
#include <cmath>

namespace {

constexpr size_t kTimingWindow = 64;
std::array<uint32_t, kTimingWindow> invoke_history = {};
size_t invoke_history_size = 0;

uint64_t invoke_count = 0;
double invoke_mean_us = 0.0;
double invoke_m2 = 0.0;

}  /* namespace */

void LogInvokeStats(uint64_t duration_ns)
{
	const uint32_t delta_us = static_cast < uint32_t > (duration_ns / 1000);

	invoke_count += 1;
	const double delta = static_cast < double > (delta_us) - invoke_mean_us;
	invoke_mean_us += delta / static_cast < double > (invoke_count);
	invoke_m2 += delta * (static_cast < double > (delta_us) - invoke_mean_us);

	const size_t slot = static_cast < size_t > ((invoke_count - 1) % kTimingWindow);
	invoke_history[slot] = delta_us;
	if (invoke_history_size < kTimingWindow) {
		++invoke_history_size;
	}

	std::array<uint32_t, kTimingWindow> tmp = invoke_history;
	std::sort(tmp.begin(), tmp.begin() + invoke_history_size);
	uint32_t median_us = tmp[(invoke_history_size - 1) / 2];
	if ((invoke_history_size % 2) == 0 && invoke_history_size > 1) {
		median_us = static_cast < uint32_t > (
			(tmp[invoke_history_size / 2 - 1] + tmp[invoke_history_size / 2]) / 2);
	}

	const double variance = (invoke_count > 1)
		? (invoke_m2 / static_cast < double > (invoke_count - 1))
		: 0.0;
	const double stddev_us = std::sqrt(variance);

	printk("Invoke ms: last=%.2f mean=%.2f median=%.2f std=%.2f count=%u",
				delta_us / 1000.0,
				invoke_mean_us / 1000.0,
				median_us / 1000.0,
				stddev_us / 1000.0,
				static_cast < unsigned int > (invoke_count));
}
