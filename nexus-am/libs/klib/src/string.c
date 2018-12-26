#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int i = 0;
  while (s[i++]);
  return --i;
}

char *strcpy(char* dst,const char* src) {
  int i = 0;
  while (1) {
    dst[i] = src[i];
    if (!src[i]) break;
    i++;
  }  
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  for (int i = 0 ; i < n ; ++i) {
    dst[i] = src[i];
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  int i = 0, j = 0;
  while (dst[j]) j++;
  while (src[i]) dst[j++] = src[i++];
  dst[j++] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  int i = 0;
  while (s1[i] && s2[i]) {
    if (s1[i] != s2[i]) return s1[i] - s2[i];
    i++;
  }
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  for (int i = 0 ; i < n ; ++i) {
    if (s1[i] != s2[i]) return s1[i] - s2[i];
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  for (int i = 0 ; i < n ; ++i) {
    *(char *)(v + i) = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  unsigned char *src = (unsigned char *) in;
  unsigned char *dst = (unsigned char *) out;
  for (int i = 0 ; i < n ; ++i) {
    *(dst + i) = *(src + i);
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if (!n) return 0;
  int i;
  unsigned char *us1 = (unsigned char *) s1;
  unsigned char *us2 = (unsigned char *) s2;
  for (i = 0 ; i < n; ++i) {
    if (*(us1 + i) != *(us2 + i)) return *(us1 + i) - *(us2 + i);
  }
  return 0;
}

#endif
