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
  // Log("events_read: %p %p\n", timmer, input);
  // assert(0);
  _device(1)->read(_DEVREG_TIMER_UPTIME, &uptime, 4);
  _device(2)->read(_DEVREG_INPUT_KBD, &kbd, 4);
  Log("events_read: %d\n", len);
  char buffer[128];
  Log("events_read: %d\n", len);
  int l;
  Log("kbd: %d %d\n", kbd.keycode, kbd.keydown);
  if (kbd.keycode == _KEY_NONE) {
    Log("events_read: hi %d lo %d\n", uptime.hi, uptime.lo);
    uint64_t hi = uptime.hi;
    uint64_t lo = uptime.lo;
    Log("events_read: hi %d lo %d\n", hi, lo);
    l = sprintf(buffer, "t %d\n", hi << 32 | lo);
    Log("events_read: %d\n", len);
  } else {
    
    printf("%d\n", kbd.keycode);
    Log("events_read: %d\n", len);
    if (kbd.keydown) l = sprintf(buffer, "kd %s\n", keyname[kbd.keycode & 0xfff]);
    else l = sprintf(buffer, "ku %s\n", keyname[kbd.keycode & 0xfff]);
    Log("events_read: %d\n", len);
  }
  Log("events_read: %d\n", len);
  l = l <= len ? l : len;
  int i;
  for (i = 0; i < l; ++i) {
    *(char *)buf++ = buffer[i];
  }
  *(char *)buf = '\0';
  Log("events_read: %d\n", l);
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
