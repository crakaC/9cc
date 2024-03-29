#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vec {
    void** data;
    int size;
    int capacity;
} Vec;

Vec* new_vec(void);
void vec_push(Vec* v, void* element);
void* vec_last(Vec* v);
//
// tokenize.c
//

// トークンの種類
typedef enum {
    TK_RESERVED,    // 記号
    TK_VOID,        // void
    TK_INT,         // int
    TK_RETURN,      // return
    TK_IF,          // if
    TK_ELSE,        // else
    TK_FOR,         // for
    TK_WHILE,       // while
    TK_IDENT,       // 識別子
    TK_NUM,         // 整数
    TK_EOF,         // 入力終わり
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

void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
bool consume_token(TokenKind kind);
bool consume(char* op);
Token* consume_ident();
void expect(char* op);
int expect_number();
bool at_eof();
void tokenize(char* p);

extern char* user_input;

//
// type.c
//

typedef enum {
    VOID,
    INT,
    PTR
} TypeKind;

typedef struct Type Type;
struct Type {
    TypeKind ty;
    Type* ptr_to;

    int size;
    int align;
};

Type* ptr_to(Type* base);
Type* int_type();
Type* void_type();
bool same_type(Type* x, Type* y);

// 
// parse.c
// 

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_MOD,     // %
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
    ND_BLOCK,   // {}ブロック
    ND_CALL,    // 関数呼び出し
    ND_FUNC,    // 関数定義
    ND_ADDR,    // &
    ND_DEREF,   // *ptr
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node* lhs; // 左辺
    Node* rhs; // 右辺
    int val;   // kindがND_NUMのときに使う
    int offset;// kindがND_LVARのとき、RBPからのoffsetをローカル変数のアドレスとして使う
    int label_number; // if文などで使用するラベル番号

    char* name;
    Type* type; // 型

    // if ("condition") "then" else "els"
    Node* condition;
    Node* then;
    Node* els;

    // for("init"; "condition"; "increment") "body"
    // while("condition") "body"
    Node* init;
    Node* increment;
    Node* body;

    // function
    Type* return_type;
    Vec* args;

    Vec* block;
};

typedef struct Var Var;
// 変数
struct Var {
    Type* type;
    char* name; // 変数名
    int len; // 変数名の長さ
    int offset; // RBPからのオフセット
};

extern Token* token;
Vec* program();

//
// codegen.c
//
void gen(Node* node);