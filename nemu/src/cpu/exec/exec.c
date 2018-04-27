#include "cpu/exec.h"
#include "all-instr.h"

typedef struct {
  DHelper decode;
  EHelper execute;
  int width;
} opcode_entry;

#define IDEXW(id, ex, w)   {concat(decode_, id), concat(exec_, ex), w}
#define IDEX(id, ex)       IDEXW(id, ex, 0)
#define EXW(ex, w)         {NULL, concat(exec_, ex), w}
#define EX(ex)             EXW(ex, 0)
#define EMPTY              EX(inv)

static inline void set_width(int width) {
  if (width == 0) {
       width = decoding.is_operand_size_16 ? 2 : 4;
  }
  decoding.src.width = decoding.dest.width = decoding.src2.width = width;
}

/* Instruction Decode and EXecute */
static inline void idex(vaddr_t *eip, opcode_entry *e) {
  /* eip is pointing to the byte next to opcode */
  if (e->decode)
    e->decode(eip);
  e->execute(eip);
}

static make_EHelper(2byte_esc);

#define make_group(name, item0, item1, item2, item3, item4, item5, item6, item7) \
  static opcode_entry concat(opcode_table_, name) [8] = { \
    /* 0x00 */	item0, item1, item2, item3, \
    /* 0x04 */	item4, item5, item6, item7  \
  }; \
static make_EHelper(name) { \
  idex(eip, &concat(opcode_table_, name)[decoding.ext_opcode]); \
}

/* 0x80, 0x81, 0x83 */
make_group(gp1,
    EX(add), EX(or), EMPTY, EX(sbb),
    EX(and), EX(sub) , EX(xor), EX(cmp))

  /* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
make_group(gp2,
    EX(rol), EMPTY, EMPTY, EMPTY,
    EX(shl), EX(shr), EMPTY, EX(sar))

  /* 0xf6, 0xf7 */
make_group(gp3,
    IDEX(test,test), EMPTY, EX(not), EX(neg),
    EX(mul), EX(imul1), EX(div), EX(idiv))

  /* 0xfe */
make_group(gp4,
    EMPTY, EX(dec), EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY)

  /* 0xff */
make_group(gp5,
    EX(inc), EX(dec), EX(call), EMPTY,
    EX(jmp), EMPTY, EX(push), EMPTY)

  /* 0x0f 0x01*/
make_group(gp7,
    EMPTY, EMPTY, EMPTY, EX(lidt),
    EMPTY, EMPTY, EMPTY, EMPTY)

/* TODO: Add more instructions!!! */

