import datetime

def get_time():
  return datetime.datetime.now().strftime('[%Y-%m-%d %H:%M:%S]')

def image_to_1d(image):
  result = []
  for row in range(64):
    for col in range(64):
      if image[row][col] < 128:
        result.append(0)
      else:
        result.append(1)
  return result