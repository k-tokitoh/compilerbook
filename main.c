#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];

  // トークナイズする
  token = tokenize(argv[1]);
  program();

  int offset = 0;
  for (LVar *var = locals; var; var = var->next) {
    offset += 8;
    var->offset = offset;
  }

  codegen();

  return 0;
}