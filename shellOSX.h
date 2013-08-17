#ifndef _SHELL_H
#define _SHELL_H

//Type definitions
typedef union {
    char  *s;
    int    i;
} shell_arg_t;

typedef struct s_shell_t {
    int cmds_cnt;
    int cmds_cap;
    struct s_shell_cmd_t *dsp_table;
	void *data;
} shell_t;

typedef struct s_shell_cmd_t {
    const char* name;
    const char* short_name;
    void (*func)(shell_arg_t*, struct s_shell_t*);
    const char* args;
    const char* doc;
} shell_cmd_t;

#define SHELL_MK_CMD(x) void shell_cmd_ ## x (shell_arg_t *args, shell_t *shell)

//The dispatch table
#define SHELL_CMD(func, shortn, params, help) {#func, shortn, shell_cmd_ ## func, params, help}

shell_t* shell_new(void *context);

void shell_destroy(shell_t *shell);

void shell_register_cmd(shell_cmd_t cmd, shell_t *shell);

int shell_run(shell_t *shell);

#endif /* _SHELL_H */
