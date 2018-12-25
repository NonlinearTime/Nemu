#include "common.h"
#include <amdev.h>

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
  _KbdReg kbd;
  _UptimeReg uptime;
  _Device *timmer = _device(1);
  _Device *input = _device(2);
  printf("events_read: %p %p\n", timmer, input);
  // assert(0);
  timmer->read(_DEVREG_TIMER_UPTIME, &uptime, 4);
  input->read(_DEV_INPUT, &kbd, 4);
  Log("events_read: %d\n", len);
  char buffer[128];
  Log("events_read: %d\n", len);
  int l;
  if (kbd.keycode == _KEY_NONE) {
    uint64_t hi = uptime.hi;
    uint64_t lo = uptime.lo;
    l = sprintf(buffer, "t %d\n", hi << 32 | lo);
  } else {
    if (kbd.keydown) l = sprintf(buffer, "kd %s\n", keyname[kbd.keycode]);
    else l = sprintf(buffer, "ku %s\n", keyname[kbd.keycode]);
  }
  Log("events_read: %d\n", len);
  l = l <= len ? l : len;
  int i;
  for (i = 0; i < l; ++i) {
    *(char *)buf++ = buffer[i];
  }
  Log("events_read: %d\n", l);
  return len;
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
  Log("fb_write: x %d y %d len %d\n", x, y, len);
  draw_rect((uint32_t* )buf, x, y, len / 4, 1);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int r = sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n\0", screen_width(), screen_height());
  Log("dispinfo_read: %s %d\n", dispinfo, r);  
}
