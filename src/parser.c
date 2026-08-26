#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

static int parse_command(char *segment, command_t *cmd)
{
    int argc = 0;
    cmd->input_file    = NULL;
    cmd->output_file   = NULL;
    cmd->append_output = 0;

    char *token = strtok(segment, " \t\r\n");
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (!token) { fprintf(stderr, "shell: syntax error near '<'\n"); return -1; }
            cmd->input_file = token;
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (!token) { fprintf(stderr, "shell: syntax error near '>>'\n"); return -1; }
            cmd->output_file   = token;
            cmd->append_output = 1;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (!token) { fprintf(stderr, "shell: syntax error near '>'\n"); return -1; }
            cmd->output_file   = token;
            cmd->append_output = 0;
        } else {
            if (argc >= MAX_ARGS - 1) {
                fprintf(stderr, "shell: too many arguments\n");
                return -1;
            }
            cmd->argv[argc++] = token;
        }
        token = strtok(NULL, " \t\r\n");
    }

    cmd->argv[argc] = NULL;
    return 0;
}


int parse_line(char *line, pipeline_t *pipeline)
{
    pipeline->count = 0;

    line[strcspn(line, "\n")] = '\0';

    char *probe = line;
    while (*probe == ' ' || *probe == '\t') probe++;
    if (*probe == '\0') return 0;

    char *saveptr = NULL;
    char *segment = strtok_r(line, "|", &saveptr);

    while (segment != NULL) {
        if (pipeline->count >= MAX_PIPE_CMDS) {
            fprintf(stderr, "shell: too many piped commands\n");
            return -1;
        }
        if (parse_command(segment, &pipeline->cmds[pipeline->count]) != 0) {
            return -1;
        }
    
        if (pipeline->cmds[pipeline->count].argv[0] == NULL) {
            fprintf(stderr, "shell: syntax error: empty command in pipeline\n");
            return -1;
        }
        pipeline->count++;
        segment = strtok_r(NULL, "|", &saveptr);
    }

    return 0;
}

void free_pipeline(pipeline_t *pipeline)
{
    (void)pipeline;
}
