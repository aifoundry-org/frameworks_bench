#ifndef EMLEARN_MLP_MNIST_MAIN_FUNCTIONS_H_
#define EMLEARN_MLP_MNIST_MAIN_FUNCTIONS_H_

/* Expose a C friendly interface for main functions. */
#ifdef __cplusplus
extern "C" {
#endif

/* Initializes all data needed for the example. */
void setup(void);

/* Runs one iteration of data gathering and inference. */
void loop(void);

#ifdef __cplusplus
}
#endif

#endif /* EMLEARN_MLP_MNIST_MAIN_FUNCTIONS_H_ */
