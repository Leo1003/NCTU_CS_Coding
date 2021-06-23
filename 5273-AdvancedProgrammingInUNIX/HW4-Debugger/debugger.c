#include "debugger.h"
#include "register.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <capstone/capstone.h>

void dbg_ctx_init(dbg_ctx *ctx)
{
    memset(ctx, 0, sizeof(dbg_ctx));
    ctx->state = DBG_STATE_INIT;
}

static data_t dbg_write_mem(dbg_ctx *ctx, uintptr_t addr, udata_t mask, udata_t data)
{
    errno = 0;
    udata_t val = ptrace(PTRACE_PEEKDATA, ctx->pid, addr, NULL);
    if (errno != 0) {
        return -1;
    }

    udata_t orig = val & mask;
    val = ((val & ~mask) | (data & mask));
    if (ptrace(PTRACE_POKEDATA, ctx->pid, addr, val) < 0) {
        return -1;
    }

    return (data_t)orig;
}

void dbg_shutdown(dbg_ctx *ctx)
{
    switch (ctx->state) {
        case DBG_STATE_INIT:
            break;
        case DBG_STATE_LOADED:
            break;
        case DBG_STATE_RUNNING:
            // TODO
            break;
    }

    ctx->state = DBG_STATE_INIT;
}

static dbg_brkpt *dbg_brkpt_find_id(dbg_ctx *ctx, unsigned id)
{
    dbg_brkpt *cur = ctx->brkpt;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }

    errno = ENOENT;
    return NULL;
}

static dbg_brkpt *dbg_brkpt_find_address(dbg_ctx *ctx, uintptr_t addr)
{
    dbg_brkpt *cur = ctx->brkpt;
    while (cur != NULL) {
        if (cur->addr == addr) {
            return cur;
        }
        cur = cur->next;
    }

    errno = ENOENT;
    return NULL;
}

static int dbg_brkpt_disable(dbg_ctx *ctx, dbg_brkpt *brkpt)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        errno = EINVAL;
        return -1;
    }

    if (brkpt->enabled) {
        if (dbg_write_mem(ctx, brkpt->addr, 0xff, brkpt->data) == -1) {
            return -1;
        }
        brkpt->enabled = false;
    }

    return 0;
}

static int dbg_brkpt_enable(dbg_ctx *ctx, dbg_brkpt *brkpt)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        errno = EINVAL;
        return -1;
    }

    if (!brkpt->enabled) {
        data_t orig;
        if ((orig = dbg_write_mem(ctx, brkpt->addr, 0xff, 0xcc)) == -1) {
            return -1;
        }
        brkpt->data = orig;
        brkpt->enabled = true;
    }

    return 0;
}

static int dbg_wait(dbg_ctx *ctx)
{
    int wstatus;
    waitpid(ctx->pid, &wstatus, 0);
    if (WIFEXITED(wstatus)) {
        debugf("child process %u terminiated normally (code %d)\n", ctx->pid, WEXITSTATUS(wstatus));
        ctx->state = DBG_STATE_LOADED;
        // TODO: Clear breakpoint
    } else if (WIFSIGNALED(wstatus)) {
        // TODO
    } else if (WIFSTOPPED(wstatus)) {
        // TODO
    }
    return 0;
}

static int dbg_step_brkpt(dbg_ctx *ctx)
{
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, ctx->pid, NULL, &regs) < 0) {
        return -1;
    }

    uintptr_t brkpt_addr = regs.rip - 1;
    dbg_brkpt *brkpt = dbg_brkpt_find_address(ctx, brkpt_addr);
    if (brkpt) {
        // Restore PC address
        regs.rip = brkpt_addr;
        if (ptrace(PTRACE_SETREGS, ctx->pid, NULL, &regs) < 0) {
            return -1;
        }
        // Disable breakpoint
        dbg_brkpt_disable(ctx, brkpt);
        // Step through
        ptrace(PTRACE_SINGLESTEP, ctx->pid, NULL, NULL);
        dbg_wait(ctx);
        // Reenable breakpoint
        dbg_brkpt_enable(ctx, brkpt);
        return 1;
    }

    return 0;
}

