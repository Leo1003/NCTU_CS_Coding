#include "register.h"

#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <sys/user.h>

const struct register_mapping reg_maps[] = {
    { "rax", offsetof(struct user_regs_struct, rax) },
    { "rbx", offsetof(struct user_regs_struct, rbx) },
    { "rcx", offsetof(struct user_regs_struct, rcx) },
    { "rdx", offsetof(struct user_regs_struct, rdx) },
    { "r8",  offsetof(struct user_regs_struct, r8) },
    { "r9",  offsetof(struct user_regs_struct, r9) },
    { "r10", offsetof(struct user_regs_struct, r10) },
    { "r11", offsetof(struct user_regs_struct, r11) },
    { "r12", offsetof(struct user_regs_struct, r12) },
    { "r13", offsetof(struct user_regs_struct, r13) },
    { "r14", offsetof(struct user_regs_struct, r14) },
    { "r15", offsetof(struct user_regs_struct, r15) },
    { "rdi", offsetof(struct user_regs_struct, rdi) },
    { "rsi", offsetof(struct user_regs_struct, rsi) },
    { "rbp", offsetof(struct user_regs_struct, rbp) },
    { "rsp", offsetof(struct user_regs_struct, rsp) },
    { "rip", offsetof(struct user_regs_struct, rip) },
    { "flags", offsetof(struct user_regs_struct, eflags) },
    { NULL, 0 },
};

const struct register_mapping *register_lookup(const char *name)
{
    for (size_t i = 0; reg_maps[i].name != NULL; i++) {
        if (strcasecmp(name, reg_maps[i].name) == 0) {
            return &reg_maps[i];
        }
    }

    return NULL;
}

