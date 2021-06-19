#ifndef _HW4_DEBUGGER_H
#define _HW4_DEBUGGER_H

#define LOGD "** "

#include <stdint.h>
#include <sys/types.h>

enum dbg_state {
    DBG_STATE_INIT,
    DBG_STATE_LOADED,
    DBG_STATE_RUNNING,
};

typedef struct dbg_ctx {
    enum dbg_state state;
    pid_t child_pid;
    unsigned long prev_disasm_addr, prev_dump_addr;
} dbg_ctx;

void dbg_ctx_init(dbg_ctx *ctx);
void dbg_shutdown(dbg_ctx *ctx);
int dbg_brkpt_disable(dbg_ctx *ctx, unsigned int id);
int dbg_brkpt_enable(dbg_ctx *ctx, unsigned int id);
int dbg_cmd_break(dbg_ctx *ctx, unsigned long addr);
int dbg_cmd_cont(dbg_ctx *ctx);
int dbg_cmd_delete(dbg_ctx *ctx, unsigned int id);
int dbg_cmd_disasm(dbg_ctx *ctx, unsigned long addr);
int dbg_cmd_dump(dbg_ctx *ctx, unsigned long addr);
int dbg_cmd_exit(dbg_ctx *ctx);
int dbg_cmd_get(dbg_ctx *ctx, const char *reg);
int dbg_cmd_getregs(dbg_ctx *ctx);
int dbg_cmd_help(dbg_ctx *ctx);
int dbg_cmd_list(dbg_ctx *ctx);
int dbg_cmd_load(dbg_ctx *ctx, const char *path);
int dbg_cmd_run(dbg_ctx *ctx, const char *argv[]);
int dbg_cmd_vmmap(dbg_ctx *ctx);
int dbg_cmd_set(dbg_ctx *ctx, const char *reg, unsigned long value);
int dbg_cmd_si(dbg_ctx *ctx);
int dbg_cmd_start(dbg_ctx *ctx, const char *argv[]);

#endif

