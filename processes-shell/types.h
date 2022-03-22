#ifndef WISH_TYPES_H
#define WISH_TYPES_H

#include <sys/types.h>

enum NodeType { empty_t, exit_t, cd_t, path_t, exec_t };

enum TokType { sym_t, str_t };

typedef struct Token {
    int tok_type;
    size_t len;
    char *val;
} Token;

typedef struct CdNode {
    Token *path;
} CdNode;

typedef struct PathNode {
    int n_paths;
    Token **paths;
} PathNode;

typedef struct ExecNode {
    int len_cmd;
    Token *cmd;
    int n_args;
    Token **args;
    Token *out;
} ExecNode;

typedef struct Node {
    int node_type;
    union {
        CdNode *cd_node;
        PathNode *path_node;
        ExecNode *exec_node;
    };
} Node;

#endif
