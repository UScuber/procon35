
height = 32
width = 4

json_board = {}

json_board["height"] = height
json_board["width"] = width

all_n = [
    "1"*width,
    "2"*width,
    "3"*width,
    "4"*width
]

goal = []
for i in range(height):
    goal.append(all_n[i%len(all_n)])

import itertools
import random
import numpy as np
start = []
goal_1d = list(itertools.chain.from_iterable(goal))
random.shuffle(goal_1d)
for i in range(height):
    tmp = ""
    for j in range(width):
        tmp += goal_1d[i*width+j]
    start.append(tmp)

json_board["start"] = start
json_board["goal"] = goal

import json
with open ("input.json", "r") as f:
    input_json = json.load(f)
input_json["problem"]["board"] = json_board
with open ("input.json", "w") as f:
    json.dump(input_json, f, indent=2)
