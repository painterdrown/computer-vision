import os, sys
import cv2
from sklearn import svm
import pickle
import datetime

def get_time():
  return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

def train(mnist_train_path):
  # read train data
  print get_time(), 'begin reading train data'
  X = []
  y = []
  for i in range(0, 10):
    digits_path = os.path.join(mnist_train_path, str(i))
    images_names = os.listdir(digits_path)
    for image_name in images_names:
      if not image_name.endswith('.jpg'):
        continue
      image_path = os.path.join(digits_path, image_name)
      image = cv2.imread(image_path, 0)
      X.append(image)
      y.append(i)
  print get_time(), 'finish reading train data'

  # train
  print get_time(), 'begin training'
  clf = svm.SVC()
  clf.fit(X, y)
  print get_time(), 'finish training'

  # save trained models
  time = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
  clf_path = os.path.join(workdir, 'models', time+'.svm')
  clf_file = open(clf_path, 'wb')
  pickle.dump(clf, clf_file)
  print get_time(), 'trained model is saved at:', clf_path

  return clf

def test(clf, images_path):
  images_names = os.listdir(images_path)
  last_row = ''
  numbers = []
  number = []
  for image_name in images_names:
    if not image_name.endswith('.jpg'):
      continue
    image_path = os.path.join(images_path, image_name)
    image = cv2.imread(image_path, 0)
    image_info = image_name.split('.')[0]
    row, col = image_info.split('_')
    digit = clf.predict([image])[0]
    if row != last_row:
      numbers.append(number)
      number = []
    number.append(digit)
    last_row = row

  return numbers

if __name__ == '__main__':
  workdir = os.getcwd()
  mnist_path = os.path.join(workdir, 'data', 'mnistasjpg')
  mnist_train_path = os.path.join(mnist_path, 'trainingSet')
  clf = train(mnist_train_path)
  image_number = sys.argv[1]
  images_path = os.path.join(workdir, 'data', 'a4_digits', image_number+'resized')
  print get_time(), 'begin testing'
  numbers = test(clf, images_path)
  print get_time(), 'finish testing'
  print get_time(), 'the result is as:', numbers
