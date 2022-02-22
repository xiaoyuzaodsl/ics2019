#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  //printf("0x%x mov val = (src = 0x%x)\n",cpu.pc,id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
	//TODO();
	s0 = id_dest->val;
	if(id_dest->width==1){
		uint8_t tmp = s0;
		int8_t tmp2 = tmp;
		s0 = tmp2;
	}
	id_dest->val = s0;
	rtl_push(&s0);
	//printf("pc: 0x%x push s0:0x%x\n",cpu.pc,s0);
	print_asm_template1(push);
}

make_EHelper(pop) {
	//TODO();
	rtl_pop(&s0);
	if(id_dest->width==1){
		uint8_t tmp = s0;
		int8_t tmp2 = tmp;
		s0 = tmp2;
	}

	operand_write(id_dest,&s0);
	print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
	//TODO();
	//???这leave怎么感觉不对劲的样子
	/*	
	s0 = id_src->val;
	operand_write(id_dest,&s0);
	*/
	/*
	if(decinfo.isa.is_operand_size_16){
		cpu.gpr[4]._16 = cpu.gpr[5]._16;
	}
	else cpu.gpr[4]._32 = cpu.gpr[5]._32;
	rtl_pop(&s0);
	cpu.gpr[5]._16 = s0;
	*/
		
	reg_l(4) = reg_l(5);
	rtl_pop(&s0);
	reg_w(5) = s0;
	
	/*
	if(id_dest->width==2){
    rtl_lr(&s0,R_EBP,2);
    rtl_sr(R_ESP,&s0,2);
  }
  else{
    rtl_lr(&s0,R_EBP,4);
    rtl_sr(R_ESP,&s0,4);
  }
  if (decinfo.isa.is_operand_size_16) {
    rtl_pop(&s0);
    rtl_sr(R_BP,&s0,2);
    //BP := Pop();
  }
  else { // OperandSize = 32 *
    rtl_pop(&s0);
    rtl_sr(R_BP,&s0,4);
    //EBP := Pop();
  }	*/
	print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
	rtl_sext(&s1, &reg_l(R_EAX), 2);
	rtl_li(&s2,31);
	rtl_shr(&s0,&s1,&s2);
	rtl_li(&s1,0x0000ffff);
	rtl_and(&reg_l(R_EDX),&s0,&s1);

  }
  else {
    //TODO();
	rtl_sext(&s1, &reg_l(R_EAX), 4);
	rtl_li(&s2,31);
	rtl_shr(&s0,&s1,&s2);
	rtl_li(&s1,0xffffffff);
	rtl_and(&reg_l(R_EDX),&s0,&s1);

  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
	rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
	rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 24);
	rtl_andi(&reg_l(R_EAX), &reg_l(R_EAX), 0x0000ffff);

  }
  else {
    //TODO();
	rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);

  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  /*printf("pc:%x : movsx:s0 = 0x%x eax:0x%x ",cpu.pc,s0,cpu.eax);
  printf("id_src->width:0x%x id_src->val:0x%x\n",id_src->width,id_src->val);
  printf("id_dest->width:0x%x id_dest->val:0x%x\n",id_dest->width,id_dest->val);
  */
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  /* 
  printf("pc:%x : movsx:s0 = 0x%x eax:0x%x ",cpu.pc,s0,cpu.eax);
  printf("id_src->width:0x%x id_src->val:0x%x\n",id_src->width,id_src->val);
  printf("id_dest->width:0x%x id_dest->val:0x%x\n",id_dest->width,id_dest->val);
  */
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
