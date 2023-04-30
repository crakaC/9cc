#include "9cc.h"

Node* code[100];
// ローカル変数
LVar* toplevel_locals[100];
LVar* locals;

Node* toplevel(); // トップレベル定義
Node* func_decl();
Node* func_call();
Node* stmt(); // 行。;で区切られているやつ
Node* expr(); // 最終的に値を吐き出すやつ。
Node* assign(); // x = yの形になってるやつ？
Node* equality(); // ==, !=
Node* relational(); // 比較
Node* add(); // 加減算
Node* mul(); // 乗除
Node* unary(); // 単項演算子
Node* primary(); // より優先するやつ。()の中。
Node* new_lvar(Token* tok);

// 制御構文で使用するラベル通し番号
int label_sequence_number = 0;

Node* new_node_kind(NodeKind kind) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

LVar* find_lvar(Token* tok, LVar* local_vars) {
    for (LVar* var = local_vars; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

void program() {
    int i = 0;
    while (!at_eof()) {
        locals = toplevel_locals[i];
        code[i++] = toplevel();
    }
    code[i] = NULL;
}

Node* toplevel() {
    return func_decl();
}

Node* func_decl() {
    Node* node = new_node_kind(ND_FUNC);

    if (!consume_token(TK_INT)) {
        error("function declaration must be started with \"int\".");
    }

    Token* tok = consume_ident();
    if (tok == NULL) {
        error("invalid function declaration\n");
    }
    node->name = strndup(tok->str, tok->len);
    node->args = new_vec();
    expect("(");
    while (!consume(")")) {
        consume_token(TK_INT);
        Token* t = consume_ident();
        if (!t) {
            error("invalid function arguments.\n");
        }
        vec_push(node->args, new_lvar(t));
        consume(",");
    }
    node->body = stmt();
    return node;
}

Node* func_call(Token* tok) {
    Node* node = new_node_kind(ND_CALL);
    node->name = strndup(tok->str, tok->len);
    node->args = new_vec();
    while (!consume(")")) {
        // TODO 引数の定義は適当に読み飛ばしている状態
        vec_push(node->args, expr());
        consume(",");
    }
    return node;
}

Node* stmt() {
    Node* node;
    if (consume("{")) {
        node = new_node_kind(ND_BLOCK);
        node->block = new_vec();
        while (!consume("}")) {
            vec_push(node->block, stmt());
        }
    } else if (consume_token(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
    } else if (consume_token(TK_WHILE)) {
        node = new_node_kind(ND_WHILE);
        node->label_number = label_sequence_number++;
        expect("(");
        node->condition = expr();
        expect(")");
        node->body = stmt();
    } else if (consume_token(TK_FOR)) {
        node = new_node_kind(ND_FOR);
        node->label_number = label_sequence_number++;
        expect("(");
        if (!consume(";")) {
            node->init = expr();
            expect(";");
        }
        if (!consume(";")) {
            node->condition = expr();
            expect(";");
        }
        if (!consume(")")) {
            node->increment = expr();
            expect(")");
        }
        node->body = stmt();
    } else if (consume_token(TK_IF)) {
        int label_number = label_sequence_number++;
        node = new_node_kind(ND_IF);
        expect("(");
        node->condition = expr();
        expect(")");
        node->then = stmt();
        node->label_number = label_number;
        if (consume_token(TK_ELSE)) {
            node->els = stmt();
        }
    } else if (consume_token(TK_INT)) {
        node = new_lvar(consume_ident());
        expect(";");
    } else {
        node = expr();
        expect(";");
    }
    return node;
}

Node* expr() {
    return assign();
}

Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality() {
    Node* node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node* relational() {
    Node* node = add();
    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_node(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

Node* add() {
    Node* node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = unary();
    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        } else if (consume("%")) {
            node = new_node(ND_MOD, node, unary());
        } else {
            return node;
        }
    }
}

Node* unary() {
    // 単項演算子
    if (consume("+")) {
        return unary();
    }
    if (consume("-")) {
        return new_node(ND_SUB, new_node_num(0), unary());
    }
    if (consume("&")) {
        return new_node(ND_ADDR, unary(), NULL);
    }
    if (consume("*")) {
        return new_node(ND_DEREF, unary(), NULL);
    }

    return primary();
}

Node* primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node* node = expr();
        expect(")");
        return node;
    }

    Token* tok = consume_ident();

    // identに()が続いているなら関数呼び出し
    if (consume("(")) {
        return func_call(tok);
    }

    if (tok) {
        Node* node = new_node_kind(ND_LVAR);
        LVar* lvar = find_lvar(tok, locals);
        if (!lvar) {
            error("lvar %s not found.\n", tok->str);
        }
        node->name = strndup(lvar->name, lvar->len);
        node->offset = lvar->offset;
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

Node* new_lvar(Token* tok) {
    LVar* exists = find_lvar(tok, locals);
    if (exists) {
        error_at(tok->str, "%s is already declared", strndup(exists->name, exists->len));
    }
    Node* node = new_node_kind(ND_LVAR);
    LVar* lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    if (locals) {
        lvar->offset = locals->offset + 8;
    } else {
        lvar->offset = 8;
    }
    node->offset = lvar->offset;
    locals = lvar;
    return node;
}