#include "debugger.h"

bool debugger_add_breakpoint(struct debugger *debugger, uint16_t addr) {}

bool debugger_add_watch_address(struct debugger *debugger, uint16_t addr);

void debugger_destroy(struct debugger *debugger);
