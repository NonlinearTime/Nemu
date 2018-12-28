#include "common.h"
#include <amdev.h>
#include "proc.h"


size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t i;
  for (i = 0; i < len; ++i) {
    _putc(*((char *)buf + i));
  }
  // Log("serial_write: %d byte\n", i);
  return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  // char buffer[128];
  int kc = read_key(), l;
  // Log("events_read: len: %d", len);
  
  if ((kc & 0xfff) == _KEY_NONE) {
    uint32_t ut = uptime();
    l = sprintf(buf, "t %d\n", ut);
    // Log("events_read: %s", buf);
  } else {
    if (kc & 0x8000) {
      l = sprintf(buf, "kd %s\n", keyname[kc & 0xfff]);
      if ((kc & 0xfff) == _KEY_F1) {
        fg_pcb = pcbs[1];
        Log("F1: %p", fg_pcb);
      } else if ((kc & 0xfff) == _KEY_F2) {
        fg_pcb = pcbs[2];
      }
      Log("events_read: %s", buf);
    } else {
      l = sprintf(buf, "ku %s\n", keyname[kc & 0xfff]);
      Log("events_read: %s", buf);
    }
  }
  // l = l <= len ? l : len;
  // strncpy(buf, buffer, l);
  // Log("events_read: %d", l);
  return l;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);
  Log("dispinfo_read: %s offset: %d len: %d", buf, offset, len);  
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int x = (offset / 4) % screen_width();
  int y = (offset / 4) / screen_width();
  // Log("fb_write: x %d y %d len %d\n", x, y, len);
  draw_sync();
  draw_rect((uint32_t* )buf, x, y, len / 4, 1);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int r = sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
  Log("dispinfo_read: %s %d", dispinfo, r);  
}
