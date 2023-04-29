#include "9cc.h"

void emit(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("  ");
    vprintf(fmt, ap);
    printf("\n");
}

void emit_noindent(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
}

void gen_lval(Node* node) {
    if (node->kind != ND_LVAR) {
        error("");
    }
    emit("mov rax, rbp");
    emit("sub rax, %d", node->offset);
    emit("push rax");
}

void gen(Node* node) {
    switch (node->kind) {
    case ND_NUM:
        emit("push %d", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        emit("pop rax");
        emit("mov rax, [rax]");
        emit("push rax");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        emit("pop rdi");
        emit("pop rax");
        emit("mov [rax], rdi");
        emit("push rdi");
        return;
    case ND_RETURN:
        gen(node->lhs);
        emit("pop rax");
        emit("mov rsp, rbp");
        emit("pop rbp");
        emit("ret");
        return;
    case ND_IF:
        emit_noindent("### BEGIN IF(%d)", node->label_number);
        gen(node->condition);
        emit("pop rax");
        emit("cmp rax, 0");
        if (node->els == NULL) {
            emit("je .Lendif%d", node->label_number);
            gen(node->then);
            emit_noindent(".Lendif%d:", node->label_number);
        } else {
            emit("je .Lelse%d", node->label_number);
            gen(node->then);
            emit("jmp .Lend_if%d", node->label_number);
            emit_noindent(".Lelse%d:", node->label_number);
            gen(node->els);
            emit_noindent(".Lend_if%d:", node->label_number);
        }
        return;
    case ND_WHILE:
        emit_noindent(".Lbegin%d:\n", node->label_number);
        gen(node->condition);
        emit("pop rax");
        emit("cmp rax, 0");
        emit("je .Lend%d", node->label_number);
        gen(node->body);
        emit("jmp .Lbegin%d", node->label_number);
        emit_noindent(".Lend%d:", node->label_number);
        return;
    case ND_FOR:
        emit_noindent("### BEGIN FOR(%d)", node->label_number);
        if (node->init) {
            gen(node->init);
        }
        emit_noindent(".Lbegin_for%d:", node->label_number);
        if (node->condition) {
            gen(node->condition);
            emit("pop rax");
            emit("cmp rax, 0");
            emit("je .Lend_for%d", node->label_number);
        }
        emit_noindent("### BEGIN FOR BLOCK(%d) ###", node->label_number);
        gen(node->body);
        emit_noindent("### END FOR BLOCK(%d) ###", node->label_number);
        if (node->increment) {
            gen(node->increment);
        }
        emit("jmp .Lbegin_for%d", node->label_number);
        emit_noindent(".Lend_for%d:", node->label_number);
        return;
    case ND_BLOCK:
        for (int i = 0; i < node->block->size; i++) {
            gen(node->block->data[i]);
        }
        return;
    case ND_CALL:
        // 引数をレジスタに格納（仮）
        emit("mov rdi, 1");
        emit("mov rsi, 2");
        emit("mov rdx, 4");

        // TODO x86-64ではcall前にスタックポインタを16ビット境界に合わせておかないといけないが
        // とりあえず動くので動作しなくなったらやる。
        emit("call %s", node->name);
        // 関数の返り値をスタックに詰める
        emit("push rax");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    emit("pop rdi");
    emit("pop rax");

    switch (node->kind) {
    case ND_EQ:  // ==
        emit("cmp rax, rdi");
        emit("sete al");
        emit("movzb rax, al");
        break;
    case ND_NE:  // !=
        emit("cmp rax, rdi");
        emit("setne al");
        emit("movzb rax, al");
        break;
    case ND_LT:  // >
        emit("cmp rax, rdi");
        emit("setl al");
        emit("movzb rax, al");
        break;
    case ND_LE:  // >=
        emit("cmp rax, rdi");
        emit("setle al");
        emit("movzb rax, al");
        break;
    case ND_ADD:
        emit("add rax, rdi");
        break;
    case ND_SUB:
        emit("sub rax, rdi");
        break;
    case ND_MUL:
        emit("imul rax, rdi");
        break;
    case ND_DIV:
        emit("cqo");
        emit("idiv rdi");
        break;
    case ND_MOD:
        emit("cqo");
        emit("idiv rdi");
        emit("mov rax, rdx");
        break;
    }

    emit("push rax");
}