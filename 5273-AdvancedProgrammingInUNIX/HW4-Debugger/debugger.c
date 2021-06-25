#define _GNU_SOURCE
#include "debugger.h"
#include "register.h"

#include <ctype.h>
#include <elf.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <capstone/capstone.h>

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

static int dbg_vmmap_clear(dbg_ctx *ctx)
{
    if (ctx->vmmaps != NULL) {
        free(ctx->vmmaps);
        ctx->vmmaps = NULL;
        ctx->vmmaps_len = 0;
    }

    return 0;
}

static int dbg_vmmap_reload(dbg_ctx *ctx)
{
    if (ctx->vmmaps != NULL) {
        dbg_vmmap_clear(ctx);
    }

    size_t len = 1024;
    ctx->vmmaps = (map_entry *)calloc(len, sizeof(map_entry));
    if (!ctx->vmmaps) {
        return -1;
    }

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%u/maps", ctx->pid);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        dbg_vmmap_clear(ctx);
        return -1;
    }

    char *buf = NULL;
    size_t bufsz = 0, i = 0;
    while (getline(&buf, &bufsz, fp) != EOF && i < len) {
        char *saveptr;
        char *start_s = strtok_r(buf, "-", &saveptr);
        char *end_s = strtok_r(NULL, " ", &saveptr);
        char *perm_s = strtok_r(NULL, " ", &saveptr);
        char *offset_s = strtok_r(NULL, " ", &saveptr);
        char *dev_s = strtok_r(NULL, " ", &saveptr);
        char *inode_s = strtok_r(NULL, " ", &saveptr);
        char *name_s = strtok_r(NULL, " ", &saveptr);

        ctx->vmmaps[i].start = strtoul(start_s, NULL, 16);
        ctx->vmmaps[i].end = strtoul(end_s, NULL, 16);
        ctx->vmmaps[i].offset = strtoul(offset_s, NULL, 16);
        int perm = 0;
        if (perm_s[0] == 'r') perm |= 0x04;
        if (perm_s[1] == 'w') perm |= 0x02;
        if (perm_s[2] == 'x') perm |= 0x01;
        ctx->vmmaps[i].perm = perm;
        strncpy(ctx->vmmaps[i].name, name_s, sizeof(ctx->vmmaps[i].name));

        i++;
    }
    ctx->vmmaps_len = i;

    fclose(fp);
    return 0;
}

static const map_entry *dbg_vmmap_get(dbg_ctx *ctx, uintptr_t addr)
{
    if (ctx->vmmaps == NULL && dbg_vmmap_reload(ctx) < 0) {
        return NULL;
    }

    for (size_t i = 0; i < ctx->vmmaps_len; i++) {
        const map_entry *entry = &ctx->vmmaps[i];
        if (entry->start <= addr && entry->end >= addr) {
            return entry;
        }
    }

    errno = ENOENT;
    return NULL;
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

static int dbg_brkpt_clear(dbg_ctx *ctx, bool disable)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        disable = false;
    }

    if (ctx->brkpt == NULL) {
        return 0;
    }

    dbg_brkpt *cur = ctx->brkpt;
    while (cur != NULL) {
        dbg_brkpt *being_free = cur;
        cur = cur->next;
        if (disable) {
            dbg_brkpt_disable(ctx, being_free);
        }
        free(being_free);
    }
    ctx->brkpt = NULL;
    ctx->brkpt_end = NULL;
    ctx->brkpt_nextid = 0;

    return 0;
}

