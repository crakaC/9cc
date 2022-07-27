#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数
    TK_EOF,      // 入力終わり
} TokenKind;

typedef struct Token Token;

// トークン
struct Token {
    TokenKind kind; // 種類
    Token* next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合はその値
    char* str;      // トークン文字列
    int len;        // トークンの長さ
};

bool consume(char* op);
void expect(char* op);
int expect_number();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize(char* p);

// 現在着目しているトークン
extern Token* token;
extern char* user_input;

// 
// parse.c
// 

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <, ">" のときはノードを入れ替える
    ND_LE,  // <=. ">=" のときはノードを入れ替える
    ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node* lhs; // 左辺
    Node* rhs; // 右辺
    int val; // kindがND_NUMのときに使う
};

Node* expr();


void gen(Node* node);
