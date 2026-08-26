#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include "shell.h"

static int apply_redirection(const command_t *cmd)
{
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "shell: %s: %s\n", cmd->input_file, strerror(errno));
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT | (cmd->append_output ? O_APPEND : O_TRUNC);
        int fd = open(cmd->output_file, flags, 0644);
        if (fd < 0) {
            fprintf(stderr, "shell: %s: %s\n", cmd->output_file, strerror(errno));
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    return 0;
}


int execute_pipeline(pipeline_t *pipeline)
{
    int n = pipeline->count;
    if (n == 0) return 0;

    if (n == 1 && is_builtin(pipeline->cmds[0].argv[0]) &&
        pipeline->cmds[0].input_file == NULL && pipeline->cmds[0].output_file == NULL) {
        int status = run_builtin(pipeline->cmds[0].argv);
        fflush(stdout);
        return status;
    }

    int   pipes[MAX_PIPE_CMDS - 1][2];
    pid_t pids[MAX_PIPE_CMDS];

    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return 1;
        }
    }

   
    fflush(stdout);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            
            signal(SIGINT, SIG_DFL);

            if (i > 0)     dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < n - 1) dup2(pipes[i][1],     STDOUT_FILENO);

            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (apply_redirection(&pipeline->cmds[i]) != 0) {
                _exit(1);
            }

            if (is_builtin(pipeline->cmds[i].argv[0])) {
                _exit(run_builtin(pipeline->cmds[i].argv));
            }

            execvp(pipeline->cmds[i].argv[0], pipeline->cmds[i].argv);

           
            fprintf(stderr, "shell: %s: %s\n", pipeline->cmds[i].argv[0], strerror(errno));
            _exit(127);
        }

        pids[i] = pid;
    }


    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int status = 0, last_status = 0;
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], &status, 0);
        if (i == n - 1) {
            last_status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
        }
    }

    return last_status;
}
