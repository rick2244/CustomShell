Author: Richard Dzreke

Custom Shell

Shell:

The outermost layer of the operating system is called the shell. In Unix-based systems, the shell is generally a command-line interface. 
Most Linux distributions ship with bash as the default (there are several others: csh, ksh, sh, tcsh, zsh). 

- Runs commands with exec, and if the commands are not supported, it prints out an error message
      Example of shell running:
        [0]-[1]─[/]$ ./hello
        Hello world!
        [0]-[2]─[/]$ ls /dir
        (directory contents are shown by ls)
        [0]-[3]─[/]$ echo hello there!
        hello there!
        [0]-[4]─[/]$ cd /this/does/not/exist
        chdir: no such file or directory: /this/does/not/exist
        [1]-[5]─[/]$ cd /this/does/exist
        [0]-[6]─[/this/does/exist]$
- Prompt: shell prompt displays some helpful information, such as: [0]-[6]─[/this/does/exist]$
      - Exit status of last command
      - Command number (begins at 1)
      - The current working directory
  
Scripting Support:
- supports scripting mode which is triggered when argv[1] is a file
- When argv[1] is a file commands are read from it instead of from stdin
    Example of Shell Scripting: 
    $ cat test.sh
    ls /
    echo hi
    mkdir d
    cd d
    ls
    /ls
    /echo bye
    exit
    $ crash test.sh
    (ls / contents appear)
    hi
    exec ls failed
    .              1 35 32
    ..             1 1 1024
    bye
  
Executable Scripts:
- Supports scripts that begin with shebang (#!)
  Example:
  $ cat test.sh
  #!/crash
  ls /
  echo hi
  mkdir d
  cd d
  ls
  /ls
  /echo bye
  exit
  $ ./test.sh # notice how we didn't have to run 'crash' here
  (ls / contents appear)
  hi
  exec ls failed
 .              1 35 32
 ..             1 1 1024
 bye

Built-in Commands: 
- Shell supports these built-in commands:
  cd to change the CWD.
  # (comments): strings prefixed with # will be ignored by the shell
  history, which prints the last 100 commands entered with their command numbers
  ! (history execution): entering !39 will re-run command number 39, and
  !! re-runs the last command that was entered.
  !ls re-runs the last command that starts with ‘ls.’ Note that command numbers are NOT the same as the array positions; e.g., you may have 100 history elements, with command numbers 600 – 699.
  exit to exit the shell.

- Example of History Command:
  [0]-[142]─[/]$ history
  42 ls
  43 cat long-text.txt
  44 echo "hi" # This prints out 'hi'

  ... (commands removed for brevity) ...

  140 ls /bin
  141 gcc -g crash.c
- History -t (Shows the amount of time it takes to complete a command):
  [0]-[142]─[/]$ history -t
  [42|68ms] ls
  [43|982ms] cat long-text.txt
  [44|13ms] echo "hi" # This prints out 'hi'
  ...
  [140|32ms] ls /bin
  [141|1ms] gcc -g crash.c
  
IO Redirection:
- Shell supports input/output redirection and pipes
   Example Execution:
   [0]-[142]─[/]$ echo "hello world!" > my_file.txt
   [0]-[143]─[/]$ cat my_file.txt
   hello world! 
   # Append text with '>>':
   [0]-[144]─[/]$ echo "hello world!" >> my_file.txt
   [0]-[145]─[/]$ cat my_file.txt
   hello world!
   hello world! 
   # Pipe:
   [0]-[146]─[/]$ cat other_file.txt | sort
   (sorted contents shown) 
   [0]-[147]─[/]$ seq 100000 | wc -l
   10000 
   [0]-[148]─[/]$ cat /etc/passwd | sort > sorted_pwd.txt
   (sorted contents written to 'sorted_pwd.txt') 
   # This is equivalent, but uses input redirection instead of cat:
   [0]-[149]─[/]$ sort < /etc/passwd > sorted_pwd.txt 
   # This is equivalent as well. Order of < and > don't matter:
   [0]-[150]─[/]$ sort > sorted_pwd.txt < /etc/passwd 
   # Here's input redirection by itself (not redirecting to a file):
   [0]-[151]─[/]$ sort < sorted_pwd.txt
   (sorted contents shown)
  
Background Jobs:
- If a command ends in &, then the command is run in the background

Path Execution: 
- shell supports path execution, which searches for paths or a set of paths for binaries. This allows for the execution of commands outside of the current directory
- a new function, execvp(pathname, args) is implemented to search for binaries based on priority.
  Priority List:
  1. Absolute or relative paths are run first. E.g., ../ls runs the ls binary in the directory one level above, /cat runs cat from the root directory, etc.
  2. If an absolute/relative path wasn’t given, and the user simply typed a program’s name, then search the / (root) directory for it by default.
  3. If the previous step failed (the program doesn’t exist in /), then attempt to execute it from the current directory instead.

Signature Feature:
- The shell's signature feature is allowing multiple commands to be run
  [0]-[150]─[/]$ echo "hello world" > my_file.txt \
  echo "goodbye world" >> my_file.txt
  [0]-[151]─[/]$ cat my_file.txt
  hello world
  goodbye world
  
  
  
