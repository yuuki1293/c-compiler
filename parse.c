#include "9cc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr(Token **token)
{
  return equality(token);
}

Node *equality(Token **token)
{
  Node *node = relational(token);

  for (;;)
  {
    if (consume("==", token))
      node = new_node(ND_EQ, node, relational(token));
    else if (consume("!=", token))
      node = new_node(ND_NE, node, relational(token));
    else
      return node;
  };
}

Node *relational(Token **token)
{
  Node *node = add(token);

  for (;;)
  {
    if (consume("<", token))
      node = new_node(ND_LT, node, add(token));
    else if (consume("<=", token))
      node = new_node(ND_LE, node, add(token));
    else if (consume(">", token))
      node = new_node(ND_LT, add(token), node);
    else if (consume(">=", token))
      node = new_node(ND_LE, add(token), node);
    else
      return node;
  }
}

Node *add(Token **token)
{
  Node *node = mul(token);

  for (;;)
  {
    if (consume("+",token))
      node = new_node(ND_ADD, node, mul(token));
    else if (consume("-", token))
      node = new_node(ND_SUB, node, mul(token));
    else
      return node;
  }
}

Node *mul(Token **token)
{
  Node *node = unary(token);

  for (;;)
  {
    if (consume("*", token))
      node = new_node(ND_MUL, node, unary(token));
    else if (consume("/", token))
      node = new_node(ND_DIV, node, unary(token));
    else
      return node;
  }
}

Node *unary(Token **token)
{
  if (consume("+", token))
    return primary(token);
  else if (consume("-", token))
    return new_node(ND_SUB, new_node_num(0), primary(token));
  return primary(token);
}

Node *primary(Token **token)
{
  if (consume("(", token))
  {
    Node *node = expr(token);
    expect(")", token);
    return node;
  }

  return new_node_num(expect_number(token));
}