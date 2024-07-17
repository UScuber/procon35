import cv2
import json
import glob
import os
import sys
import convert
import numpy as np
from tqdm import tqdm



def write_data(center: np.ndarray, board: np.ndarray, H: int, W: int, outputfile: str, colorfile: str):
  assert colorfile.endswith(".json"), "colorfileの拡張子をjsonにしてください"

  with open(outputfile, "w") as f:
    for i in range(H):
      s = ""
      for j in range(W):
        s += str(board[i, j])
      f.write(s + "\n")

  with open(colorfile, "w") as f:
    d = {}
    d["pieceColors"] = []
    for i in range(convert.K):
      d["pieceColors"].append(f"({center[i][0]}, {center[i][1]}, {center[i][2]}, 255)")
    f.write(json.dumps(d, indent=2))


if __name__ == "__main__":
  if len(sys.argv) < 3:
    print("Argument Error: python3 gen.py H W")
    exit()
  if not sys.argv[1].isdigit() or not sys.argv[2].isdigit():
    print("H or W is not integer")

  H = int(sys.argv[1])
  W = int(sys.argv[2])
  dir = "output"
  os.makedirs(dir, exist_ok=True)

  image_names = glob.glob("images/*.jpg") + glob.glob("images/*.png")
  print("Start writing")

  for name in tqdm(image_names):
    file = name.split("/")[-1].split(".")[0]
    filename = f"{dir}/{H}-{W}--{file}.txt"
    colorname = f"{dir}/{H}-{W}--{file}-col.json"
    # 生成済みであればスルー
    if os.path.exists(filename):
      continue

    image = cv2.imread(name)
    result, center, board, isvalid = convert.convert(image, H, W)

    if isvalid:
      write_data(center, board, H, W, filename, colorname)
