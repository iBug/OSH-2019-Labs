### TL;DR - The following features are implemented

Mandatory:

- Built-in commands `cd` and `pwd`
- Command piping with pipe(2) and dup2(2) (and pipe chaining)
- Basic redirections `<`, `>` and `>>`
- Setting environment variables with the syntax `export VAR=VALUE` (implemented as a built-in command)

Optional:

- Built-in commands `exec`, `exit` and `unset`

- Colon for running multiple commands on one line `echo abcd; date; uname -r`
- Setting local variables with the syntax `VAR=VALUE`
- Home directory expansion for current user (`echo ~` outputs `/home/ubuntu`, depends on `$HOME`)
- Basic (direct) variable expansion (`echo $USER:${HOME}MMM:$INVALID:` outputs `ubuntu:/home/ubuntuMMM::`)
- Backslash escape sequence (*some* of them: `echo \e[31;1m\a\\\x42\e[0m` will output `\a\B` in red - check the source code for details)
- Quoting with single or double quotes (`echo ~.abc"~.def$HOME\""\ '"$HOME"'` will output `/home/ubuntu.abc~.def/home/ubuntu" "$HOME"`)
- GNU readline support
  - Tab completion for filenames, understands `~` for home path
  - Command histories, though they don't preserve across sessions
  - More GNU readline features (`~/.inputrc` will be respected)

## 0. Foreword

I started this project more than a week ago when the OS course (no H) by Yongkun Li announced its second lab, the part 2 of which was exactly a subset of this lab.

Their lab 2 part 2 required us to implement a shell with support for multiple commands on the same line (the colon `;` as separator) and piping between exactly two processes (the vertical bar `|` as indicator). One of the most ridiculous thing found in their lab was that their TAs listed several "system calls", *including* system(3) and popen(3) (and that's why we were required to implement piping between only two processes and no more - popen(3) can't handle more).

Incredibility aside, I finished that lab with only an hour and a half, and as a result I have my own code to start off with even before this lab was published. With the same reason, I'll separate this report by "OS Lab" and "OSH Additions".

## 1. Basic command parsing

Because this project does not aim to create a fully POSIX-compliant shell, which would be too much work due to the necessity of a decent implementation of lexical parsing supporting all the shell syntaxes (`if then elif else fi`, `while do done`, `for in do done`, `case in *) ;; esac`), I decided that it'd be easy to go with a plain one-way string scanning method (i.e. looping over the string just once and stop whenever wanted. This is the code used to parse the command line:

```c
// Prepare prompt
fprintf(stderr, "OSLab2-> ");
fflush(stderr);

// Get input
s = fgets(cmd, MAX_LEN, stdin);
if (!s) {
    puts("exit");
    return 0;
}
cmdlen = strlen(cmd);
if (cmd[cmdlen - 1] == '\n') {
    cmd[--cmdlen] = 0;
}

// Split arguments
i = 0;
// Loop first in case of ';' to handle
while (i < cmdlen) {
    for (argcount = 0; i < cmdlen; argcount++) {
        // Skip all control stuff
        while (cmd[i] <= ' ' || cmd[i] == '\x7F') {
            cmd[i] = 0;
            i++;
        }

        // Make a pointer to all following non-control characters
        s = cmd + i;
        argv[argcount] = s;
        while (cmd[i] > ' ' && cmd[i] < '\x7F') {
            if (cmd[i] == ';') {
                cmd[i] = 0;
                break;
            }
            i++;
        }

        // Get ready to execute!
        if (cmd[i] == 0) {
            argcount++; // The last pointer ...
            break;
        }
    }
```
The loop at lines 22-25 should be able to handle the case of multiple whitespaces between arguments (and at the beginning of the read line), so `echo abc` would behave identically as `echo  abc`.

The loop at lines 30-36 handles the case of colons, so that the loop starting at line 20 would break out upon seeing a colon, and the part of the command that's already been parsed can start executing.

```c
pid_t fork_pid = fork();
if (fork_pid) {
    // Parent
    int status, ecode;
    waitpid(fork_pid, &status, 0);
    ecode = WEXITSTATUS(status);
    DEBUG("Child exit code: %d\n", ecode);
    if (ecode == 233) {
        printf("OSLab2: %s: not found\n", argv[0]);
    }
}
else {
    // Child - Go execve
    int err = execvp(argv[0], argv);

    // Normally unreachable - something's wrong
    DEBUG("exec: %d\n", err);
    exit(233);
}
```

That's almost all of the initial version of this OS Lab 2 shell. It stops at every colon and execute the command parsed so far.

Then, before implementing pipes, some built-in commands would be handy. This is how built-in commands are implemented - by performing a special check before forking and exec-ing.

```diff
--- a/shell.c
+++ b/shell.c
@@ -6,6 +6,7 @@
 #include <string.h>
 
 #include <unistd.h>
+#include <errno.h>
 #include <sys/wait.h>
 
 #define MAX_LEN 256
@@ -26,6 +27,8 @@ char arg[MAX_ARGS][MAX_ARG_LEN];
 char *argv[MAX_ARGS + 1];
 int cmdlen, argcount;
 
+int process_builtin(int, char const * const * args);
+
 int main(int _argc, char** _argv, char** _envp) {
     char *s;
     int i, j;
@@ -82,6 +85,10 @@ int main(int _argc, char** _argv, char** _envp) {
             DEBUG("argcount = %d\n", argcount);
             DEBUG("$0 = %s\n", argv[0]);
 
+            // Check for builtin commands
+            if (process_builtin(argcount, argv))
+                continue;
+
             // Execute the command
             pid_t fork_pid = fork();
 
@@ -100,10 +107,36 @@ int main(int _argc, char** _argv, char** _envp) {
                 int err = execvp(argv[0], argv);
 
                 // Normally unreachable - something's wrong
-                DEBUG("exec: %d\n", err);
+                DEBUG("exec(3): %d\n", err);
                 exit(233);
             }
         }
     }
     return 0;
 }
+
+int process_builtin(int argc, char const * const * args) {
+    const char *cmd = args[0];
+    if (!strlen(cmd)) {
+        return 0; // wat?
+    }
+    else if (!strcmp(cmd, "cd")) {
+        const char *target;
+        if (argc < 2) {
+            target = getenv("HOME");
+        } else {
+            target = args[1];
+        }
+        int result = chdir(target);
+        if (result) {
+            fprintf(stderr, "cd: %s\n", strerror(errno));
+        }
+    }
+    else if (!strcmp(cmd, "exit")) {
+        exit(0);
+    }
+    else {
+        return 0; // Not a built-in
+    }
+    return 1; // True - this is a built-in
+}
```

With the template of `process_builtin` function, more builtin commands can be easily added, which will be talked about later.

