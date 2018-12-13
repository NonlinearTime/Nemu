#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

static int choose(int n) {
  int ind = rand() % n;
  return ind;
}

// this should be enough
static char buf[65536];

int current_pos = 0;

static char ops[] = {
  '+', '-', '*', '/'
};

static void inline gen_num() {
  char nbuf[32];
  uint32_t num = rand() % 100 + 1;
  sprintf(nbuf, "%u", num);
  int i = 0;
  while(nbuf[i]) buf[current_pos++] = nbuf[i++];
}

static void inline gen_rand_op() {
  int ind = choose(4);
  buf[current_pos++] = ops[ind];
  return;
}

static inline void gen_rand_expr() {
  switch(choose(3)) {
    case 0: { 
      gen_num();
      break; 
    }
    case 1: {
      buf[current_pos++] = '(';
      gen_num();
      buf[current_pos++] = ')';
      break;
    }
    default: {
      gen_rand_expr();
      gen_rand_op();
      gen_rand_expr();
      break;
    }
  }
  buf[current_pos] = '\0';
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    // printf("%s %d\n", buf, current_pos);
    current_pos = 0;

    sprintf(code_buf, code_format, buf);
    // printf("%s\n", code_buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
