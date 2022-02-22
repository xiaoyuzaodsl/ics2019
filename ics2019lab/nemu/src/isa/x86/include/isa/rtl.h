#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  cpu.esp = cpu.esp - 4;
  vaddr_write(cpu.esp,(*src1),4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
  *dest = vaddr_read(cpu.esp,4);
  cpu.esp = cpu.esp+4;
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  //TODO();
  //我已经知道了src1,src2跟结果，来判断是否出现溢出？
  //负-正=正，正-负 = 负
  t0 = *src1;
  t1 = *src2;
  uint32_t tmpo = (*res);
  switch(width){
	  case 1: 
		  t0 = (t0 & 0xff) >> 7; t1 = (t1 & 0xff) >> 7; 
		  tmpo = (tmpo & 0xff) >> 7;
		  break;
	  case 2: 
		  t0 = (t0 & 0xffff) >> 15; t1 = (t1 & 0xffff) >> 15; 
		  tmpo = (tmpo & 0xffff) >> 15;
		  break;
	  case 4: 
		  t0 = (t0 & 0xffffffff) >> 31; t1 = (t1 & 0xffffffff) >> 31; 
		  tmpo = (tmpo & 0xffffffff) >> 31;
		  break;
	  default: 
		  printf("fuck you, wrong in rtl_is_sub_overflow\n"); 
		  break;
  }
  *dest = ((!t0)&&(t1)&&(tmpo)) || ((t0)&&(!t1)&&(!tmpo));
  /*
	t1 = *src1;t2 = *src2;t0 = *res;
	t3 = ((((int32_t)(t1) < 0) == (((int32_t)(t2) >> 31) == 0)) && (((int32_t)(t0) < 0) != ((int32_t)(t1) < 0))); // 负正得正 正负得负
	*dest = t3;*/
}
//OF针对有符号数，CF针对无符号数
static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  // 借位跟进位
  //TODO();
  *dest = ((*src1) < (*res));
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  //TODO();
  //负+负=正，正+正 = 负
  t0 = *src1;
  t1 = *src2;
  uint32_t tmpo = (*res);
  switch(width){
	//dest = ((*src1) < (*src2));
	  case 1: 
		  t0 = (t0 & 0xff) >> 7; t1 = (t1 & 0xff) >> 7; 
		  tmpo = (tmpo & 0xff) >> 7;
		  break;
	  case 2: 
		  t0 = (t0 & 0xffff) >> 15; t1 = (t1 & 0xffff) >> 15; 
		  tmpo = (tmpo & 0xffff) >> 15;
		  break;
	  case 4: 
		  t0 = (t0 & 0xffffffff) >> 31; t1 = (t1 & 0xffffffff) >> 31; 
		  tmpo = (tmpo & 0xffffffff) >> 31;
		  break;
	  default: 
		  printf("fuck you, wrong in rtl_is_sub_overflow\n"); 
		  break;
  }
  *dest = ((!t0)&&(!t1)&&(tmpo)) || ((t0)&&(t1)&&(!tmpo));
  
	/*
	t0 = *res;t1 = *src1;t2=*src2;
	t3 = ((((int32_t)(t1) >= 0) ^ (((int32_t)(t2) >= 0 ))) && (((int32_t)(t0) < 0) ^ (((int32_t)(t2) >= 0 )) )); //正正得负 负负得正
	*dest = t3;
	*/
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  //TODO();
  *dest = ((*res) < (*src1));
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.EFLAGS.f =  (*src);\
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu.EFLAGS.f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  //TODO();
  cpu.EFLAGS.ZF = ((((*result) << (32-8*width)) >> (32-8*width))==0);
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  //TODO();
  cpu.EFLAGS.SF = ((((*result) << (32-8*width) >> (32-8*width)) >> (width*8-1))==1);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
