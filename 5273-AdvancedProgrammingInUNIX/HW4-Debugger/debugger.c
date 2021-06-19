#include "debugger.h"

#include <stdio.h>
#include <string.h>

void dbg_ctx_init(dbg_ctx *ctx)
{
    memset(ctx, 0, sizeof(dbg_ctx));
    ctx->state = DBG_STATE_INIT;
}



