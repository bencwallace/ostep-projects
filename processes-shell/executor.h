#ifndef WISH_EXECUTOR_H
#define WISH_EXECUTOR_H

#include <stdbool.h>

#include "types.h"

bool exec_cd(Token *);

void exec_path(PathNode *, PathNode *);

void exec_exec(ExecNode *, PathNode *);

int exec_command(CommandNode *, PathNode *);

int exec_parallel(ParallelNode *, PathNode *);

#endif
