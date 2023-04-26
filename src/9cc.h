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
    TK_RETURN,   // return
    TK_IF,       // if
    TK_ELSE,     // else
    TK_FOR,      // for
    TK_WHILE,    // while
    TK_IDENT,    // 識別子
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

void error(char* msg);
bool consume_token(TokenKind kind);
bool consume(char* op);
Token* consume_ident();
void expect(char* op);
int expect_number();
bool at_eof();
void tokenize(char* p);

extern char* user_input;

// 
// parse.c
// 

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <, ">" のときはノードを入れ替える
    ND_LE,      // <=. ">=" のときはノードを入れ替える
    ND_ASSIGN,  // =
    ND_LVAR,    // ローカル変数
    ND_NUM,     // 整数
    ND_RETURN,  // return
    ND_IF,      // if
    ND_FOR,     // for
    ND_WHILE,   // while
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node* lhs; // 左辺
    Node* rhs; // 右辺
    int val;   // kindがND_NUMのときに使う
    int offset;// kindがND_LVARのとき、RBPからのoffsetをローカル変数のアドレスとして使う
    int label_number; // if文などで使用するラベル番号

    // for文で使うやつ
    Node* initialization;
    Node* condition;
    Node* increment;
    Node* block;
};

typedef struct LVar LVar;
// ローカル変数の型
struct LVar {
    LVar* next; // 次の変数かNULL
    char* name; // 変数名
    int len; // 変数名の長さ
    int offset; // RBPからのオフセット
};

extern Node* code[100];
void program();

//
// codegen.c
//
void gen(Node* node);
