import sys, os

if __name__ == '__main__':
  filename = sys.argv[1]
  workdir = os.getcwd()

  a4 = os.path.join(workdir, 'data', 'a4')
  a4_bi = os.path.join(workdir, 'data', 'a4_bi')
  a4_seg = os.path.join(workdir, 'data', 'a4_seg')
  a4_digits = os.path.join(workdir, 'data', 'a4_digits')
  a4_digits_one = os.path.join(workdir, 'data', 'a4_digits', filename)

  if not os.path.exists(a4):
    os.mkdir(a4)
  if not os.path.exists(a4_bi):
    os.mkdir(a4_bi)
  if not os.path.exists(a4_seg):
    os.mkdir(a4_seg)
  if not os.path.exists(a4_digits):
    os.mkdir(a4_digits)
  if os.path.exists(a4_digits_one):
    os.rmdir(a4_digits_one)
  else:
    os.mkdir(a4_digits_one)
