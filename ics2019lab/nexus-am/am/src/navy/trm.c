#include <am.h>
#define SERIAL_PORT 0x3f8
_Area _heap = {};

void _trm_init() {
}

void _putc(char ch) {

}

void _halt(int code) {
  while (1);
}
