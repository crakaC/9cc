# Memo
[x86-64命令セット チートシート](https://www.sigbus.info/compilerbook#%E4%BB%98%E9%8C%B21x86-64%E5%91%BD%E4%BB%A4%E3%82%BB%E3%83%83%E3%83%88-%E3%83%81%E3%83%BC%E3%83%88%E3%82%B7%E3%83%BC%E3%83%88)

### 省略記法
- src, dst: 同じサイズの2つの任意のレジスタ
- r8, r16, r32, r64: それぞれ8ビット、16ビット、32ビット、64ビットのレジスタ
- imm: 即値
- reg1:reg2: 2つのレジスタreg1, reg2をそれぞれ上位ビットと下位ビットとして、128ビットのような一つのレジスタに入らない大きな数を表す場合の記法。

## 整数レジスタ一覧
64ビット整数レジスタの一覧とそのエイリアスの名前
| 64  | 32   | 16   | 8    |
| --- | ---- | ---- | ---- |
| RAX | EAX  | AX   | AL   |
| RDI | EDI  | DI   | DIL  |
| RSI | ESI  | SI   | SIL  |
| RDX | EDX  | DX   | DL   |
| RCX | ECX  | CX   | CL   |
| RBP | EBP  | BP   | BPL  |
| RSP | ESP  | SP   | SPL  |
| RBX | EBX  | BX   | BL   |
| R8  | R8D  | R8W  | R8B  |
| R9  | R9D  | R9W  | R9B  |
| R10 | R10D | R10W | R10B |
| R11 | R11D | R11W | R11B |
| R12 | R12D | R12W | R12B |
| R13 | R13D | R13W | R13B |
| R14 | R14D | R14W | R14B |
| R15 | R15D | R15W | R15B |

ABIにおける使い方。関数から戻るときにもとの値に戻さなくて良いレジスタにはチェックマークつき

| レジスタ |      使い方       | 後始末不要 |
| :------: | :---------------: | ---------: |
|   RAX    | 返り値 / 引数の数 |          ✔ |
|   RDI    |      第1引数      |          ✔ |
|   RSI    |      第2引数      |          ✔ |
|   RDX    |      第3引数      |          ✔ |
|   RCX    |      第4引数      |          ✔ |
|   RBP    |  ベースポインタ   |            |
|   RSP    | スタックポインタ  |            |
|   RBX    |   （特になし）    |            |
|    R8    |      第5引数      |          ✔ |
|    R9    |      第6引数      |          ✔ |
|   R10    |   （特になし）    |          ✔ |
|   R11    |   （特になし）    |          ✔ |
|   R12    |   （特になし）    |            |
|   R13    |   （特になし）    |            |
|   R14    |   （特になし）    |            |
|   R15    |   （特になし）    |            |

関数呼び出しを行うときは、RSPが16の倍数になっている(16にアラインされている)状態でcall命令を呼ぶ必要がある。
この条件を満たさない関数呼び出しはABIに準拠しておらず、一部の関数がクラッシュするこがあります。

## メモリアクセス
| 命令           | 説明                                 |
| -------------- | ------------------------------------ |
| mov dst, [r64] | r64が差すアドレスからdstに値をロード |
| mov [r64], src | srcの値をr64が差すアドレスにストア   |
| push r64/imm   | RSPを8減らして、r64/immをRSPにストア |
| pop r64        | RSPからr64にロードして、RSPを8増やす |

## 関数呼び出し
| 命令       | 説明                                               |
| ---------- | -------------------------------------------------- |
| call label | RIPをスタックにプッシュしてlabelにジャンプ         |
| call r64   | RIPをスタックにプッシュしてr64のアドレスにジャンプ |
| ret        | スタックをポップしてそのアドレスにジャンプ         |
| leave      | mov rsp, rbpの後pop rbpと同等                      |

## 条件分岐
| 命令                         | 説明                                                 |
| ---------------------------- | ---------------------------------------------------- |
| cmp reg1, reg2/imm je label  | reg1 == reg2/immならlabelにジャンプ                  |
| cmp reg1, reg2/imm jne label | reg1 != reg2/immならlabelにジャンプ                  |
| cmp reg1, reg2/imm jl label  | reg1 < reg2ならlabelにジャンプ （符号ありでの比較）  |
| cmp reg1, reg2/imm jle label | reg1 <= reg2ならlabelにジャンプ （符号ありでの比較） |

## 条件代入
| 命令                                      | 説明                                              |
| ----------------------------------------- | ------------------------------------------------- |
| cmp reg1, reg2/imm sete al movzb eax, al  | RAX = (reg1 == reg2) ? 1 : 0                      |
| cmp reg1, reg2/imm setne al movzb eax, al | RAX = (reg1 != reg2) ? 1 : 0                      |
| cmp reg1, reg2/imm setl al movzb eax, al  | RAX = (reg1 > reg2) ? 1 : 0 （符号ありでの比較）  |
| cmp reg1, reg2/imm setle al movzb eax, al | RAX = (reg1 >= reg2) ? 1 : 0 （符号ありでの比較） |

## 整数・論理演算
| 命令             | 説明                                                                                                                 |
| ---------------- | -------------------------------------------------------------------------------------------------------------------- |
| add dst, src/imm | dst = dst + src/imm                                                                                                  |
| sub dst, src/imm | dst = dst - src/imm                                                                                                  |
| mul src          | RDX:RAX = RAX * src                                                                                                  |
| imul dst, src    | dst = dst * src                                                                                                      |
| div r32          | EAX = EDX:EAX / r32 EDX = EDX:EAX % r32                                                                              |
| div r64          | RAX = RDX:RAX / r64 RDX = RDX:RAX % r64                                                                              |
| idiv r32/r64     | divの符号ありバージョン                                                                                              |
| cqo              | RAXを128ビットに符号拡張して RDX:RAXにストア                                                                         |
| and dst, src     | dst = src & dst                                                                                                      |
| or dst, src      | dst = src \| dst                                                                                                     |
| xor dst, src     | dst = src ^ dst                                                                                                      |
| neg dst          | dst = -dst                                                                                                           |
| not dst          | dst = ~dst                                                                                                           |
| shl dst, imm/CL  | immかCLレジスタの値だけdstを左シフトする（レジスタでシフト量を指定する場合、CLしか使えない）                         |
| shr dst, imm/CL  | immかCLレジスタの値だけdstを論理右シフトする シフトインされてきた上位ビットはゼロクリアされる                        |
| sar dst, imm/CL  | immかCLレジスタの値だけdstを算術右シフトする シフトインされてきた上位ビットは、もともとのdstの符号ビットと同じになる |
| lea dst, [src]   | [src]のアドレス計算を行うが、メモリアクセスは行わずアドレス計算の結果そのものをdstにストア                           |
| movsb dst, r8    | r8を符号拡張してdstにストア                                                                                          |
| movzb dst, r8    | r8を符号拡張せずにdstにストア                                                                                        |
| movsw dst, r16   | r16を符号拡張してdstにストア                                                                                         |
| movzw dst, r16   | r16を符号拡張せずにdstにストア                                                                                       |