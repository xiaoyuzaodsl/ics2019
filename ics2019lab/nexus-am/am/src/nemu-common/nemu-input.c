#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define key_port 0x60
#define KEYDOWN_MASK 0x8000
unsigned long long press;
size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
	  press = inl(key_port);
      kbd->keycode = press;
	  if(press!=_KEY_NONE){
		 if(kbd->keydown==0)kbd->keydown = 1;
		 else kbd->keydown = 0;
	  }
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
