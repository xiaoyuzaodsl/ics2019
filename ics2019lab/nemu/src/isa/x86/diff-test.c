#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  //return false;
  uint32_t r[10];
  ref_difftest_getregs(r);
  int flag = 0;
  for(int i = 0;i < 9;i++){
		if( r[i]  !=   *((uint32_t*)(ref_r+i))){
			printf("diff: 0x%x:%08x in NEMU and %08x in QEMU\n",cpu.pc,r[i], *((uint32_t *)ref_r+i));
		    flag = 1;
		}
  }
  //eflags
  if(r[9]!=*((uint32_t*)(ref_r+9))){
		printf("Error in  EFLAGS\n");
		flag = 1;
  }
  if(flag==0) return true;
  return false;
}

void isa_difftest_attach(void) {
}
