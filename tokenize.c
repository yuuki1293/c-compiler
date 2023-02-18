#include "9cc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー箇所を報告する
void error_at(char *user_input, char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op, Token **token)
{
    if ((*token)->kind != TK_RESERVED ||
        strlen(op) != (*token)->len ||
        memcmp((*token)->str, op, (*token)->len))
        return false;
    *token = (*token)->next;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op, Token **token)
{
    if ((*token)->kind != TK_RESERVED ||
        strlen(op) != (*token)->len ||
        memcmp((*token)->str, op, (*token)->len))
        error("'%c'ではありません", op);
    *token = (*token)->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(Token **token)
{
    if ((*token)->kind != TK_NUM)
        error("数ではありません");
    int val = (*token)->val;
    *token = (*token)->next;
    return val;
}

bool at_eof(Token *token)
{
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    // 空白文字をスキップ
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (!strncmp(p, "==", 2) ||
        !strncmp(p, "!=", 2) ||
        !strncmp(p, "<=", 2) ||
        !strncmp(p, ">=", 2))
    {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>", *p))
    {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p, 0);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    if ('a' <= *p && *p <= 'z'){
      cur = new_token(TK_IDENT, cur, p, 1);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}