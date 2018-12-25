#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};


extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);

size_t evnt_read(void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */

static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb",0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write}, 
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

size_t evnt_read(void *buf, size_t offset, size_t len) {
  size_t ret = events_read(buf, offset, len);
  fs_lseek(5, 0, SEEK_SET);
  return ret;
}

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[3].size = screen_height() * screen_width() * 4;
  Log("init_fs: %d\n", file_table[3].size);
  file_table[0].size = file_table[1].size = file_table[2].size= 0x7fffffff;
}

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; i < NR_FILES; ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      // Log("fs_open: %s\n",pathname);
      return i;
    }
  }
  panic("Unknown file %s\n", pathname);
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  Log("fs_read: %s offset: 0x%x length: 0x%x\n", file_table[fd].name, file_table[fd].open_offset, len);
  // assert(file_table[fd].open_offset + len <= file_table[fd].size);
  size_t l = file_table[fd].open_offset + len <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].read == NULL) ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, l);
  else l = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  // Log("fs_read: %s offset: 0x%x length: 0x%x\n", file_table[fd].name, file_table[fd].open_offset, l);
  file_table[fd].open_offset += l;
  return l; 
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  // assert(file_table[fd].open_offset + len <= file_table[fd].size);
  size_t l = file_table[fd].open_offset + len <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
  // ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, l);
  if (file_table[fd].write == NULL) ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, l);
  else l = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, l);
  file_table[fd].open_offset += l;
  // Log("fs_write: %s offset: 0x%x length: 0x%x\n", file_table[fd].name, file_table[fd].open_offset, l);
  return l;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  Log("fs_lseek: %s open_offset: 0x%x offset: 0x%x\n whence: %d\n", file_table[fd].name, file_table[fd].open_offset, offset, whence);
  switch(whence) {
    case SEEK_SET: assert(offset <= file_table[fd].size); file_table[fd].open_offset = offset; break;
    case SEEK_CUR: assert(file_table[fd].open_offset + offset <= file_table[fd].size); file_table[fd].open_offset += offset; break;
    case SEEK_END: assert(file_table[fd].open_offset <= 0); file_table[fd].open_offset = file_table[fd].size + offset; break;
    default: panic("Unkown whence.\n");
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  fs_lseek(fd, 0, SEEK_SET);
  return 0;
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}