int dbg_cmd_break(dbg_ctx *ctx, uintptr_t addr)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    dbg_brkpt *last = NULL, *cur = ctx->brkpt;
    while (cur != NULL) {
        if (cur->addr == addr) {
            debugf("The address already has a breakpoint on it.\n");
            errno = EEXIST;
            return -1;
        }

        last = cur;
        cur = cur->next;
    }

    dbg_brkpt *newbrk = (dbg_brkpt *)malloc(sizeof(dbg_brkpt));
    if (!newbrk) {
        return -1;
    }

    newbrk->addr = addr;
    newbrk->enabled = false;
    newbrk->id = ctx->brkpt_nextid++;
    newbrk->prev = NULL;
    newbrk->next = NULL;
    if (ctx->brkpt == NULL) {
        ctx->brkpt = newbrk;
        ctx->brkpt_end = newbrk;
    } else {
        last->next = newbrk;
        newbrk->prev = cur;
        ctx->brkpt_end = newbrk;
    }
    
    return dbg_brkpt_enable(ctx, newbrk);
}

int dbg_cmd_cont(dbg_ctx *ctx)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }
    
    if (dbg_step_brkpt(ctx) < 0) {
        errorf("Failed to step through breakpoint.\n");
        return -1;
    }

    if (ptrace(PTRACE_CONT, ctx->pid, NULL, NULL) < 0) {
        errorf("Failed to continue the debugee.\n");
        return -1;
    }

    return 0;
}

int dbg_cmd_delete(dbg_ctx *ctx, unsigned id)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    dbg_brkpt *brkpt;
    if ((brkpt = dbg_brkpt_find_id(ctx, id)) == NULL) {
        debugf("Break point %u does not exist.\n", id);
        errno = ENOENT;
        return -1;
    }

    if (dbg_brkpt_disable(ctx, brkpt) < 0) {
        errorf("Failed to restore the breakpoint: %s\n", strerror(errno));
        return -1;
    }

    // Unlink prev
    if (brkpt->prev) {
        brkpt->prev->next = brkpt->next;
    } else {
        ctx->brkpt = brkpt->next;
    }
    // Unlink next
    if (brkpt->prev) {
        brkpt->next->prev = brkpt->prev;
    } else {
        ctx->brkpt_end = brkpt->prev;
    }

    free(brkpt);
    debugf("Breakpoint %u deleted.\n", id);
    return 0;
}

int dbg_cmd_disasm(dbg_ctx *ctx, uintptr_t addr)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    uintptr_t daddr = addr;
    if (daddr == 0) {
        if (ctx->prev_disasm_addr == 0) {
            debugf("No address is given.\n");
            errno = EINVAL;
            return -1;
        } else {
            daddr = ctx->prev_disasm_addr;
        }
    }

    // Maximum x86_64 instruction length is 15 bytes
    size_t dumpsize = 15 * 10;
    // TODO: Check memory boundary
    unsigned char buf[256];
    for (size_t i = 0; i < dumpsize; i += 8) {
        errno = 0;
        udata_t data = ptrace(PTRACE_PEEKDATA, ctx->pid, daddr + i, NULL);
        for (size_t o = 0; (o < 8) && (i + o < dumpsize); o++) {
            buf[i + o] = (data >> 8 * o) & 0xff;
        }
    }
    for (size_t i = 0; i < dumpsize; i++) {
        if (buf[i] == 0xcc) {
            dbg_brkpt *bt = dbg_brkpt_find_address(ctx, daddr + i);
            if (bt != NULL) {
                buf[i] = bt->data;
            }
        }
    }

    csh handle;
    cs_insn *insn;
    size_t count, real_size = 0;
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        errorf("Failed to initialize capstone.\n");
        return -1;
    }
    if ((count = cs_disasm(handle, buf, dumpsize, daddr, 10, &insn)) == 0) {
        errorf("Failed to disassemble instructions.\n");
        cs_close(&handle);
        return -1;
    }
    for (size_t i = 0; i < count; i++) {
        printf("%"PRIx64":\t%s\t\t%s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
        real_size += insn[i].size;
    }
    // Store next address
    ctx->prev_disasm_addr = daddr + real_size;

    cs_free(insn, count);
    cs_close(&handle);

    return 0;
}

int dbg_cmd_dump(dbg_ctx *ctx, uintptr_t addr)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    uintptr_t daddr = addr;
    if (daddr == 0) {
        if (ctx->prev_dump_addr == 0) {
            debugf("No address is given.\n");
            errno = EINVAL;
            return -1;
        } else {
            daddr = ctx->prev_dump_addr;
        }
    }

    size_t dumpsize = 80;
    // TODO: Check memory boundary
    unsigned char buf[256];
    for (size_t i = 0; i < dumpsize; i += 8) {
        errno = 0;
        udata_t data = ptrace(PTRACE_PEEKDATA, ctx->pid, daddr + i, NULL);
        for (size_t o = 0; (o < 8) && (i + o < dumpsize); o++) {
            buf[i + o] = (data >> 8 * o) & 0xff;
        }
    }

    for (size_t i = 0; i < dumpsize; i += 16) {
        printf("%"PRIx64": ", daddr + i);
        for (size_t o = 0; (o < 16) && (i + o < dumpsize); o++) {
            printf("%02x ", buf[i + o]);
        }
        printf(" |");
        for (size_t o = 0; (o < 16) && (i + o < dumpsize); o++) {
            if (isprint(buf[i + o])) {
                printf("%c", buf[i + o]);
            } else {
                printf(".");
            }
        }
        printf("|\n");
    }
    ctx->prev_disasm_addr = daddr + dumpsize;

    return 0;
}