static ssize_t dbg_disasm(dbg_ctx *ctx, uintptr_t addr, size_t dasmcnt)
{
    // Maximum x86_64 instruction length is 15 bytes
    size_t dumpsize = 15 * 10;
    // Check memory boundary
    const map_entry *entry = dbg_vmmap_get(ctx, addr);
    if (entry == NULL) {
        errorf("Invalid address.\n");
        return -1;
    }
    if (!(entry->perm & 0x01)) {
        errorf("Address not in executable address.\n");
        return -1;
    }
    size_t section_left = (entry->end - addr + 1);
    if (section_left < dumpsize) {
        dumpsize = section_left;
    }

    unsigned char buf[256];
    for (size_t i = 0; i < dumpsize; i += 8) {
        errno = 0;
        udata_t data = ptrace(PTRACE_PEEKDATA, ctx->pid, addr + i, NULL);
        for (size_t o = 0; (o < 8) && (i + o < dumpsize); o++) {
            buf[i + o] = (data >> 8 * o) & 0xff;
        }
    }
    for (size_t i = 0; i < dumpsize; i++) {
        if (buf[i] == 0xcc) {
            dbg_brkpt *bt = dbg_brkpt_find_address(ctx, addr + i);
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
    if ((count = cs_disasm(handle, buf, dumpsize, addr, dasmcnt, &insn)) == 0) {
        errorf("Failed to disassemble instructions.\n");
        cs_close(&handle);
        return -1;
    }
    for (size_t i = 0; i < count; i++) {
        //insn[i].
        printf("%16lx: ", insn[i].address);
        for (size_t j = 0; j < insn[i].size; j++) {
            printf("%02hhx ", insn[i].bytes[j]);
        }
        for (size_t j = 0; j < (10 - insn[i].size) * 3; j++) {
            putchar(' ');
        }
        printf("\t%s\t%s\n", insn[i].mnemonic, insn[i].op_str);
        real_size += insn[i].size;
    }
    // Store next address

    cs_free(insn, count);
    cs_close(&handle);
    return real_size;
}

static int dbg_wait(dbg_ctx *ctx)
{
    int wstatus;
restart:
    if (waitpid(ctx->pid, &wstatus, 0) < 0) {
        return -1;
    }
    if (WIFEXITED(wstatus)) {
        debugf("child process %u terminiated normally (code %d)\n", ctx->pid, WEXITSTATUS(wstatus));
        ctx->state = DBG_STATE_LOADED;
        dbg_brkpt_clear(ctx, false);
    } else if (WIFSIGNALED(wstatus)) {
        debugf("child process %u terminiated by signal %s (%d)\n", ctx->pid, strsignal(WTERMSIG(wstatus)), WTERMSIG(wstatus));
        ctx->state = DBG_STATE_LOADED;
        dbg_brkpt_clear(ctx, false);
    } else if (WIFSTOPPED(wstatus)) {
        int stopsig = WSTOPSIG(wstatus);
        if (stopsig == SIGTRAP) {
            // Breakpoint & execve
            struct user_regs_struct regs;
            if (ptrace(PTRACE_GETREGS, ctx->pid, NULL, &regs) < 0) {
                return -1;
            }

            uintptr_t brkpt_addr = regs.rip - 1;
            dbg_brkpt *bt = dbg_brkpt_find_address(ctx, brkpt_addr);
            if (bt) {
                debugf("breakpoint @");
                dbg_disasm(ctx, brkpt_addr, 1);
            }
        } else {
            // Signal delivery stop or group-stop
            siginfo_t siginfo;
            if (stopsig == SIGSTOP || stopsig == SIGTSTP || stopsig == SIGTTIN || stopsig == SIGTTOU) {
                if (ptrace(PTRACE_GETSIGINFO, ctx->pid, NULL, &siginfo) < 0 && errno == EINVAL) {
                    // Group stop
                    goto restart;
                }
            }
            ptrace(PTRACE_CONT, ctx->pid, NULL, stopsig);
            goto restart;
        }
    } else {
        return -1;
    }

    return wstatus;
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

void dbg_ctx_init(dbg_ctx *ctx)
{
    memset(ctx, 0, sizeof(dbg_ctx));
    ctx->state = DBG_STATE_INIT;
}

void dbg_shutdown(dbg_ctx *ctx)
{
    switch (ctx->state) {
        case DBG_STATE_INIT:
            break;
        case DBG_STATE_LOADED:
            free(ctx->loaded_program);
            break;
        case DBG_STATE_RUNNING:
            kill(ctx->pid, SIGKILL);
            dbg_wait(ctx);
            dbg_brkpt_clear(ctx, false);
            ctx->pid = 0;
            free(ctx->loaded_program);
            dbg_vmmap_clear(ctx);
            break;
    }

    ctx->state = DBG_STATE_INIT;
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

    dbg_wait(ctx);

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

    ssize_t real_size = dbg_disasm(ctx, daddr, 10);
    if (real_size < 0) {
        return -1;
    }
    ctx->prev_disasm_addr = daddr + real_size;

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
    // Check memory boundary
    const map_entry *entry = dbg_vmmap_get(ctx, daddr);
    if (entry == NULL) {
        errorf("Invalid address.\n");
        return -1;
    }
    size_t section_left = (entry->end - daddr + 1);
    if (section_left < dumpsize) {
        dumpsize = section_left;
    }

    unsigned char buf[256];
    for (size_t i = 0; i < dumpsize; i += 8) {
        errno = 0;
        udata_t data = ptrace(PTRACE_PEEKDATA, ctx->pid, daddr + i, NULL);
        for (size_t o = 0; (o < 8) && (i + o < dumpsize); o++) {
            buf[i + o] = (data >> 8 * o) & 0xff;
        }
    }

    for (size_t i = 0; i < dumpsize; i += 16) {
        printf("%16lx: ", daddr + i);
        size_t o;
        for (o = 0; (o < 16) && (i + o < dumpsize); o++) {
            printf("%02x ", buf[i + o]);
        }
        for (; o < 16; o++) {
            printf("   ");
        }
        printf(" |");
        for (o = 0; (o < 16) && (i + o < dumpsize); o++) {
            if (isprint(buf[i + o])) {
                printf("%c", buf[i + o]);
            } else {
                printf(".");
            }
        }
        printf("|\n");
    }
    ctx->prev_dump_addr = daddr + dumpsize;

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
        debugf("\"%s\" is not a valid register name.\n", reg);
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
        printf("%3u: %lx\n", cur->id, cur->addr);
        cur = cur->next;
    }

    return 0;
}

int dbg_cmd_load(dbg_ctx *ctx, const char *path)
{
    if (ctx->state != DBG_STATE_INIT) {
        debugf("The debugger is already loaded.\n");
        errno = EINVAL;
        return -1;
    }

    unsigned char elfbuf[sizeof(Elf64_Ehdr)];
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        errorf("Failed to open %s: %s\n", path, strerror(errno));
        return -1;
    }

    size_t nread = fread(elfbuf, sizeof(Elf64_Ehdr), 1, fp);
    fclose(fp);
    if (nread != 1) {
        debugf("Failed to read ELF header.\n");
        errno = EIO;
        return -1;
    }

    Elf64_Ehdr *elfhdr = (Elf64_Ehdr *)elfbuf;
    if (!(elfhdr->e_ident[EI_MAG0] == ELFMAG0 &&
          elfhdr->e_ident[EI_MAG1] == ELFMAG1 &&
          elfhdr->e_ident[EI_MAG2] == ELFMAG2 &&
          elfhdr->e_ident[EI_MAG3] == ELFMAG3 &&
          elfhdr->e_ident[EI_CLASS] == ELFCLASS64 &&
          elfhdr->e_machine == EM_X86_64)) {
        debugf("Invalid x86_64 ELF file.\n");
        errno = ENOEXEC;
        return -1;
    }

    ctx->loaded_program = strdup(path);
    if (ctx->loaded_program == NULL) {
        errorf("Failed to strdup().\n");
        return -1;
    }
    ctx->start_address = elfhdr->e_entry;
    ctx->state = DBG_STATE_LOADED;
    debugf("program '%s' loaded. entry point 0x%lx\n", path, elfhdr->e_entry);

    return 0;
}

static int dbg_run(dbg_ctx *ctx, const char *argv[])
{
    if (ctx->state == DBG_STATE_RUNNING) {
        debugf("pid %u is already running, killing the old process...\n", ctx->pid);
        kill(ctx->pid, SIGKILL);
        dbg_wait(ctx);
    }

    const char *actual_argv[MAX_ARGV + 2];
    actual_argv[0] = ctx->loaded_program;
    memcpy(&actual_argv[1], argv, sizeof(char *) * MAX_ARGV);
    actual_argv[MAX_ARGV + 1] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // Child
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        personality(ADDR_NO_RANDOMIZE);
        execv(ctx->loaded_program, actual_argv);
        errorf("Failed to execve(): %s\n", strerror(errno));
        abort();
    } else if (pid > 0) {
        // Parent
        ctx->pid = pid;
        dbg_wait(ctx);
        if (ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL) < 0) {
            return -1;
        }
        ctx->state = DBG_STATE_RUNNING;
    } else {
        errorf("Failed to fork().\n");
        return -1;
    }

    return 0;
}

