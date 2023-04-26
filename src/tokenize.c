#include "9cc.h"

// 現在着目しているトークン
Token* token;

char* user_input;

void error(char* msg) {
    fprintf(stderr, "error: %s", msg);
    exit(1);
}

void error_at(char* loc, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めてtrueを返す
// それ以外の場合はfalseを返す。
bool consume(char* op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

bool consume_token(TokenKind kind) {
    if (token->kind != kind) {
        return false;
    }
    token = token->next;
    return true;
}

Token* consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token* cur = token;
    token = token->next;
    return cur;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーになる。
void expect(char* op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        error_at(token->str, "Unexpected token. '%c' is expected.", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを１つ読み進めてその数値を返す
// 数値でなければエラーになる。
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "It's not a number. '%s'", token->str);
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool starts_with(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

void dump() {
    for (Token* t = token; t; t = t->next) {
        char* s = calloc(t->len, sizeof(char));
        strncpy(s, t->str, t->len);
        fprintf(stderr, "str: %s, len: %d, kind: %d\n", s, t->len, t->kind);
    }
}

void tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }
        if (
            starts_with(p, "==") ||
            starts_with(p, "!=") ||
            starts_with(p, ">=") ||
            starts_with(p, "<=")
            ) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>=", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        if (isalpha(*p)) {
            cur = new_token(TK_IDENT, cur, p, 0);
            char* q = p;
            while (isalnum(*p)) {
                p++;
            }
            cur->len = p - q;
            continue;
        }
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        if (*p == ';') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        error_at(p, "Could not tokenize");
    }
    new_token(TK_EOF, cur, p, 0);
    token = head.next;
    // dump();
}