# mini-shell

A small Unix-like command shell written from scratch in C, using only
POSIX system calls (`fork`, `execvp`, `pipe`, `dup2`, `waitpid`). Built to
understand how a real shell works under the hood: process creation,
inter-process communication via pipes, file descriptor manipulation, and
signal handling.

## Features

- **Built-in commands:** `cd`, `pwd`, `exit`, `help`
- **External commands:** anything available in `$PATH` (`ls`, `grep`,
  `wc`, ...), run via `fork` + `execvp`
- **Pipes:** chain any number of commands, e.g.
  `cat file.txt | grep foo | wc -l`
- **I/O redirection:** `<` (input), `>` (output, truncate), `>>` (output,
  append) — can be combined with pipes, e.g.
  `sort < names.txt > sorted.txt`
- **Signal handling:** `Ctrl-C` interrupts the running command, not the
  shell itself
- **Live prompt** showing the current working directory

## Build & run

```bash
make
./mini-shell
```

or in one step:

```bash
make run
```

## Example session

```
mini-shell:/home/user$ ls | grep .c | wc -l
4
mini-shell:/home/user$ echo hello > out.txt
mini-shell:/home/user$ cat < out.txt
hello
mini-shell:/home/user$ cd projects
mini-shell:/home/user/projects$ pwd
/home/user/projects
mini-shell:/home/user/projects$ exit
```

## Project structure

```
mini-shell/
├── src/
│   ├── main.c        # REPL loop: read a line, parse it, execute it
│   ├── parser.c       # Splits a line into a pipeline of commands + redirection
│   ├── executor.c      # fork/exec, pipe wiring, redirection, waiting on children
│   ├── builtins.c    # cd, pwd, exit, help
│   └── shell.h        # Shared types and function declarations
├── Makefile
└── README.md
```

## How it works

1. **Parsing** (`parser.c`): the raw input line is split on `|` into
   individual command segments. Each segment is tokenized on whitespace;
   tokens `<`, `>`, `>>` are pulled out as redirection instead of being
   treated as arguments.
2. **Execution** (`executor.c`): for a pipeline of *N* commands, *N-1*
   pipes are created up front. Each command is run in its own forked
   child, with `dup2` wiring its stdin/stdout to the correct pipe ends
   (and/or files, for redirection) before `execvp` replaces the child's
   image. The parent closes all pipe file descriptors and waits for every
   child to finish, returning the exit status of the last command in the
   chain — matching standard shell semantics.
3. **Builtins** (`builtins.c`): commands like `cd` change the *calling*
   process's state, so they only make sense running directly in the
   shell — not in a forked child, whose changes would vanish when it
   exits. A single, non-redirected, non-piped builtin therefore runs
   in-process; a builtin used inside a pipeline (e.g. `pwd | cat`) still
   runs in a child like any other command, since piping to/from it
   requires separate file descriptors anyway.

## Possible extensions

- Background jobs with `&` and a `jobs` builtin
- Environment variable expansion (`$HOME`, `$PATH`, ...)
- Command history and line editing (readline / libedit)
- Wildcard (glob) expansion
- Quoting support (`"a string with spaces"`)

## License

MIT — see [LICENSE](LICENSE).

## 👤 Author

Built by ' MESLOUB YOUCEF ABD ELMOUIZ '
