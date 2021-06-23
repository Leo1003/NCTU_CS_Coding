#ifndef _HW4_DEBUGGER_H
#define _HW4_DEBUGGER_H

#define LOGD "** "
#define debugf(fmt, ...) printf(LOGD fmt, ##__VA_ARGS__)
#define errorf(fmt, ...) \
    do { \
        int __saved_errno = errno; \
        debugf(fmt, ##__VA_ARGS__); \
        errno = __saved_errno; \
    } while (0)

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

enum dbg_state {
    DBG_STATE_INIT,
    DBG_STATE_LOADED,
    DBG_STATE_RUNNING,
};

typedef uint64_t udata_t;
typedef int64_t data_t;

typedef struct dbg_brkpt {
    struct dbg_brkpt *prev, *next;
    unsigned id;
    uintptr_t addr;
    uint64_t data;
    bool enabled;
} dbg_brkpt;

typedef struct dbg_ctx {
    enum dbg_state state;
    pid_t pid;
    uintptr_t prev_disasm_addr, prev_dump_addr;
    dbg_brkpt *brkpt, *brkpt_end;
    unsigned brkpt_nextid;
} dbg_ctx;

void dbg_ctx_init(dbg_ctx *ctx);
void dbg_shutdown(dbg_ctx *ctx);
int dbg_cmd_break(dbg_ctx *ctx, uintptr_t addr);
int dbg_cmd_cont(dbg_ctx *ctx);
int dbg_cmd_delete(dbg_ctx *ctx, unsigned int id);
int dbg_cmd_disasm(dbg_ctx *ctx, uintptr_t addr);
int dbg_cmd_dump(dbg_ctx *ctx, uintptr_t addr);
int dbg_cmd_exit(dbg_ctx *ctx);
int dbg_cmd_get(dbg_ctx *ctx, const char *reg);
int dbg_cmd_getregs(dbg_ctx *ctx);
int dbg_cmd_help();
int dbg_cmd_list(dbg_ctx *ctx);
int dbg_cmd_load(dbg_ctx *ctx, const char *path);
int dbg_cmd_run(dbg_ctx *ctx, const char *argv[]);
int dbg_cmd_vmmap(dbg_ctx *ctx);
int dbg_cmd_set(dbg_ctx *ctx, const char *reg, uint64_t value);
int dbg_cmd_si(dbg_ctx *ctx);
int dbg_cmd_start(dbg_ctx *ctx, const char *argv[]);

#endif

