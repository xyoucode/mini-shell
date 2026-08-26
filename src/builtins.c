#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "shell.h"

static const char *BUILTIN_NAMES[] = { "cd", "pwd", "exit", "help", NULL };

int is_builtin(const char *cmd)
{
    for (int i = 0; BUILTIN_NAMES[i] != NULL; i++) {
        if (strcmp(cmd, BUILTIN_NAMES[i]) == 0) return 1;
    }
    return 0;
}

static int builtin_cd(char **argv)
{
    const char *target = argv[1];

    if (target == NULL) {
        target = getenv("HOME");
        if (target == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    if (chdir(target) != 0) {
        fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
        return 1;
    }
    return 0;
}

static int builtin_pwd(char **argv)
{
    (void)argv;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", cwd);
    return 0;
}

static int builtin_help(char **argv)
{
    (void)argv;
    printf("mini-shell -- built-in commands:\n");
    printf("  cd [dir]     change directory (default: $HOME)\n");
    printf("  pwd          print working directory\n");
    printf("  exit [code]  exit the shell\n");
    printf("  help         show this message\n");
    printf("Supports: pipes ( | ), redirection ( < > >> ), any external program in $PATH.\n");
    return 0;
}


int run_builtin(char **argv)
{
    if (strcmp(argv[0], "cd") == 0)   return builtin_cd(argv);
    if (strcmp(argv[0], "pwd") == 0)  return builtin_pwd(argv);
    if (strcmp(argv[0], "help") == 0) return builtin_help(argv);

    if (strcmp(argv[0], "exit") == 0) {
        int code = 0;
        if (argv[1] != NULL) code = atoi(argv[1]);
        exit(code);
    }

    fprintf(stderr, "shell: unknown builtin '%s'\n", argv[0]);
    return 1;
}
