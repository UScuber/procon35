# procon35

## tester


# 初期化

```
sudo apt update
sudo apt install imagemagick

pip3 install -r requirements.txt
```


### gen.py

- `python3 gen.py <H> <W>`で動作(生成するドット絵のサイズがH,Wとなる)

- images内の画像がすべて変換され、outputディレクトリにファイルが出力される


### convert.py

- 画像を特定のサイズの4色だけのピクセルに変換するプログラム
