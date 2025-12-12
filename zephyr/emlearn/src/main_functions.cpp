#include "main_functions.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys_clock.h>

#include "invoke_profiler.h"
#include "mlp_mnist.h"
#include "mnist_data.cc"

namespace {
	int inference_count = 0;
}

void setup(void)
{
	inference_count = 0;
	printk("emlearn: samples=%d image=%d\n", kNumSamples, kImageSize);
}

void loop(void)
{
	const int idx = inference_count % kNumSamples;
	const float *sample = kMnistInputs[idx];

	uint32_t start = k_cycle_get_32();
	const int pred = mlp_mnist_predict(sample, kImageSize);
	uint32_t end = k_cycle_get_32();

	const uint32_t cycles = end - start;
	const uint64_t ns = k_cyc_to_ns_floor64(cycles);


	const int label = static_cast<int>(kMnistLabels[idx]);

	printk("\nRAW: cycles=%u ns=%llu pred=%d label=%d\n",
		   (unsigned int)cycles,
		   (unsigned long long)ns,
		   pred,
		   label);

    LogInvokeStats(ns);

	inference_count = (inference_count + 1) % kNumSamples;
}
