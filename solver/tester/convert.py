import cv2
import numpy as np
import json


H, W = 32, 32
K = 4

image_pre = cv2.imread("images/image.jpg")
if image_pre is None:
  print("Error")
  exit()

image_pre = cv2.resize(image_pre, (W, H), interpolation=cv2.INTER_NEAREST)
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


# 結果の出力
cv2.imwrite("result.jpg", result)

with open("input.txt", "w") as f:  
  board = label.flatten().reshape((W, H))
  for i in range(H):
    s = ""
    for j in range(W):
      s += str(board[i, j])
    f.write(s + "\n")

with open("color.json", "w") as f:
  d = {}
  d["pieceColors"] = []
  for i in range(K):
    d["pieceColors"].append(f"({center[i][0]}, {center[i][1]}, {center[i][2]}, 255)")
  f.write(json.dumps(d, indent=2))
