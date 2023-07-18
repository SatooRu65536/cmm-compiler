# C-- Language
`C--` は C言語のサブセットサブセット言語で、C言語の構文を簡略化したものです.  
**授業程度であれば**役に立たないこともないです.


### インストール方法
```shell
brew tap satooru65536/cmm
brew install cmm
```

## 実行方法
### コンパイル
c言語のファイルを出力します.
```shell
cmm [options] <input> <output>
```
`<input>` は入力するファイル名(必須)  
`<output>` は出力するファイル名(省略可)

### オプション
- `-r` : 実行までします. ファイルは出力されません.


## 構文
### f文字列
文字列の前に `f` をつけると、変数を展開できます.  
展開する変数は `{}` で囲み、型を `:` の後に指定します.
```c
printf(f"num={num:d}\n");
scan(f"{n:d} {m:d}");
```

### print文
`print` 文は `printf` と同じです.  
これでPython後でも書き間違えません!!  
もっと書く量を減らしたい場合は `p` でもOKです.
```c
print("Hello, World!\n");
p("Hello, World!\n");
```

### scan文
`scan` 文も `print` と同様です.
```c
scan("%d %d", &n, &m);
s("%d %d", &n, &m);
```

### main関数
ここまで来たら `main` 関数も省略しましょう.
```c
Main(int argc, char const *argv[]) {
    // コード
}
```

main関数の引数も大体同じなので省略しましょう.
```c
Main(Arg) {
    // コード
}
```

### include
`include` も書くのが面倒!!  
授業程度であれば書かなくても(大体は)勝手に `#include <xxx.h>` を挿入してくれます.
対応しているライブラリと関数(変数)は以下の通りです.

#### stdio.h
- p関数
- print関数
- print関数

#### stdlib.h
- rand関数
- random関数
- srand関数

#### string.h
- strlen関数
- strcmp関数
- strcpy関数
- strcat関数

#### math.h
- M_PI
- sin関数
- cos関数
- tan関数
- asin関数
- acos関数
- atan関数
- atan2関数
- sinh関数
- cosh関数
- tanh関数
- exp関数
- log関数
- log10関数
- pow関数
- sqrt関数
- ceil関数
- floor関数
- fabs関数
- fmod関数

# LICENSE
[MIT](./LICENSE)