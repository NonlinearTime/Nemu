#include <../include/monitor/expr.h>
#include <stdio.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);


int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  // bool success = true;
  // uint32_t result;
  // char buf[65536];
  // while (scanf("%u %s", &result, buf) != EOF) {
  //   bool equ = false;
  //   uint32_t res = expr(buf, &success);
  //   equ = res == result;
  //   if (success) {
  //     if (!equ) printf("%d %u %u %s\n", equ, res, result, buf);
  //   } else {
  //     printf("P Error\n");
  //   }
  // }

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
