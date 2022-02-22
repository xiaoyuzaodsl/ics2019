#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
	printf("eax : 0x%x and %d\n",cpu.eax,cpu.eax);
	printf("ecx : 0x%x and %d\n",cpu.ecx,cpu.ecx);
	printf("edx : 0x%x and %d\n",cpu.edx,cpu.edx);
	printf("ebx : 0x%x and %d\n",cpu.ebx,cpu.ebx);

	printf("esp : 0x%x and %d\n",cpu.esp,cpu.esp);
	printf("ebp : 0x%x and %d\n",cpu.ebp,cpu.ebp);
	printf("esi : 0x%x and %d\n",cpu.esi,cpu.esi);
	printf("edi : 0x%x and %d\n",cpu.edi,cpu.edi);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
	*success = true;
	if(!strcmp(s,"$eax")) return cpu.eax;
	if(!strcmp(s,"$ebx")) return cpu.ebx;
	if(!strcmp(s,"$ecx")) return cpu.ecx;
	if(!strcmp(s,"$edx")) return cpu.edx;
	if(!strcmp(s,"$esp")) return cpu.esp;
	if(!strcmp(s,"$ebp")) return cpu.ebp;
	if(!strcmp(s,"$esi")) return cpu.esi;
	if(!strcmp(s,"$edi")) return cpu.edi;

	if(!strcmp(s,"$ax")) return cpu.gpr[R_AX]._16;
	if(!strcmp(s,"$bx")) return cpu.gpr[R_BX]._16;
	if(!strcmp(s,"$cx")) return cpu.gpr[R_CX]._16;
	if(!strcmp(s,"$dx")) return cpu.gpr[R_DX]._16;

	if(!strcmp(s,"$ah")) return cpu.gpr[R_AX]._8[1];
	if(!strcmp(s,"$bh")) return cpu.gpr[R_AX]._8[1];
	if(!strcmp(s,"$ch")) return cpu.gpr[R_AX]._8[1];
	if(!strcmp(s,"$dh")) return cpu.gpr[R_AX]._8[1];


	if(!strcmp(s,"$al")) return cpu.gpr[R_AL]._8[0];
	if(!strcmp(s,"$bl")) return cpu.gpr[R_AL]._8[0];
	if(!strcmp(s,"$cl")) return cpu.gpr[R_AL]._8[0];
	if(!strcmp(s,"$dl")) return cpu.gpr[R_AL]._8[0];
	*success = false;
	return 0;
}
