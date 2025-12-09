/*
 * Utility implementation for printing multi-dimensional arrays.
 */

#include "tensor_printer.h"

#include <cstdio>
#include <tensorflow/lite/micro/micro_log.h>

namespace {

constexpr int kMaxDims = 6;
constexpr int kValuesPerLine = 28;

const char *TypeName(TfLiteType type)
{
	switch (type) {
	case kTfLiteInt8: return "int8";
	case kTfLiteUInt8: return "uint8";
	case kTfLiteInt16: return "int16";
	case kTfLiteFloat32: return "float32";
	default: return "unknown";
	}
}

struct LineBuilder {
	char buffer[128];
	int used;

	LineBuilder() : buffer(), used(0) {}

	void Flush(void)
	{
		if (used == 0) return;
		buffer[used] = '\0';
		MicroPrintf("%s", buffer);
		used = 0;
	}

	template <typename... Args>
	void Append(const char *fmt, Args... args)
	{
		int available = static_cast < int > (sizeof(buffer)) - used;
		int written = std::snprintf(buffer + used, available, fmt, args...);
		if (written < 0) return;
		if (written >= available) {
			Flush();
			available = static_cast < int > (sizeof(buffer));
			written = std::snprintf(buffer, available, fmt, args...);
			if (written < 0) return;
			if (written >= available) {
				buffer[available - 1] = '\0';
				MicroPrintf("%s", buffer);
				used = 0;
				return;
			}
			used = written;
			return;
		}
		used += written;
	}
};

template <typename T>
void PrintFlatTensor(const char *label, const T *data, int count)
{
	const char *name = (label != nullptr) ? label : "tensor";
	MicroPrintf("%s shape=[%d] =", name, count);

	LineBuilder line;
	line.Append("[");
	for (int i = 0; i < count; ++i) {
		line.Append("%0.6f", static_cast < double > (data[i]));
		if (i != count - 1) {
			line.Append(", ");
		}

		if (kValuesPerLine > 0 && ((i + 1) % kValuesPerLine) == 0 &&
		    i != count - 1) {
			line.Flush();
		}
	}
	line.Append("]");
	line.Flush();
}

template <typename T>
void PrintMatrixTensor(const char *label, const T *data,
		       const int *shape)
{
	const int rows = shape[0];
	const int cols = shape[1];
	const char *name = (label != nullptr) ? label : "tensor";

	LineBuilder line;
	line.Append("%s shape=[%d, %d] = [", name, rows, cols);
	for (int r = 0; r < rows; ++r) {
		line.Append("[");
		for (int c = 0; c < cols; ++c) {
			const int idx = r * cols + c;
			line.Append("%0.6f", static_cast < double > (data[idx]));
			if (c != cols - 1) line.Append(", ");
		}
		line.Append("]");
		if (r != rows - 1) line.Append(", ");
	}
	line.Append("]\n");
	line.Flush();
}

template <typename T>
void PrintTensorImpl(const char *label, const T *data, const int *shape, int dims)
{
	if (data == nullptr || shape == nullptr || dims <= 0) {
		MicroPrintf("PrintMultiDimensional: invalid arguments");
		return;
	}

	if (dims > kMaxDims) {
		MicroPrintf("PrintMultiDimensional: dims=%d exceeds %d",
			    dims, kMaxDims);
		return;
	}

	if (dims == 1) {
		PrintFlatTensor(label, data, shape[0]);
		return;
	}

	if (dims == 2) {
		PrintMatrixTensor(label, data, shape);
		return;
	}

	int total = 1;
	for (int i = 0; i < dims; ++i) total *= shape[i];

	int indices[kMaxDims];
	for (int flat = 0; flat < total; ++flat) {
		int remainder = flat;
		for (int dim = dims - 1; dim >= 0; --dim) {
			indices[dim] = remainder % shape[dim];
			remainder /= shape[dim];
		}

		char index_buffer[96];
		int written = std::snprintf(index_buffer,
					    sizeof(index_buffer),
					    "%s[",
					    (label != nullptr) ? label
							       : "value");
		if (written < 0) written = 0;
		for (int dim = 0; dim < dims && written < (int)sizeof(index_buffer) - 2;
		     ++dim) {
			written += std::snprintf(index_buffer + written,
						 sizeof(index_buffer) - written,
						 (dim == dims - 1) ? "%d" : "%d, ",
						 indices[dim]);
			if (written < 0) {
				written = 0;
				break;
			}
		}
		std::snprintf(index_buffer + written,
			      sizeof(index_buffer) - written,
			      "]");

		MicroPrintf("%s = %f",
			    index_buffer,
			    static_cast < double > (data[flat]));
	}
}

}  /* namespace */

void PrintMultiDimensional(const char *label,
			   const float *data,
			   const int *shape,
			   int dims)
{
	PrintTensorImpl(label, data, shape, dims);
}

void PrintMultiDimensional(const char *label,
			   const int8_t *data,
			   const int *shape,
			   int dims)
{
	PrintTensorImpl(label, data, shape, dims);
}

void PrintTensorShape(const char *label, const TfLiteTensor *tensor)
{
	const char *name = (label != nullptr) ? label : "tensor";
	if (tensor == nullptr) {
		MicroPrintf("%s tensor is null", name);
		return;
	}

	const TfLiteIntArray *dims = tensor->dims;
	if (dims == nullptr) {
		MicroPrintf("%s tensor has no dims", name);
		return;
	}

	LineBuilder line;
	line.Append("%s shape=[", name);
	for (int i = 0; i < dims->size; ++i) {
		line.Append("%d", dims->data[i]);
		if (i != dims->size - 1) line.Append(", ");
	}
	line.Append("]");
	line.Flush();
}

void PrintTensor(const TfLiteTensor *tensor)
{
	if (tensor == nullptr || tensor->dims == nullptr) {
		MicroPrintf("tensor unavailable");
		return;
	}

	const char *name = (tensor->name != nullptr) ? tensor->name : "tensor";
	const TfLiteIntArray *dims = tensor->dims;
	if (dims->size <= 0 || dims->size > kMaxDims) {
		MicroPrintf("%s dims=%d not supported", name, dims->size);
		return;
	}

	int shape[kMaxDims];
	for (int i = 0; i < dims->size; ++i) shape[i] = dims->data[i];

	MicroPrintf("%s type=%s", name, TypeName(tensor->type));

	switch (tensor->type) {
	case kTfLiteInt8:
		PrintMultiDimensional(name, tensor->data.int8, shape, dims->size);
		break;
	case kTfLiteFloat32:
		PrintMultiDimensional(name, tensor->data.f, shape, dims->size);
		break;
	default:
		MicroPrintf("%s tensor type %d not printable", name, tensor->type);
		break;
	}
}
