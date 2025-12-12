import emlearn
import numpy as np

import sys
import os
from pathlib import Path

# dirty hack
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from sklearn.neural_network import MLPClassifier
from litert.train_mnist_model import prepare_datasets

def tfds_to_numpy(ds):
    X_chunks, y_chunks = [], []
    for xb, yb in ds:
        X_chunks.append(xb.numpy())
        y_chunks.append(yb.numpy())
    return np.concatenate(X_chunks, axis=0), np.concatenate(y_chunks, axis=0)

ds_train, ds_test = prepare_datasets(batch_size=128)

X_train, y_train = tfds_to_numpy(ds_train)
X_test, y_test = tfds_to_numpy(ds_test)


# Assume X_train and y_train are your training data
estimator = MLPClassifier(
    hidden_layer_sizes=(512, 256),
    activation="relu",
    solver="sgd",
    max_iter=30,
    random_state=0
)
estimator.fit(X_train, y_train)

print("train acc:", estimator.score(X_train, y_train))
print("test  acc:", estimator.score(X_test, y_test))

savepath = "models_gen/emlearn/saved_models"
os.makedirs(savepath, exist_ok=True)
cmodel = emlearn.convert(estimator, method='inline')
cmodel.save("mlp_mnist",file=os.path.join(savepath, 'mlp_mnist.h'))