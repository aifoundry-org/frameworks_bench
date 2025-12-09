/*
 * Helper utilities for printing multi-dimensional arrays.
 */

#ifndef HELLO_MNIST_TENSOR_PRINTER_H_
#define HELLO_MNIST_TENSOR_PRINTER_H_

#include <cstdint>
#include <tensorflow/lite/c/common.h>

/* Prints the contents of a float tensor with the provided shape. */
void PrintMultiDimensional(const char *label,
			   const float *data,
			   const int *shape,
			   int dims);

/* Prints the contents of an int8_t tensor with the provided shape. */
void PrintMultiDimensional(const char *label,
			   const int8_t *data,
			   const int *shape,
			   int dims);

/* Prints the shape of a TfLiteTensor. */
void PrintTensorShape(const char *label, const TfLiteTensor *tensor);

/* Prints tensor name, type, shape, and all values (supports int8 and float32). */
void PrintTensor(const TfLiteTensor *tensor);

#endif  /* HELLO_MNIST_TENSOR_PRINTER_H_ */
