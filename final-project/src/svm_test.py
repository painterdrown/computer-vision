import os, sys
import cv2
import pickle
from svm_util import get_time, image_to_1d

def test(clf, images_path):
  images_names = os.listdir(images_path)
  images_names.sort()
  last_row = ''
  numbers = []
  number = ''
  for image_name in images_names:
    if not image_name.endswith('.jpg'):
      continue
    image_path = os.path.join(images_path, image_name)
    image = cv2.imread(image_path, 0)
    image_info = image_name.split('.')[0]
    row, col = image_info.split('_')
    digit = clf.predict([image_to_1d(image)])[0]
    # print image_name, digit
    if last_row != '' and row != last_row:
      numbers.append(number)
      number = ''
    number += str(digit)
    last_row = row
  if number != '':
    numbers.append(number)

  return numbers

if __name__ == '__main__':
  cwd = os.getcwd()
  print get_time(), 'load the svm model via pickle'
  clf_path = os.path.join(cwd, 'models', 'svm_mnist.pkl')
  clf_file = open(clf_path, 'rb')
  clf = pickle.load(clf_file)

  image_number = sys.argv[1]
  images_path = os.path.join(cwd, 'data', 'a4_digits', image_number+'_resized')
  print get_time(), 'begin testing'
  numbers = test(clf, images_path)
  print get_time(), 'result:'
  count = 1
  for number in numbers:
    print 'line ' + str(count) + ':', number
    count += 1
