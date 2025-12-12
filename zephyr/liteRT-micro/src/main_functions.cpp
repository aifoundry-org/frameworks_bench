#include "main_functions.h"

#include <cmath>
#include <cstdint>
#include <zephyr/sys_clock.h>
// #include <zephyr/timing/timing.h>
#include <zephyr/kernel.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include "constants.h"
#include "model.hpp"
#include "mnist_data.cc"
#include "output_handler.hpp"
#include "invoke_profiler.h"
#include "tensor_printer.h"
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>

/* Globals, used for compatibility with Arduino-style sketches. */
namespace {
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *input = nullptr;
	TfLiteTensor *output = nullptr;
	int inference_count = 0;
	int output_size = 0;
	int input_size = 0;

	// constexpr int kTensorArenaSize = 2000;
	constexpr int kTensorArenaSize = 8 * 1024;
	uint8_t tensor_arena[kTensorArenaSize];
	timing_t start_time, end_time;

	uint64_t total_cycles;
    uint64_t total_ns;
}  /* namespace */

/* The name of this function is important for Arduino compatibility. */
void setup(void)
{
	/* Map the model into a usable data structure. This doesn't involve any
	 * copying or parsing, it's a very lightweight operation.
	 */
	model = tflite::GetModel(g_model);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		MicroPrintf("Model provided is schema version %d not equal "
					"to supported version %d.",
					model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	/* This pulls in the operation implementations we need.
	 * NOLINTNEXTLINE(runtime-global-variables)
	 */
	static tflite::MicroMutableOpResolver <2> resolver;
	resolver.AddFullyConnected();
	resolver.AddReshape();

	/* Build an interpreter to run the model with. */
	static tflite::MicroInterpreter static_interpreter(
		model, resolver, tensor_arena, kTensorArenaSize);
	interpreter = &static_interpreter;

	/* Allocate memory from the tensor_arena for the model's tensors. */
	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) {
		MicroPrintf("AllocateTensors() failed");
		return;
	}

	/* Obtain pointers to the model's input and output tensors. */
	input = interpreter->input(0);
	output = interpreter->output(0);
	PrintTensorShape("input", input);
	PrintTensorShape("output", output);
	input_size = model->subgraphs()->Get(0)->inputs()->size();

	/* Keep track of how many inferences we have performed. */
	inference_count = 0;

}

/* The name of this function is important for Arduino compatibility. */
void loop(void)
{
	/* Calculate an x value to feed into the model. We compare the current
	 * inference_count to the number of inferences per cycle to determine
	 * our position within the range of possible x values the model was
	 * trained on, and use this to calculate a value.
	 */

	const int sample_index = 2;
	const int sample_shape[1] = {kImageSize};
	const float *sample_data = kMnistInputs[inference_count];
	// PrintMultiDimensional("x", sample_data, sample_shape, 1);

	for (int i = 0; i < kImageSize; ++i) {
		const float pixel = sample_data[i];
		input->data.f[i] = pixel;
	}

	/* Run inference, and report any error */
	uint32_t start = k_cycle_get_32();
	TfLiteStatus invoke_status = interpreter->Invoke();
	uint32_t end   = k_cycle_get_32();

	uint32_t cycles = end - start;
    uint64_t ns     = k_cyc_to_ns_floor64(cycles);

	const int label = static_cast<int>(kMnistLabels[inference_count]);

	MicroPrintf("RAW: cycles=%u ns=%llu ms=%llu",
            (unsigned int)cycles,
            (unsigned long long)ns,
            (unsigned long long)(ns / 1000000ULL));

	LogInvokeStats(ns);
	
	MicroPrintf("Label=%d", label);
	PrintTensor(output);

	/* Increment the inference_counter, and reset it if we have reached
	 * the total number per cycle
	 */
	inference_count += 1;
	if (inference_count >= kInferencesPerCycle) inference_count = 0;
}
