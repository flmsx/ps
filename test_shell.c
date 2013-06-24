#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "shell.h"

SHELL_MK_CMD(add) {printf("%d\n",args[0].i+args[1].i);}
SHELL_MK_CMD(mul) {printf("%d\n",args[0].i*args[1].i);}
SHELL_MK_CMD(sqrt) {printf("%f\n",sqrt(args[0].i));}

shell_cmd_t dsp_table[3] ={
    SHELL_CMD(add,"ii","Add two numbers"),
    SHELL_CMD(mul,"ii","Multiply two numbers"),
    SHELL_CMD(sqrt,"i","Take the square root of number")};


int main()
{
    shell_t *shell;
    shell = shell_new();
    shell_register_cmd(dsp_table[0], shell);
    shell_register_cmd(dsp_table[1], shell);
    shell_register_cmd(dsp_table[2], shell);
    shell_run(shell);
    shell_destroy(shell);
    return 2;
}
