#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

bool parse_empty(size_t len, Token **toks) {
    return len == 0 || toks[0]->tok_type == ampand_tok;
}

bool parse_exit(size_t len, Token **toks) {
    return len == 1 && toks[0]->tok_type == exit_tok;
}

CdNode *parse_cd(size_t len, Token **toks) {
    CdNode *cd_node = NULL;

    if (len >= 2 && toks[0]->tok_type == cd_tok) {
        cd_node = malloc(sizeof(CdNode));
        cd_node->path = toks[1];
    }

    return cd_node;
}

PathNode *parse_path(size_t len, Token **toks) {
    PathNode *path_node = NULL;

    if (len >= 1 && toks[0]->tok_type == path_tok) {
        path_node = malloc(sizeof(PathNode));
        path_node->n_paths = 0;
        path_node->paths = malloc((len - 1) * sizeof(char *));

        while (1 + path_node->n_paths < len && toks[path_node->n_paths + 1]->tok_type == ident_tok) {
            (path_node->paths)[path_node->n_paths] = toks[path_node->n_paths + 1];
            ++(path_node->n_paths);
        }
    }

    return path_node;
}

ExecNode *parse_exec(size_t len, Token **toks) {
    ExecNode *exec_node = NULL;

    if (len >= 1 &&
        toks[0]->tok_type == ident_tok &&
        toks[0]->len > 0
    ) {
        // initialize command
        exec_node = malloc(sizeof(ExecNode));
        exec_node->len_cmd = toks[0]->len;
        exec_node->cmd = toks[0];
        exec_node->n_args = 0;
        exec_node->args = malloc((len - 1) * sizeof(Token *));

        // parse arguments
        while (1 + exec_node->n_args < len && toks[exec_node->n_args + 1]->tok_type == ident_tok) {
            (exec_node->args)[exec_node->n_args] = toks[exec_node->n_args + 1];
            ++(exec_node->n_args);
        }

        // parse output redirection
        if (exec_node->n_args < len - 1 &&
            toks[exec_node->n_args + 1]->tok_type == rangle_tok
        ) {
            if (exec_node->n_args < len - 2 &&
                toks[exec_node->n_args + 2]->tok_type == ident_tok
            ) {
                exec_node->out = toks[exec_node->n_args + 2];
            } else {
                return NULL;
            }
        }
    }

    return exec_node;
}

CommandNode *parse_command(size_t len, Token **toks) {
    CommandNode *node = malloc(sizeof(CommandNode));
    if (!node) error();

    CdNode *cd_node;
    PathNode *path_node;
    ExecNode *exec_node;
    if (parse_empty(len, toks)) {
        node->node_type = empty_t;
    } else if (parse_exit(len, toks)) {
        node->node_type = exit_t;
        return node;
    } else if ((cd_node = parse_cd(len, toks))) {
        node->node_type = cd_t;
        node->cd_node = cd_node;
    } else if ((path_node = parse_path(len, toks))) {
        node->node_type = path_t;
        node->path_node = path_node;
    } else if ((exec_node = parse_exec(len, toks))) {
        node->node_type = exec_t;
        node->exec_node = exec_node;
    } else {
        free(node);
        return NULL;
    }

    return node;
}

ParallelNode *parse_parallel(size_t len, Token** toks) {
    ParallelNode *parallel_node = NULL;

    parallel_node = malloc(sizeof(ParallelNode));
    if ((parallel_node->left = parse_command(len, toks))) {
        int n_toks;
        switch (parallel_node->left->node_type) {
            case empty_t:
                n_toks = 0;
                break;
            case exit_t:
                n_toks = 1;
                break;
            case cd_t:
                Token *path = parallel_node->left->cd_node->path;
                n_toks = (path == NULL) ? 1 : 2;
                break;
            case path_t:
                n_toks = 1 + parallel_node->left->path_node->n_paths;
                break;
            case exec_t:
                ExecNode *exec_node = parallel_node->left->exec_node;
                n_toks = 1 + exec_node->n_args + (exec_node->out == NULL ? 0 : 2);
                break;
        }
        toks += n_toks;
        len -= n_toks;
    }
    if (len > 0) {
        if (toks[0]->tok_type != ampand_tok) {
            return NULL;
        }
        ++toks;
        --len;
        if (len > 1) {
            parallel_node->right = parse_parallel(len, toks);
        }
    }

    return parallel_node;
}
