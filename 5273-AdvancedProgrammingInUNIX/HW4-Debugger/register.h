#ifndef __HW4_REGISTER_H
#define __HW4_REGISTER_H

#include <sys/types.h>

struct register_mapping {
    const char *name;
    size_t offset;
};

extern const struct register_mapping reg_maps[];

const struct register_mapping *register_lookup(const char *name);

#endif