int dbg_cmd_exit(dbg_ctx *ctx)
{
    dbg_shutdown(ctx);
    return 0;
}

int dbg_cmd_get(dbg_ctx *ctx, const char *reg)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    const struct register_mapping *regmap = register_lookup(reg);
    if (regmap == NULL) {
        debugf("Invalid register name.\n");
        errno = ENOENT;
        return -1;
    }

    errno = 0;
    data_t val = ptrace(PTRACE_PEEKUSER, ctx->pid, regmap->offset, NULL);
    if (errno != 0) {
        errorf("Failed to PTRACE_PEEKUSER.\n");
        return -1;
    }
    printf("%s = %ld (0x%lx)\n", regmap->name, val, val);

    return 0;
}

int dbg_cmd_getregs(dbg_ctx *ctx)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, ctx->pid, NULL, &regs) < 0) {
        return -1;
    }

    printf("RAX %-16llx RBX %-16llx RCX %-16llx RDX %-16llx\n", regs.rax, regs.rbx, regs.rcx, regs.rdx);
    printf("R8  %-16llx R9  %-16llx R10 %-16llx R11 %-16llx\n", regs.r8, regs.r9, regs.r10, regs.r11);
    printf("R12 %-16llx R13 %-16llx R14 %-16llx R15 %-16llx\n", regs.r12, regs.r13, regs.r14, regs.r15);
    printf("RDI %-16llx RSI %-16llx RBP %-16llx RSP %-16llx\n", regs.rdi, regs.rsi, regs.rbp, regs.rsp);
    printf("RIP %-16llx FLAGS %016llx\n", regs.rip, regs.eflags);
    return 0;
}

int dbg_cmd_help()
{
    printf("- break [instruction-address]: add a break point\n");
    printf("- cont: continue execution\n");
    printf("- delete <break-point-id>: remove a break point\n");
    printf("- disasm <address>: disassemble instructions in a file or a memory region\n");
    printf("- dump [address]: dump memory content\n");
    printf("- exit: terminate the debugger\n");
    printf("- get <reg>: get a single value from a register\n");
    printf("- getregs: show registers\n");
    printf("- help: show this message\n");
    printf("- list: list break points\n");
    printf("- load <program file>: load a program\n");
    printf("- run: run the program\n");
    printf("- vmmap: show memory layout\n");
    printf("- set <reg> <val>: get a single value to a register\n");
    printf("- si: step into instruction\n");
    printf("- start: start the program and stop at the first instruction\n");
    return 0;
}

int dbg_cmd_list(dbg_ctx *ctx)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    dbg_brkpt *cur = ctx->brkpt;
    while (cur != NULL) {
        printf("%-3u: %lx\n", cur->id, cur->addr);
        cur = cur->next;
    }

    return 0;
}

int dbg_cmd_load(dbg_ctx *ctx, const char *path);

int dbg_cmd_run(dbg_ctx *ctx, const char *argv[]);

int dbg_cmd_vmmap(dbg_ctx *ctx);

int dbg_cmd_set(dbg_ctx *ctx, const char *reg, uint64_t value)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    const struct register_mapping *regmap = register_lookup(reg);
    if (regmap == NULL) {
        debugf("Invalid register name.\n");
        errno = ENOENT;
        return -1;
    }

    if (ptrace(PTRACE_POKEUSER, ctx->pid, regmap->offset, value) < 0) {
        errorf("Failed to PTRACE_POKEUSER.\n");
        return -1;
    }

    return 0;
}

int dbg_cmd_si(dbg_ctx *ctx)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    int result = dbg_step_brkpt(ctx);
    if (result < 0) {
        return -1;
    }
    if (result == 0) {
        ptrace(PTRACE_SINGLESTEP, ctx->pid, NULL, NULL);
        dbg_wait(ctx);
    }

    return 0;
}

int dbg_cmd_start(dbg_ctx *ctx, const char *argv[]);


