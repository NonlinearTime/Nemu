#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  int i = 0;
  char buffer[1024];
  va_list args;
  va_start(args,fmt);
  vsprintf(buffer,fmt,args);
  va_end(args);
  while (buffer[i]) _putc(buffer[i++]);
  
  return 0;
}

int write_Int(char* buffer,int value, int prefix_n, char prefix, int digit) {
  int len = 0;
  char rec[100];
  int vt = value;
  if (value < 0) {
    value *= -1;
    *buffer++ = '-';
  }
  while (1) {
    if (digit) {
      int t = value % 10;
      rec[len++] = '0' + t;
      value /= 10;
    } else {
      int t = value % 16;
      rec[len++] = t < 10 ? '0' + t : 'a' + t - 10;
      value /= 16;
    }
    if (!value) break;
  }
  int tmp = len;
  if(len < prefix_n) {
    int j = prefix_n - len;
	  while(j--) {
		  *buffer++ = prefix;
	  }
    len = prefix_n;
  }

  while (tmp--) {
    *buffer++ = rec[tmp];
  }

  return vt >= 0 ? len : len + 1;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char* buffer = out;
  // unsigned int buffer_length=strlen(buffer);
  unsigned int fmt_length=strlen(fmt);
  unsigned int index=0;
     
  int temp;
  char ctemp;
  char* stemp;
  int width = 0;
  char apd = ' ';

  for(index=0;index<fmt_length;++index)
  {
    // _putc(fmt[index]);
    if(fmt[index]!='%')
      (*buffer++)=fmt[index];
    else {
      index++;
      while(fmt[index] == ' ' || fmt[index] == '0') {
        if(fmt[index] == ' ')  apd = ' ';
        else if(fmt[index] == '0') apd = '0';
        index++;
      }
      width = 0;
      if(fmt[index] >= '0' && fmt[index] <= '9') {
	      while(fmt[index] >= '0' && fmt[index] <= '9') {
		      width = width*10 + fmt[index] -'0';
		      index++;
	      }
      }
      switch(fmt[index]) {
        case 'd':
          temp=va_arg(ap,int);
          buffer=buffer+write_Int(buffer,temp, width, apd, 1);
          break;
        case 's':
          stemp=(char*)va_arg(ap,char*);
          strcpy(buffer,stemp);
          buffer+=strlen(stemp) - 1;
          break;
        case 'c':
          ctemp=va_arg(ap,int);
          *buffer++ =ctemp;
          break;
        case 'x':
          temp=va_arg(ap,int);
          buffer=buffer+write_Int(buffer,temp, width, apd, 0);
          break;
        case 'p':
          temp=va_arg(ap,int);
          buffer=buffer+write_Int(buffer,temp, width, apd, 0);
          break;
      }
    }
  }
  *buffer = '\0';

  return strlen(out);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int ret;
  va_start(args,fmt);
  ret = vsprintf(out,fmt,args);
  va_end(args);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
