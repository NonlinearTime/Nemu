#include <stdio.h>

int main() {
  FILE *fp = fopen("/dev/events", "r");
  volatile int j = 0;
  while(1) {
    j ++;
    if (j == 1000000) {
      char buf[256];
      char *p = buf, ch;
      // printf("fuckl!\n");
      while ((ch = fgetc(fp)) != -1) {
        *p ++ = ch;
        printf("%c|", *(p-1));
        if(ch == '\n') {
          *p = '\0';
          printf("\n");
          break;
        }
      }
      p = buf;
      if (*p == 0) {printf("0 ");}
      printf("%p %s\n", p, buf);
      int i = 0;
      for (i = 0 ; i < 256; ++i) printf("%c", buf[i]);
      printf("\n");
      // printf("receive event: %s", buf);
      j = 0;
    }
  }

  fclose(fp);
  return 0;
}

