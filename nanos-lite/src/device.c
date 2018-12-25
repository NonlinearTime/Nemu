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
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);
  Log("dispinfo_read: %s offset: %d len: %d", buf, offset, len);  
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int x = offset / screen_width();
  int y = offset % screen_height();
  Log("fb_write: x %d y %d\n", x, y);
  draw_rect((uint32_t* )buf, x, y, len, 1);
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
