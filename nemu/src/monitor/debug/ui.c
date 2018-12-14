#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "cpu/decode.h"

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Execute i steps", cmd_si},
  { "info", "Check the information of registers or watch points, usage: r - regs  w - wps", cmd_info},
  { "x", "Check n bytes from the given address, usage: x n expr", cmd_x}, 
  { "p", "Calculate the expression in decimal, usage: p [expr]", cmd_p},
  { "w", "Create watchpoints, usage: w [expr]", cmd_w},
  { "d", "Delete a watchpoint by number, usage: d [no]", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");

  if (arg == NULL) {
    cpu_exec(1);
  } else {
    int steps = atoi(arg);
    cpu_exec(steps);
  }
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");

  if (arg == NULL) {
    return 0;
  } else {
    if (strcmp(arg, "r") == 0) {
      printf("eax 0x%x %d\n", reg_l(R_EAX), reg_l(R_EAX));
      printf("ecx 0x%x %d\n", reg_l(R_ECX), reg_l(R_ECX));
      printf("edx 0x%x %d\n", reg_l(R_EDX), reg_l(R_EDX));
      printf("ebx 0x%x %d\n", reg_l(R_EBX), reg_l(R_EBX));
      printf("esp 0x%x 0x%x\n", reg_l(R_ESP), reg_l(R_ESP));
      printf("ebp 0x%x 0x%x\n", reg_l(R_EBP), reg_l(R_EBP));
      printf("esi 0x%x %d\n", reg_l(R_ESI), reg_l(R_ESI));
      printf("edi 0x%x %d\n", reg_l(R_EDI), reg_l(R_EDI));
      printf("eip 0x%x 0x%x %s\n", cpu.eip, cpu.eip, decoding.assembly);
      return 0;
    } else if (strcmp(arg, "w") == 0) {
      print_watchpoints();
      return 0;
    } else {
      printf("Unknown information type.\n");
      return 0;
    }
  }

}

static int cmd_x(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) return 0;
  int n = atoi(arg);
  int i;
  char *hex = strtok(NULL, " ");
  if (arg == NULL) return 0;
  paddr_t p;
  sscanf(hex, "%x", &p);

  for (i = 0; i < n; ++i) {
    int cxt = paddr_read(p, 1);
    printf("%02x ", cxt);
    p += 1;
  }

  printf("\n");

  return 0;
}

static int cmd_p(char *args) {
  bool success;
  uint32_t res = expr(args, &success);
  if(success) printf("%u\n",res);
  return 0;
}

static int cmd_w(char *args) {
  bool success;
  uint32_t ret = expr(args, &success);
  if (success) {
    WP* node = new_wp();
    sprintf(node->expression, "%s", args);
    node->old_value = ret;
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) return 0;
  int n = atoi(arg);
  free_wp(n);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
