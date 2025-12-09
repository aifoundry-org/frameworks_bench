# frameworks_bench

Utilities for generating small machine-learning assets (models and test data) that can be embedded in C/C++ inference demos or benchmarks (e.g., TFLite Micro on Zephyr). The current focus is a simple MNIST MLP exported to TFLite along with sample inputs.

## Repository layout
- `models_gen/litert/train_mnist_model.py` — trains a Keras MLP on MNIST, exports a TensorFlow `SavedModel`, converts to TFLite, and emits a C array (`mnist_mlp_model_data.cc`) via `xxd -i`.
- `models_gen/litert/gen_mnist_data.py` — pulls MNIST test samples with TensorFlow Datasets and writes normalized, flattened inputs and labels to `gen_data/mnist/mnist_data.cc` for direct inclusion in C/C++.
- `saved_models/` — generated exports (TensorFlow + TFLite); ignored by git.
- `gen_data/` — generated C/C++-friendly datasets.
- `oot_executorch/` — placeholder for out-of-tree ExecuTorch/Zephyr integration.
- `models_gen/cifar`, `models_gen/mnist` — placeholders for future generators.
- `requirements.txt` — pinned Python dependencies; use a venv (`venv/` is git-ignored).

## Setup
```bash
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```
The scripts download MNIST via TensorFlow Datasets on first run, so you need network access once.

## Regenerate the MNIST model (TFLite + C array)
```bash
python models_gen/litert/train_mnist_model.py
```
Outputs:
- `saved_models/tensorflow/mnist_mlp` — TensorFlow `SavedModel` checkpoint.
- `saved_models/tflite/mnist_mlp/mnist_mlp.tflite` — flatbuffer model.
- `saved_models/tflite/mnist_mlp/mnist_mlp_model_data.cc` — byte array produced by `xxd -i` for embedding; rename or wrap as needed for your build.

## Regenerate sample MNIST inputs/labels
```bash
python models_gen/litert/gen_mnist_data.py
```
Outputs:
- `gen_data/mnist/mnist_data.cc` — `kMnistInputs[50][784]` of normalized floats and `kMnistLabels[50]` of `uint8` labels. Adjust `num_samples` in the script to change the batch size.

## Zephyr
For running zephyr example consider consulting with README.md in zephyr derectory

## Tips
- Generated assets live under `saved_models/` and `gen_data/`; clear them if you want a clean re-run.
- If you need deterministic runs, set `TF_DETERMINISTIC_OPS=1` and seed TensorFlow/NumPy before training.
