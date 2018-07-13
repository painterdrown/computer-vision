import os
import cv2
import pickle
from sklearn import svm
from svm_util import get_time, image_to_1d

def train(train_path):
  # read train data
  print get_time(), 'read train data'
  X = []
  y = []
  for i in range(0, 10):
    digits_path = os.path.join(train_path, str(i))
    images_names = os.listdir(digits_path)
    for image_name in images_names:
      if not image_name.endswith('.jpg'):
        continue
      image_path = os.path.join(digits_path, image_name)
      image = cv2.imread(image_path, 0)
      X.append(image_to_1d(image))
      y.append(i)

  # train
  print get_time(), 'begin training'
  clf = svm.SVC()
  clf.fit(X, y)
  print get_time(), 'trained model:', clf

  # save trained models
  clf_path = os.path.join(workdir, 'models', 'svm_mnist.pkl')
  clf_file = open(clf_path, 'wb')
  pickle.dump(clf, clf_file)
  print get_time(), 'trained model is saved at:', clf_path

  return clf

if __name__ == '__main__':
  workdir = os.getcwd()
  mnist_path = os.path.join(workdir, 'data', 'mnistasjpg', 'trainingSet')
  train(mnist_path)
