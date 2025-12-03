import subprocess
import os
import pathlib
import tensorflow as tf
import tensorflow_datasets as tfds


def prepare_datasets(batch_size: int = 128):
    (ds_train, ds_test), ds_info = tfds.load(
        "mnist",
        split=["train", "test"],
        as_supervised=True,
        with_info=True,
    )

    def normalize_flatten(image, label):
        image = tf.cast(image, tf.float32) / 255.0
        image = tf.reshape(image, (-1,))  # 28x28 -> 784
        return image, label

    ds_train = (
        ds_train
        .map(normalize_flatten, num_parallel_calls=tf.data.AUTOTUNE)
        .cache()
        .shuffle(ds_info.splits["train"].num_examples)
        .batch(batch_size)
        .prefetch(tf.data.AUTOTUNE)
    )

    ds_test = (
        ds_test
        .map(normalize_flatten, num_parallel_calls=tf.data.AUTOTUNE)
        .cache()
        .batch(batch_size)
        .prefetch(tf.data.AUTOTUNE)
    )

    return ds_train, ds_test


class MLP(tf.keras.Model):
    def __init__(self, input_dim: int = 28 * 28, num_classes: int = 10):
        super().__init__()
        self.flatten = tf.keras.layers.Flatten()
        self.dense1 = tf.keras.layers.Dense(512, activation="relu")
        self.dense2 = tf.keras.layers.Dense(256, activation="relu")
        self.logits = tf.keras.layers.Dense(num_classes)

        # build ahead of time so model.summary() works immediately
        self.build((None, input_dim))

    def call(self, inputs, training=False):
        x = self.flatten(inputs)
        x = self.dense1(x)
        x = self.dense2(x)
        return self.logits(x)


def main():
    batch_size = 128
    epochs = 10

    ds_train, ds_test = prepare_datasets(batch_size=batch_size)

    model = MLP()

    model.compile(
        optimizer=tf.keras.optimizers.Adam(),
        loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
        metrics=["accuracy"],
    )

    model.fit(
        ds_train,
        epochs=epochs,
        validation_data=ds_test,
    )

    test_loss, test_acc = model.evaluate(ds_test)
    print(f"Test loss: {test_loss:.4f}, Test accuracy: {test_acc:.4f}")

    # --- converting model ---
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    # --- saving ---
    save_path = pathlib.Path("saved_models/tensorflow/mnist_mlp")
    save_path_tfl = pathlib.Path("saved_models/tflite/mnist_mlp")
    os.makedirs(save_path, exist_ok=True)
    os.makedirs(save_path_tfl, exist_ok=True)
    
    model.save(save_path)  # SavedModel format
    print(f"Model saved to: {save_path}")

    full_path_tflite = pathlib.Path(os.path.join(save_path_tfl, 'mnist_mlp.tflite'))
    with open(full_path_tflite, 'wb') as f:
        f.write(tflite_model)
    
    subprocess.run(f"xxd -i {full_path_tflite.resolve()} > {save_path_tfl.resolve()}/mnist_mlp_model_data.cc", shell=True)


if __name__ == "__main__":
    main()
