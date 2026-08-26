#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include "shell.h"


static void install_signal_handlers(void)
{
    signal(SIGINT, SIG_IGN);
}

static void print_prompt(void)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("mini-shell:%s$ ", cwd);
    } else {
        printf("mini-shell$ ");
    }
    fflush(stdout);
}

int main(void)
{
    char line[MAX_LINE];
    pipeline_t pipeline;

    install_signal_handlers();

    int interactive = isatty(STDIN_FILENO);

    while (1) {
        if (interactive) print_prompt();

        if (fgets(line, sizeof(line), stdin) == NULL) {
            if (interactive) printf("\n");
            break; 
        }

        if (parse_line(line, &pipeline) != 0) {
            continue; 
        }

        if (pipeline.count == 0) {
            continue; 
        }

        execute_pipeline(&pipeline);
        free_pipeline(&pipeline);
    }

    return 0;
}
