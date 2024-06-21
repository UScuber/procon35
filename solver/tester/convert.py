import cv2
import numpy as np


K = 4

# return (出力画像, K個のカラーコード配列, 0~K-1のHW盤面)
def convert(input_image: np.ndarray, H: int, W: int):
  if input_image is None:
    print("Error")
    return

  image_pre = cv2.resize(input_image, (W, H), interpolation=cv2.INTER_NEAREST)
  image = image_pre.reshape(-1, 3)
  image = image.astype(np.float32)
  criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)

  ret, label, center = cv2.kmeans(image, K, None, criteria, 10, cv2.KMEANS_PP_CENTERS)

  center = center.astype(np.uint8)
  result = center[label.flatten()]
  result = result.reshape((image_pre.shape))

  r, g, b = cv2.split(result)
  a = np.ones(r.shape, dtype=np.uint8) * 255
  a[0, 0] = 0
  result = cv2.merge((r, g, b, a))

  board = label.flatten().reshape((W, H))

  return (result, center, board)
