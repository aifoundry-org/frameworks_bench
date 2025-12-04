import numpy as np
import tensorflow_datasets as tfds
import os
from pathlib import Path

def main():
    num_samples = 50
    image_height = 28
    image_width = 28
    num_pixels = image_height * image_width

    # Load MNIST validation (test) split
    ds = tfds.load("mnist", split="test", as_supervised=True)

    images = []
    labels = []

    # Take first `num_samples` samples
    for img, label in tfds.as_numpy(ds.take(num_samples)):
        # Normalize to [0, 1] and flatten
        img = img.astype(np.float32) / 255.0
        images.append(img.reshape(-1))
        labels.append(int(label))

    images = np.stack(images, axis=0)  # (num_samples, 784)
    labels = np.array(labels, dtype=np.uint8)

    save_path = Path("gen_data/mnist")
    os.makedirs(save_path, exist_ok=True)

    lines = []
    lines.append("// Generated MNIST samples")
    lines.append("#include <cstdint>")
    lines.append("")
    lines.append(f"constexpr int kNumSamples = {num_samples};")
    lines.append(f"constexpr int kImageSize = {num_pixels};")
    lines.append("")
    lines.append("alignas(4) const float kMnistInputs[kNumSamples][kImageSize] = {")
    for i in range(num_samples):
        img_vals = images[i]
        row_vals = []
        for j, v in enumerate(img_vals):
            row_vals.append(f"{v:.6f}f")
        row_str = ", ".join(row_vals)
        lines.append("    {" + row_str + "},")
    lines.append("};")
    lines.append("")
    lines.append("const uint8_t kMnistLabels[kNumSamples] = {")
    label_str = ", ".join(str(int(l)) for l in labels)
    lines.append("    " + label_str)
    lines.append("};")
    lines.append("")

    cpp_code = "\n".join(lines)
    with open(os.path.join(save_path, "mnist_data.cc"), 'w') as f:
        f.write(cpp_code)

    # print(cpp_code)


if __name__ == "__main__":
    main()
