#include "9cc.h"

void gen_lval(Node* node) {
    if (node->kind != ND_LVAR) {
        error("");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node* node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_IF:
        printf("### BEGIN IF %d\n", node->label_number);
        gen(node->condition);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (node->rhs == NULL) {
            printf("  je .Lendif%d\n", node->label_number);
            gen(node->lhs);
            printf(".Lendif%d:\n", node->label_number);
        } else {
            printf("  je .Lelse%d\n", node->label_number);
            gen(node->lhs);
            printf("  jmp .Lendif%d\n", node->label_number);
            printf(".Lelse%d:\n", node->label_number);
            gen(node->rhs);
            printf(".Lendif%d:\n", node->label_number);
        }
        printf("### END IF %d\n", node->label_number);
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", node->label_number);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", node->label_number);
        gen(node->rhs);
        printf("  jmp .Lbegin%d\n", node->label_number);
        printf(".Lend%d:\n", node->label_number);
        return;
    case ND_FOR:
        printf("### BEGIN FOR(%d)\n", node->label_number);
        if (node->initialization) {
            gen(node->initialization);
        }
        printf(".Lbegin%d:\n", node->label_number);
        if (node->condition) {
            gen(node->condition);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", node->label_number);
        }
        printf("### FOR BLOCK(%d) ###\n", node->label_number);
        gen(node->block);
        printf("### END FOR BLOCK(%d) ###\n", node->label_number);
        if (node->increment) {
            gen(node->increment);
        }
        printf("  jmp .Lbegin%d\n", node->label_number);
        printf(".Lend%d:\n", node->label_number);
        printf("### END FOR(%d)\n", node->label_number);
        return;
    case ND_BLOCK:
        while (node->block) {
            gen(node->block);
            node = node->block;
        }
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_EQ:  // ==
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:  // !=
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:  // >
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:  // >=
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }

    printf("  push rax\n");
}