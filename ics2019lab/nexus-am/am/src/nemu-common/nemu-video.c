#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include<klib.h>
#define screen_port 0x100
unsigned int pix;
uint32_t *fuck = (uint32_t*)0xa0000000;
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
	  pix = inl(screen_port);
      info->width = pix >> 16;
      info->height = (pix << 16) >>16;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
	  int x = ctl->x,y = ctl->y,w = ctl->w,h = ctl->h;
	  int len = screen_width();
      for (int i = 0; i < h; i++) {
		memcpy(fuck + (y+i) * len + x, ctl->pixels + w * i, 4 * w);
	  }

      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
	int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
