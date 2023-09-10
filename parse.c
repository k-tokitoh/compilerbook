#include "9cc.h"
#include <stdio.h>
#include <string.h>

LVar *locals = NULL;

// Find a local variable by name.
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (strlen(var->name) == tok->len && !memcmp(tok->str, var->name, tok->len))
      return var;
  }
  return NULL;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
}

Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *new_node_lvar(LVar *lvar) {
  Node *node = new_node(ND_LVAR);
  node->lvar = lvar;
  return node;
}

LVar *push_var(char *name) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  locals = lvar;
  return lvar;
}

Node *code[100];

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// program = stmt*
void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

// stmt = expr;
Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

// expr = assign
Node *expr() {
  Node *node = assign();
  return node;
}

// equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node_binary(ND_ASSIGN, node, assign());
  return node;
}

// equality = relational ("==" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node_binary(ND_EQ, node, relational());
    if (consume("!="))
      node = new_node_binary(ND_NE, node, relational());
    if (consume("<="))
      node = new_node_binary(ND_LE, node, relational());
    if (consume(">="))
      node = new_node_binary(ND_LE, relational(), node);
    else
      return node;
  }
}

Node *relational() {
  // relational は add < add という構造をしている
  // 最初の add を取り出す
  Node *node = add();

  // `<add`をみつけて前のmulと統合したひとつうえのnodeを返す、というのを繰り返す
  for (;;) {
    if (consume("<"))
      node = new_node_binary(ND_LT, node, add());
    if (consume(">"))
      node = new_node_binary(ND_LT, add(), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
  // add は mul + mul + mul + ... という構造をしている
  // 最初の mul を取り出す
  Node *node = mul();

  // `+mul`をみつけて前のmulと統合したひとつうえのnodeを返す、というのを繰り返す
  for (;;) {
    if (consume("+"))
      node = new_node_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  // mul は unary + unary + unary + ... という構造をしている
  // 最初の primary を取り出す
  Node *node = unary();

  // `+unary`をみつけて前のnumと統合したひとつうえのnodeを返す、というのを繰り返す
  for (;;) {
    if (consume("*"))
      node = new_node_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? primary
Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node_binary(ND_SUB, new_node_num(0), primary());
  return primary();
}

// primary = num | ident | "(" expr ")"
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    LVar *lvar = find_lvar(tok);
    if (!lvar)
      lvar = push_var(strndup(tok->str, tok->len));
    return new_node_lvar(lvar);
  }

  // 数値のはず
  Node *node = new_node_num(expect_number());
  return node;
}
