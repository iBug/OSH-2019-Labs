## Source overview

All source codes are in `src/` directory. A `Makefile` is provided so the whole program can be easily compiled with `make`.

There are two optional features enabled by default: Colored prompt (similar to `$PS1`) and GNU readline utilization. To disable them, edit `src/Makefile`.

You can verify the functionality of ish by invoking `./ish < test.sh` and comparing the output against `bash test.sh`. They should be the same.

## The following features are implemented

Mandatory:

- Built-in commands `cd` and `pwd`
- Command piping with pipe(2) and dup2(2) (and pipe chaining)
- Basic redirections `<`, `>` and `>>`
  - Built-in commands support both piping and redirection
    - With piping, it still `fork`s for built-in commands. With only redirection, it doesn't
- Setting environment variables with the syntax `export VAR=VALUE` (implemented as a built-in command)

Optional:

- Built-in commands `exec`, `exit` and `unset` (and `:`, the shell no-op)
- Colon for running multiple commands on one line `echo abcd; date; uname -r`
- Setting local variables with the syntax `VAR=VALUE`
- Home directory expansion for current user (`echo ~` outputs `/home/ubuntu`, depends on `$HOME`)
- Basic (direct) variable expansion (`echo $USER:${HOME}MMM:$INVALID:` outputs `ubuntu:/home/ubuntuMMM::`)
- Backslash escape sequence (*some* of them: `echo \e[31;1m\a\\\x42\e[0m` will output `\a\B` in red - check the source code for details)
- Quoting with single or double quotes (`echo ~.abc"~.def$HOME\""\ '"$HOME"'` will output `/home/ubuntu.abc~.def/home/ubuntu" "$HOME"`)
- Extended redirection, namely `<<` and `<<<` heredocs
  - Escape sequence and variable expansion are supported for both types of redirections
  - Built-in commands are executed in the current shell if there's no pipe, even with redirections

    That means `exit > /dev/null` will still exit the shell, while `exit | cat` won't. This is consistent with observable behavior from other known shells like dash, ksh or bash.
- Extended variable substitution (GNU Bash)
  - `${PWD:2:4}` gives `ome/` (if your PWD is `/home/ubuntu`)
  - `${PWD:2}` gives `ome/ubuntu`
  - `${#PWD}` gives `12`
  - `${name=value}` sets the value of `$name` if it's previously unset, then give the value of `$name`
  - However, spaces in the substituted string will not separate arguments
- GNU readline support
  - Tab completion for filenames, understands `~` for home path
  - Command histories, though they don't preserve across sessions
  - More GNU readline features (`~/.inputrc` will be respected)

### Limitations on this implementation

A minimal amount of dynamic memory arrangement is employed in this project, so a lot of things can't go infinitely, when on a standard shell it should, provided there's enough system memory.

All these limitations are hard-coded and is only changeable by editing the code.

- There's a length limit on each input line
- There's a length limit on the name of non-environment variables
- There's a length limit on the value of non-environment variables
- There's a length limit on the full path to the current working directory (or it will display truncated)

## The following features are not implemented, or implemented in a way that may not be compliant to the POSIX standard

- Variable expansion for everything else not listed above, including but not limited to positional arguments `$1`, `$2`, etc. and special variables `$$`, `$#`, `$_`, etc.
- File descriptor redirection like `2>&1` or `1>&-`
