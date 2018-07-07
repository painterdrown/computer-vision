import sys, os
import cv2
import numpy as np

if __name__ == '__main__':
  filename = sys.argv[1]
  workdir = os.getcwd()
  a4_digits = os.path.join(workdir, 'data', 'a4_digits', filename)
  a4_digits_dilation = os.path.join(workdir, 'data', 'a4_digits', filename+'_dilation')
  a4_digits_resized = os.path.join(workdir, 'data', 'a4_digits', filename + '_resized')
  os.mkdir(a4_digits_dilation)
  os.mkdir(a4_digits_resized)
  images = os.listdir(a4_digits)
  kernel = np.ones((3,3),np.uint8)
  for image_name in images:
    if not image_name.endswith('.jpg'):
      continue
    print "resize:", image_name
    image_path = os.path.join(a4_digits, image_name)
    image = cv2.imread(image_path, 0)
    # dilation
    dilation = cv2.dilate(image, kernel, iterations=1)
    dilation_path = os.path.join(a4_digits_dilation, image_name)
    cv2.imwrite(dilation_path, dilation)
    # resize
    resized_image = cv2.resize(dilation, (28, 28))
    resize_path = os.path.join(a4_digits_resized, image_name)
    cv2.imwrite(resize_path, resized_image)
