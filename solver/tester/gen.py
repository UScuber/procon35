import cv2
import json
import glob
import os
import sys
import convert
import numpy as np
from tqdm import tqdm


def gen_random_board(board: np.ndarray, H: int, W: int) -> np.ndarray:
  result = board.copy()
  np.random.shuffle(result.reshape((H * W)))
  return result.reshape((H, W))


def write_data(center: np.ndarray, board: np.ndarray, H: int, W: int, outputfile: str, colorfile: str):
  with open(outputfile, "w") as f:
    outdata = {
      "H": H, "W": W,
    }
    outdata["goal"] = ["".join(str(board[i, j]) for j in range(W)) for i in range(H)]
    random_board = gen_random_board(board, H, W)
    outdata["start"] = ["".join(str(random_board[i, j]) for j in range(W)) for i in range(H)]

    json.dump(outdata, f, indent=2)

  with open(colorfile, "w") as f:
    d = {}
    d["pieceColors"] = []
    for i in range(convert.K):
      d["pieceColors"].append(f"({center[i][2]}, {center[i][1]}, {center[i][0]}, 255)")
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

  invalid_num = 0

  for name in tqdm(image_names):
    file = name.split("/")[-1].split(".")[0]
    filename = f"{dir}/{H}-{W}--{file}-in.json"
    colorname = f"{dir}/{H}-{W}--{file}-col.json"

    image = cv2.imread(name)
    result, center, board, isvalid = convert.convert(image, H, W)

    if isvalid:
      write_data(center, board, H, W, filename, colorname)
    else:
      invalid_num += 1
  
  print(f"{invalid_num} data was excepted (less color)")
