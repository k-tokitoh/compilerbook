https://www.sigbus.info/compilerbook

```
% docker build -t compilerbook .
% docker run -it -v $(pwd):/home/user compilerbook
```

```
// コンパイラのCのコードからコンパイラの実行ファイルを生成する
$ make
// コンパイル対象のコードを与えてコンパイラを実行し、アセンブリを生成する
$ ./9cc 5+20-4 > tmp.s
// アセンブリから実行ファイルを生成する
$ cc -o tmp tmp.s
// 実行する
$ ./tmp
$ echo $?
21
```

```
$ make test
$ bash -x test.sh  // for debug
```
