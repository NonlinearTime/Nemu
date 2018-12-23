#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case 0x81: panic("event ID = %d", e.event);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
