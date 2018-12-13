#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#define EVAL_ERROR 0xffffffff

uint32_t eval(int p, int q);

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DEC

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
  int priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE, 0},    // spaces
  {"\\+", '+', 5},         // plus
  {"==", TK_EQ, 7},        // equal
  {"\\-", '-', 5},         // subtract 
  {"\\*", '*', 3},         // multiple
  {"/", '/', 3},           // divide
  {"\\(", '(', 1},         // left parentheses
  {"\\)", ')', 1},         // right parentheses
  {"[0-9]+", TK_DEC, 0},   // Decimal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  int priority;
} Token;

Token tokens[1080];
int nr_token;

static void my_strcpy(char *dest, char *src, int n) {
  strncpy(dest, src, n);
  dest[n] = '\0';
  return;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        // char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_DEC: {
            tokens[nr_token].type = TK_DEC;
            my_strcpy(tokens[nr_token].str, e + position - substr_len, substr_len);
            break;
          };
          case TK_NOTYPE: break;
          default: {
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
          };
        }

        nr_token += 1;
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  int nr_parentheses = 0;
  for (i = 0 ; i < nr_token ; ++i) {
    if (tokens[i].type == '(') nr_parentheses++;
    if (tokens[i].type == ')') nr_parentheses--;
    if (nr_parentheses < 0) return false;
  }
  if (nr_parentheses != 0) return false;
  if (tokens[0].type == ')' || tokens[nr_token - 1].type == '(') return false;
  for (i = 0 ; i < nr_token - 1 ; ++i) {
    if (tokens[i + 1].type == ')' && !(tokens[i].type == TK_DEC || tokens[i].type == ')')) return false;
    if (tokens[i].type == '(' && !(tokens[i + 1].type == TK_DEC || tokens[i + 1].type == '(')) return false;
  }

  return true;
}

static inline bool check_parentheses(int p, int q) {
  int i, nr_parentheses = 0;
  if (!(tokens[p].type == '(' && tokens[q].type == ')')) return false;
  for (i = p ; i <= q; ++i) {
    if (tokens[i].type == '(') nr_parentheses++;
    if (tokens[i].type == ')') {
      nr_parentheses--;
      if (nr_parentheses == 0 && i != q) 
        return false;
    }
  }
  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("Bad expression.\n");
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  uint32_t res = eval(0, nr_token - 1);
  *success = res != EVAL_ERROR;
  if (*success) { 
    // printf("%u\n", res);
    return res;
  }
  else printf("Failed\n");
  return 0;
}

static int find_main_op(int p, int q) {
  int i ;
  int pos = 0;
  int priority = -1;
  int in_parentheses = 0;
  
  for (i = p; i < q; ++i) {
    int t = tokens[i].type;
    int p = tokens[i].priority;
    if (t == TK_DEC) continue;
    else if (t == '(') {
      in_parentheses++;
      continue;
    } else if (t == ')') {
      in_parentheses--;
      continue;
    } else {
      if (p >= priority) {
        priority = p;
        pos = i;
      }
    }
  }
  return pos;
}

uint32_t eval(int p, int q) {
  // printf("%d, %d\n", p, q);
  if (p > q) {
    // printf("Bad expression.\n");
    return EVAL_ERROR;
  } else if (p == q) {
    assert(tokens[p].type == TK_DEC);
    // printf("%u\n", atoi(tokens[p].str));
    return (uint32_t)atoi(tokens[p].str);
  } else if (check_parentheses(p, q)) {
    return eval(p + 1, q - 1);
  } else {
    int op_pos = find_main_op(p, q);
    // printf("main op pos: %d\n", op_pos);
    uint32_t val_l = eval(p, op_pos - 1);
    uint32_t val_r = eval(op_pos + 1, q);
    if (val_l == EVAL_ERROR || val_r == EVAL_ERROR) {
      printf("main op pos: %d\n", op_pos);printf("1\n"); 
      return EVAL_ERROR;
    }
    switch(tokens[op_pos].type) {
      case '+': /*printf("%u\n", val_l + val_r);*/ return val_l + val_r;
      case '-': /*printf("%u\n", val_l - val_r);*/ return val_l - val_r;
      case '*': /*printf("%u\n", val_l * val_r);*/ return val_l * val_r;
      case '/': if (val_r == 0) return EVAL_ERROR; else /*printf("%u\n", val_l / val_r);*/ return val_l / val_r;
      default : return EVAL_ERROR;
    }
  }
}
