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

int write_Int(char* buffer,int value) {
  int len = 0;
  char rec[100];
  int vt = value;
  if (value < 0) {
    value *= -1;
    *(buffer++)='-';
  }
  while (1) {
    int t = value % 10;
    rec[len++] = '0' + t;
    value /= 10;
    if (!value) break;
  }

  int tmp = len;
  while (tmp--) {
    *buffer++ = rec[tmp - 1];
  }

  return vt >= 0 ? len : len + 1;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char c;
  char *str = out;
  const char *tmp;
  char num_s[100];
  int i,j,len,num;
  int flag,field_width;

  for(;*fmt; fmt++)
  {
      if(*fmt != '%')
      {
	  *str++ = *fmt;
	  continue;
      }

      flag = 0;
      fmt++;
      while(*fmt == ' ' || *fmt == '0')
      {
	if(*fmt == ' ')  flag |= 8;
	else if(*fmt == '0') flag |= 1;
	fmt++;
      }
      
      field_width = 0;
      if(*fmt >= '0' && *fmt <= '9')
      {
	      while(*fmt >= '0' && *fmt <= '9')
	      {
		      field_width = field_width*10 + *fmt -'0';
		      fmt++;
	      }
      }
      else if(*fmt == '*')
      {
	      fmt++;
	      field_width = va_arg(ap,int);
      }
      //base = 10;

      switch(*fmt)
      {
	  case 's':
	      tmp = va_arg(ap,char *);
	      len = strlen(tmp);
	      for(i = 0;i < len;i ++)
	      {
		   *str++ = *tmp++;
	      }
	      continue;
	  case 'd': break;
      }

      num = va_arg(ap,int);
      j = 0;
      if(num == 0)
      {
	  num_s[j++] = '0';
      }
      else
      {
	  if(num < 0)
	  {
	      *str++ = '-';
	      num = -num;
	  }
	  //j = 0;
	  while(num)
	  {
	      num_s[j++] = num%10 + '0';
	      num /= 10;
	  }
      }
      if(j < field_width)
      {
	      num = field_width - j;
	      c = flag & 1 ? '0' : ' ';
	      while(num--)
	      {
		      *str++ = c;
	      }
      }
      while(j--)
      {
	  *str++ = num_s[j];
      }
  }
  *str = '\0';
  return 0;
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