opcode_entry opcode_table [512] = {
  /* 0x00 */	EMPTY, IDEX(G2E,add), IDEX(E2G,add), IDEX(E2G,add),
  /* 0x04 */	EMPTY,IDEX(or,add), EMPTY, EMPTY,
  /* 0x08 */	IDEX(G2E,or), IDEX(G2E,or), IDEX(E2G,or), IDEX(E2G,or),
  /* 0x0c */	IDEX(or,or), IDEX(or,or), EMPTY, EX(2byte_esc),
  /* 0x10 */	EMPTY, IDEX(G2E,adc), EMPTY, IDEX(E2G,adc),
  /* 0x14 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */	EMPTY,IDEX(G2E,sbb), EMPTY, IDEX(E2G,sbb),
  /* 0x1c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x20 */	IDEX(G2E,and), IDEX(G2E,and), IDEX(E2G,and), IDEX(E2G,and),
  /* 0x24 */	EMPTY, IDEX(and_0x25,and), EMPTY, EMPTY,
  /* 0x28 */	EMPTY, IDEX(G2E,sub), EMPTY, IDEX(E2G,sub),
  /* 0x2c */	EMPTY, IDEX(and_0x25,sub), EMPTY, EMPTY,
  /* 0x30 */	IDEX(G2E,xor), IDEX(G2E,xor), IDEX(E2G,xor), EMPTY,
  /* 0x34 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */	IDEX(G2E,cmp), IDEX(G2E,cmp), IDEX(E2G,cmp), IDEX(E2G,cmp),
  /* 0x3c */	IDEX(I_width_8,cmp), IDEX(I_width_32,cmp), EMPTY, EMPTY,
  /* 0x40 */	IDEX(r,inc), IDEX(r,inc), IDEX(r,inc), IDEX(r,inc),
  /* 0x44 */	IDEX(r,inc), IDEX(r,inc), IDEX(r,inc), IDEX(r,inc),
  /* 0x48 */	IDEX(r,dec), IDEX(r,dec), IDEX(r,dec), IDEX(r,dec),
  /* 0x4c */	IDEX(r,dec), IDEX(r,dec), IDEX(r,dec), IDEX(r,dec),
  /* 0x50 */	IDEX(r,push), IDEX(r,push), IDEX(r,push), IDEX(r,push),
  /* 0x54 */	IDEX(r,push), IDEX(r,push), IDEX(r,push), IDEX(r,push),
  /* 0x58 */	IDEX(pop,pop), IDEX(pop,pop),IDEX(pop,pop), IDEX(pop,pop),
  /* 0x5c */	IDEX(pop,pop), IDEX(pop,pop),IDEX(pop,pop), IDEX(pop,pop),
  /* 0x60 */	EX(pusha), EX(popa), EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EX(operand_size), EMPTY,
  /* 0x68 */	IDEX(I,push), EMPTY, IDEX(push_width_8,push), EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x70 */	IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc),
  /* 0x74 */	IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc),
  /* 0x78 */	IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc),
  /* 0x7c */	IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc), IDEX(jcc,jcc),
  /* 0x80 */	IDEXW(I2E, gp1, 1), IDEX(I2E, gp1),EMPTY, IDEX(SI2E, gp1),
  /* 0x84 */	IDEX(G2E,test), IDEX(G2E,test), EMPTY, EMPTY,
  /* 0x88 */	IDEXW(mov_G2E, mov, 1), IDEX(mov_G2E, mov), IDEXW(mov_E2G, mov, 1), IDEX(mov_E2G, mov),
  /* 0x8c */	EMPTY, IDEX(lea_M2G,lea), EMPTY, EMPTY,
  /* 0x90 */	IDEX(xchg,xchg), IDEX(xchg,xchg), IDEX(xchg,xchg), IDEX(xchg,xchg),
  /* 0x94 */	IDEX(xchg,xchg), IDEX(xchg,xchg), IDEX(xchg,xchg), IDEX(xchg,xchg),
  /* 0x98 */	EX(cbw), EX(cltd), EMPTY, EMPTY,
  /* 0x9c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa0 */	IDEXW(O2a, mov, 1), IDEX(O2a, mov), IDEXW(a2O, mov, 1), IDEX(a2O, mov),
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	IDEX(I_width_8,test), EMPTY, EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb0 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb4 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb8 */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xbc */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xc0 */	IDEXW(gp2_Ib2E, gp2, 1), IDEX(gp2_Ib2E, gp2), EMPTY, IDEX(ret,ret),
  /* 0xc4 */	EMPTY, EMPTY, IDEXW(mov_I2E, mov, 1), IDEX(mov_I2E, mov),
  /* 0xc8 */	EMPTY, EX(leave), EMPTY, EMPTY,
  /* 0xcc */	EMPTY, IDEX(push_width_8,int), EMPTY, EX(iret),
  /* 0xd0 */	IDEXW(gp2_1_E, gp2, 1), IDEX(gp2_1_E, gp2), IDEXW(gp2_cl2E, gp2, 1), IDEX(gp2_cl2E, gp2),
  /* 0xd4 */	EMPTY, EMPTY, EX(nemu_trap), EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	IDEXW(in_I2a,in,1), IDEX(in_I2a,in), IDEXW(out_a2I,out,1), IDEX(out_a2I,out),
  /* 0xe8 */	IDEX(call,call), IDEX(I,jmp), EMPTY, IDEX(I_width_8,jmp),
  /* 0xec */	IDEXW(in_dx2a,in,1), IDEX(in_dx2a,in), IDEXW(out_a2dx,out,1), IDEX(out_a2dx,out),
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, IDEXW(E, gp3, 1), IDEX(E, gp3),
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, IDEXW(E, gp4, 1), IDEX(E, gp5),

  /*2 byte_opcode_table */

  /* 0x00 */	EMPTY, IDEX(gp7_E, gp7), EMPTY, EMPTY,
  /* 0x04 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x10 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x20 */	IDEX(mov_G2E,mov_cr2r), EMPTY, IDEX(mov_E2G,mov_r2cr), EMPTY,
  /* 0x24 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x28 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x2c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x30 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x34 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x3c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x40 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x48 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x4c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x50 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x54 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x58 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x5c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x60 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x68 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x70 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x74 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x78 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x7c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x80 */	IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc),
  /* 0x84 */	IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc),
  /* 0x88 */	IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc),
  /* 0x8c */	IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc), IDEX(I,jcc),
  /* 0x90 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x94 */	IDEX(set,set), IDEX(set,set), IDEX(set,set), EMPTY,
  /* 0x98 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x9c */	EMPTY, EMPTY, IDEX(set,set), IDEX(set,set),
  /* 0xa0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, IDEX(E2G,imul2),
  /* 0xb0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb4 */	EMPTY, EMPTY, IDEX(mov_E2G,movzx), IDEX(mov_E2G,movzx),
  /* 0xb8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xbc */	EMPTY, EMPTY, IDEX(mov_E2G,movsx), IDEX(mov_E2G,movsx),
  /* 0xc0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xcc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */	EMPTY, IDEX(r,jmp), EMPTY, EMPTY,
  /* 0xec */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(2byte_esc) {
  uint32_t opcode = instr_fetch(eip, 1) | 0x100;
  decoding.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(eip, &opcode_table[opcode]);
}

make_EHelper(real) {
  uint32_t opcode = instr_fetch(eip, 1);
  decoding.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(eip, &opcode_table[opcode]);
}

static inline void update_eip(void) {
  cpu.eip = (decoding.is_jmp ? (decoding.is_jmp = 0, decoding.jmp_eip) : decoding.seq_eip);
}

extern void raise_intr(uint8_t, vaddr_t);

void exec_wrapper(bool print_flag) {
#ifdef DEBUG
  decoding.p = decoding.asm_buf;
  decoding.p += sprintf(decoding.p, "%8x:   ", cpu.eip);
#endif

  decoding.seq_eip = cpu.eip;
  exec_real(&decoding.seq_eip);


#ifdef DEBUG
  int instr_len = decoding.seq_eip - cpu.eip;
  sprintf(decoding.p, "%*.s", 50 - (12 + 3 * instr_len), "");
  strcat(decoding.asm_buf, decoding.assembly);
  Log_write("%s\n", decoding.asm_buf);
  if (print_flag) {
    puts(decoding.asm_buf);
  }
#endif

#ifdef DIFF_TEST
  uint32_t eip = cpu.eip;
#endif
  update_eip();
  if (cpu.INTR && cpu.EFLAGS.IF){
      cpu.INTR=false;
      raise_intr(32,cpu.eip);
      update_eip();
  }

#ifdef DIFF_TEST
  void difftest_step(uint32_t);
  difftest_step(eip);
#endif
}
