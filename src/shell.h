#ifndef SHELL_H
#define SHELL_H

#define MAX_LINE       1024   /* max length of a raw input line          */
#define MAX_ARGS       64     /* max number of tokens in one command     */
#define MAX_PIPE_CMDS  16     /* max number of commands chained by pipes */

/*
 * A single command, e.g. "grep -i foo < in.txt >> out.txt"
 * argv is a NULL-terminated array suitable for execvp().
 */
typedef struct {
    char *argv[MAX_ARGS];
    char *input_file;    /* set by  <   , NULL if none */
    char *output_file;   /* set by  >   , NULL if none */
    int   append_output; /* 1 if redirection was  >>   */
} command_t;

/*
 * A full pipeline, e.g. "cmd1 | cmd2 | cmd3"
 */
typedef struct {
    command_t cmds[MAX_PIPE_CMDS];
    int       count;
} pipeline_t;

/* parser.c */
int  parse_line(char *line, pipeline_t *pipeline);
void free_pipeline(pipeline_t *pipeline);

/* executor.c */
int  execute_pipeline(pipeline_t *pipeline);

/* builtins.c */
int  is_builtin(const char *cmd);
int  run_builtin(char **argv);

#endif /* SHELL_H */
