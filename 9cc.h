#include <stdbool.h>
#include <stdlib.h>

////////////////////////////////////////////////////// tokenize

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列の開始位置
  int len; // トークンの長さ（記号, 識別子の場合のみ意味をもつ）
};

void error(char *fmt, ...);
char *strndup(char *p, int len);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *tokenize(char *p);

extern char *user_input;
extern Token *token;

////////////////////////////////////////////////////// parse

// Local variable
typedef struct LVar LVar;
struct LVar {
  LVar *
      next; // 連結リストで辿って全部尽くして調べることで、変数名の重複などを確認できる
  char *name; // Variable name
  int offset; // Offset from RBP(register base pointer)
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_NUM,    // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
  LVar *lvar;    // kindがND_LVARの場合のみ使う
};

void program();

extern Node *code[];
extern LVar *locals;

////////////////////////////////////////////////////// codegen

void codegen();
