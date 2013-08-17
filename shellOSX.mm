#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "shellOSX.h"

/* Built-in cmds */
static SHELL_MK_CMD(exit)
{
	exit(0);
}

static SHELL_MK_CMD(help)
{
    puts("Available Commands:");
    int i=shell->cmds_cnt;
    while(i--) {
        shell_cmd_t cmd=shell->dsp_table[i];
        char tmp[100];//Formatting buffer
        snprintf(tmp,100,"%s(%s)",cmd.name,cmd.args);
        printf("%10s\t- %s\n",tmp,cmd.doc);
    }
}

static shell_cmd_t builtin_cmd_exit = SHELL_CMD(exit,"e","","Exits the interpreter");
static shell_cmd_t builtin_cmd_help = SHELL_CMD(help,"h","","Display this help");

shell_t* shell_new(void *context)
{
	shell_t *shell = (shell_t*)malloc(sizeof(shell_t));
	assert(shell);
	shell->cmds_cnt = 0;
	shell->cmds_cap = 10;
	shell->dsp_table = (shell_cmd_t*)calloc(shell->cmds_cap, sizeof(shell_cmd_t));
	assert(shell->dsp_table);
	shell_register_cmd(builtin_cmd_help, shell);
	shell_register_cmd(builtin_cmd_exit, shell);
	shell->data = context;
	return shell;
}

void shell_destroy(shell_t *shell)
{
	free(shell->dsp_table);
	free(shell);
}

void shell_register_cmd(shell_cmd_t cmd, shell_t *shell)
{
	if (++shell->cmds_cnt > shell->cmds_cap) {
		shell->cmds_cap += 5;
		shell->dsp_table = (shell_cmd_t*)realloc(shell->dsp_table, shell->cmds_cap * sizeof(shell_cmd_t));
		assert(shell->dsp_table);
	}
	shell->dsp_table[shell->cmds_cnt-1] = cmd;
}

static shell_arg_t *args_parse(const char *s);

static const char *delim = " \n(,);";

static char pre_cmd[20] = {0,};
static void parse(char *cmd, shell_t *shell)
{
    const char* tok = strtok(cmd,delim);
    if(!tok) {
		if (strlen(pre_cmd))
			tok = pre_cmd;
		else
        	return;
	}

    int i=shell->cmds_cnt;
    while(i--) {
        shell_cmd_t cur = shell->dsp_table[i];
        if(!strcmp(tok,cur.name) || !strcmp(tok,cur.short_name)) {
            shell_arg_t *args = args_parse(cur.args);
            if(args==NULL && strlen(cur.args))
                return;//Error in argument parsing
            cur.func(args, shell);
            free(args);
			strcpy(pre_cmd, tok);
            return;
        }
    }

    puts("Command Not Found");
}

#define ESCAPE {free(args); puts("Bad Argument(s)"); return NULL;}
static shell_arg_t *args_parse(const char *s)
{
    int argc=strlen(s);
    shell_arg_t *args=(shell_arg_t*)malloc(sizeof(shell_arg_t)*argc);
    int i;
    for(i=0;i<argc;++i) {
        char *tok;
        switch(s[i]) {
            case 's':
                args[i].s = strtok(NULL,delim);
                if(!args[i].s)
                    ESCAPE;
                break;
            case 'i':
                tok = strtok(NULL,delim);
                if(!tok)
                    ESCAPE;
                args[i].i = atoi(tok);
                //printf("arg %d is %d\n", i, args[i].i);
                //if(!islower(args[i].c))
                //    ESCAPE;
                break;
        }
    }
    return args;
}
#undef ESCAPE

//Global data
static const char *prompt = ">";

int shell_run(shell_t *shell)
{
    //Read Parse Exec Loop
    char cmd[200];
    while(1) {
        printf("%s ",prompt);
        fflush(stdout);

        parse(fgets(cmd,200,stdin), shell);
    }

    return 2;
}
