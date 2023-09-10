#include "9cc.h"
#include <stdio.h>

// 「ローカル変数に格納された値」ではなく「ローカル変数を格納するアドレス番号」をスタックトップにpushする関数であることに注意
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  // 変数の格納されたアドレス番号を算出
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->lvar->offset);
  // 変数の格納されたアドレス番号をスタックトップにpush
  printf("  push rax\n");
}

void gen(Node *node) {
  // nodeがリーフだった場合
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    // 変数の格納されたアドレス番号をpopして取り出す
    printf("  pop rax\n");
    // 変数の値をrax経由でスタックトップにpush
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  // nodeが代入演算子だった場合（代入演算子の左辺値に対しては読み出しではなく書き込みをしたいので）
  if (node->kind == ND_ASSIGN) {
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n"); // 右辺値を取り出す
    printf("  pop rax\n"); // 左辺値の指示するアドレス番号を取り出す
    printf("  mov [rax], rdi\n"); // 左辺値に右辺値を代入する
    printf("  push rdi\n"); // 代入した値をスタックトップにpushする
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NUM:
    error("子を持つnodeがND_NUMであることは不正です");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

void codegen() {
  // アセンブリの冒頭部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数の領域をざっくり確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 4096\n");

  // 抽象構文木を下りながらコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("  pop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}