int dbg_cmd_run(dbg_ctx *ctx, const char *argv[])
{
    if (ctx->state == DBG_STATE_INIT) {
        debugf("The debugger is not loaded.\n");
        errno = EINVAL;
        return -1;
    }
    if (ctx->state == DBG_STATE_RUNNING) {
        debugf("program '%s' is already running.\n", ctx->loaded_program);
        return dbg_cmd_cont(ctx);
    }

    if (dbg_run(ctx, argv) < 0) {
        errorf("Failed to run the program.\n");
        return -1;
    }
    debugf("pid %u\n", ctx->pid);

    return dbg_cmd_cont(ctx);
}

int dbg_cmd_vmmap(dbg_ctx *ctx)
{
    if (ctx->state != DBG_STATE_RUNNING) {
        debugf("The debugger is not running.\n");
        errno = EINVAL;
        return -1;
    }

    if (ctx->vmmaps == NULL && dbg_vmmap_reload(ctx) < 0) {
        errorf("Failed to read vmmap.\n");
        return -1;
    }

    for (size_t i = 0; i < ctx->vmmaps_len; i++) {
        printf("%016lx-%016lx ", ctx->vmmaps[i].start, ctx->vmmaps[i].end);
        if (ctx->vmmaps[i].perm & 0x04) {
            putchar('r');
        } else {
            putchar('-');
        }
        if (ctx->vmmaps[i].perm & 0x02) {
            putchar('w');
        } else {
            putchar('-');
        }
        if (ctx->vmmaps[i].perm & 0x01) {
            putchar('x');
        } else {
            putchar('-');
        }
        printf(" %-8lx %s", ctx->vmmaps[i].offset, ctx->vmmaps[i].name);
    }

    return 0;
}

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

int dbg_cmd_start(dbg_ctx *ctx, const char *argv[])
{
    if (ctx->state == DBG_STATE_INIT) {
        debugf("The debugger is not loaded.\n");
        errno = EINVAL;
        return -1;
    }

    if (dbg_run(ctx, argv) < 0) {
        errorf("Failed to run the program.\n");
        return -1;
    }
    debugf("pid %u\n", ctx->pid);

    return 0;
}


