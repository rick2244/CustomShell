#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define NBUILTINS 3
#define MAX_SIZE 100
#define MAX_PATH 128

/**
 * Calculates the length of the initial segment of `str` which consists entirely of characters in `chars`.
 * 
 * @param str A pointer to the null-terminated string to search.
 * @param chars A pointer to a null-terminated set of characters.
 * @return The length of the initial segment of `str` containing only characters in `chars`.
 */
uint strspn(const char *str, const char *chars) {
    uint i, j;
    for (i = 0; str[i] != '\0'; i++) {
        for (j = 0; chars[j] != str[i]; j++) {
            if (chars[j] == '\0') return i;
        }
    }
    return i;
}

/**
 * Finds the length of the initial segment of `str` which consists of characters not in `chars`.
 * 
 * @param str A pointer to the null-terminated string to search.
 * @param chars A pointer to a null-terminated set of characters.
 * @return The length of the initial segment of `str` containing no characters in `chars`.
 */
uint strcspn(const char *str, const char *chars) {
    const char *p, *sp;
    char c, sc;
    for (p = str;;) {
        c = *p++;
        sp = chars;
        do {
            if ((sc = *sp++) == c) return (p - 1 - str);
        } while (sc != 0);
    }
}

/**
 * Retrieves the next token in a string based on delimiters, updating the string pointer.
 * 
 * @param str_ptr Pointer to the string to be tokenized; it is updated as tokens are extracted.
 * @param delim A string containing the delimiter characters.
 * @return A pointer to the next token, or NULL if no token is found.
 */
char *next_token(char **str_ptr, const char *delim) {
    if (*str_ptr == NULL) return NULL;
    uint tok_start = strspn(*str_ptr, delim);
    uint tok_end = strcspn(*str_ptr + tok_start, delim);

    if (tok_end == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    char *current_ptr = *str_ptr + tok_start;
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        *str_ptr = NULL;
    } else {
        **str_ptr = '\0';
        (*str_ptr)++;
    }

    return current_ptr;
}


struct Node {
    const char* command;
    struct Node* next;
    struct Node* prev;
    int command_num;
    int runtime;
};

struct LinkedList {
    struct Node* head;
    struct Node* tail;
    int size;
};

/**
 * Duplicates a string by allocating memory for a copy of `src` and copying its contents.
 * 
 * @param src The source string to duplicate.
 * @return A pointer to the newly allocated copy of the string.
 */
char *strdup(const char *src) {
    char *dst = (char *)malloc(strlen(src) + 1);
    if (dst == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    strcpy(dst, src);
    return dst;
}

/**
 * Creates a new Node instance with the specified command and command number.
 * 
 * @param cmd The command string.
 * @param num The command number.
 * @return A pointer to the newly created Node.
 */
struct Node* createNode(char *cmd, int num) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->command = strdup(cmd);
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->command_num = num;
    newNode->runtime = 0;
    return newNode;
}

/**
 * Creates a doubly-linked list with dummy head and tail nodes.
 * 
 * @return A pointer to the newly created LinkedList.
 */
struct LinkedList* createList() {
    struct LinkedList* list = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    list->head = createNode(0, 0);
    list->tail = createNode(0, 0);
    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->size = 0;
    return list;
}

/**
 * Inserts a Node at the front of a LinkedList. If the list exceeds `MAX_SIZE`, removes the last Node.
 * 
 * @param list The LinkedList to insert into.
 * @param newNode The Node to insert.
 */
void insertAtFront(struct LinkedList* list, struct Node* newNode) {
    if (list->size == 0) {
        newNode->next = list->tail;
        newNode->prev = list->head;
        list->head->next = newNode;
        list->tail->prev = newNode;
    } else {
        newNode->next = list->head->next;
        newNode->prev = list->head;
        list->head->next->prev = newNode;
        list->head->next = newNode;
    }

    list->size++;
    if (list->size > MAX_SIZE) {
        struct Node* temp = list->tail->prev;
        list->tail->prev = temp->prev;
        temp->prev->next = list->tail;
        free(temp);
        list->size--;
    }
}

/**
 * Prints all nodes in the LinkedList, displaying either with or without runtime information based on `t`.
 * 
 * @param list The LinkedList to print.
 * @param t If 0, prints only command number and command; if 1, also includes runtime.
 */
void printList(struct LinkedList* list, int t) {
    struct Node* current = list->head->next;
    while (current != list->tail) {
        if (t == 0) {
            printf("[%d] %s", current->command_num, current->command);
        } else {
            printf("[%d][%d] %s", current->command_num, current->runtime, current->command);
        }
        current = current->next;
    }
}

/**
 * Retrieves a command from the linked list by command number.
 * 
 * @param list The LinkedList to search.
 * @param num The command number to search for. If `-1`, returns the most recent command.
 * @return A duplicated command string matching `num`, or NULL if not found.
 */
char *getCommand(struct LinkedList *list, int num) {
    struct Node *current = list->head->next;

    if (num == -1) {
        if (list->size == 0) {
            return NULL;
        }
        return strdup(current->command);
    }

    while (current != list->tail) {
        if (current->command_num == num) {
            return strdup(current->command);
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Checks if `prefix` is a prefix of `word`, ignoring leading/trailing spaces or newline characters.
 * 
 * @param word The word to check.
 * @param prefix The prefix to check for in `word`.
 * @return true if `prefix` is a prefix of `word`; otherwise, false.
 */
bool isPrefix(const char *word, const char *prefix) {
    while (*prefix) {
        if (*prefix != ' ' && *prefix != '\n' && *prefix != *word) {
            return false;
        }
        prefix++;
        word++;
    }
    return true; 
}

/**
 * Searches for and retrieves the most recent command from the LinkedList that matches the specified prefix.
 * 
 * @param list The LinkedList to search.
 * @param cmd The command string, with the prefix as the portion after the first character.
 * @return A duplicated command string matching the prefix, or NULL if not found.
 */
char *getCommandPrefix(struct LinkedList *list, char *cmd) {
    struct Node *current = list->head->next;
    char *prefix = cmd + 1;
    char first_word[128];

    int i = 0;
    while (prefix[i] != '\0' && prefix[i] != ' ') {
        first_word[i] = prefix[i];
        i++;
    }
    first_word[i] = '\0';

    while (current != list->tail) {
        char *other = strdup(current->command);
        if (isPrefix(other, first_word)) {
            free(other);
            return other;
        }
        free(other);
        current = current->next;
    }
    return NULL;
}

/**
 * Checks if a character is a digit (0-9).
 *
 * @param c The character to check.
 * @return 1 if `c` is a digit, 0 otherwise.
 */
int isDigit(char c) {
    return (c >= '0' && c <= '9') ? 1 : 0;
}

/**
 * Frees all nodes in a linked list, including the list itself.
 *
 * @param list Pointer to the LinkedList to free.
 */
void freeList(struct LinkedList *list) {
    struct Node *current = list->head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free((void *)temp->command);
        free(temp);
    }
    free(list);
}

/**
 * Built-in command function type definition. 
 * Used for built-in command execution with linked list context.
 */
typedef int (*builtin_func)(char *cmd, char *arg, struct LinkedList *list);

struct builtin {
    const char *name;
    builtin_func func;
};

/**
 * Exits the shell with a message.
 *
 * @param cmd The command name.
 * @param arg Arguments for the command.
 * @param list The LinkedList context.
 * @return 1 to signal an exit.
 */
int builtin_exit(char *cmd, char *arg, struct LinkedList *list) {
    printf("Aight, I'm gonna head out\n");
    return 1;
}

/**
 * Changes the current directory to the specified directory or the root directory if no argument is given.
 *
 * @param cmd The command name.
 * @param arg The directory to change to.
 * @param list The LinkedList context.
 * @return 2 on success; -1 if the directory does not exist.
 */
int builtin_cd(char *cmd, char *arg, struct LinkedList *list) {
    if (arg == NULL) {
        chdir("/");
    } else {
        if (chdir(arg) == -1) {
            fprintf(2, "chdir: no such file or directory: %s\n", cmd);
            return -1;
        }
    }
    return 2;
}

/**
 * Displays command history, optionally with runtime information.
 *
 * @param cmd The command name.
 * @param arg Argument to display with runtime information if "-t" is passed.
 * @param list The LinkedList containing command history.
 * @return 4 if history is shown; 3 if history is shown with runtime; -1 on error.
 */
int builtin_history(char *cmd, char *arg, struct LinkedList *list) {
    if (arg != 0) {
        if (strcmp(arg, "-t") == 0) {
            return 3;
        } else {
            fprintf(2, "history command failed\n");
            return -1;
        }
    }
    return 4;
}

/**
 * Retrieves a command from history by its number or prefix, based on the command input.
 *
 * @param cmd The input command, starting with '!' for history retrieval.
 * @param list The LinkedList containing command history.
 * @return A duplicated command string if found; otherwise, NULL if not found.
 */
char *builtin_bang(char *cmd, struct LinkedList *list) {
    if (cmd[0] == '!') {
        char *last = "";
        if (strcmp(cmd, "!!\n") == 0) {
            last = getCommand(list, -1);
        } else {
            int num = 0;
            int i = 1;
            bool prefix = false;
            
            while (cmd[i] != '\0' && cmd[i] != ' ' && cmd[i] != '\n') {
                if (isDigit(cmd[i])) {
                    num = num * 10 + ((cmd[i] - '0') % 10);
                } else {
                    prefix = true;
                    break;
                }
                i++;
            }

            last = (!prefix) ? getCommand(list, num) : getCommandPrefix(list, cmd);
        }

        if (last != NULL) {
            free(cmd);
            cmd = strdup(last);
        } else {
            printf("command doesn't exist\n");
        }
    }
    return cmd;
}

struct builtin builtins[NBUILTINS] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
    {"history", builtin_history}
};

/**
 * Executes a built-in function if `cmd` matches a built-in command.
 *
 * @param cmd The command name.
 * @param arg Arguments for the command.
 * @param list The LinkedList context.
 * @return The result of the built-in function if matched; 0 if no match.
 */
int execute_func(char *cmd, char *arg, struct LinkedList *list) {
    for (int i = 0; i < NBUILTINS; i++) {
        if (strcmp(cmd, builtins[i].name) == 0) {
            return builtins[i].func(cmd, arg, list);
        }
    }
    return 0;
}

struct command {
    char **tokens;
    bool stdout_pipe;
    bool stdout_append;
    char *stdout_file;
    char *stdin_file;
};

/**
 * Checks if a path contains a '/' character.
 *
 * @param path The path to check.
 * @return true if the path contains a '/', false otherwise.
 */
bool contains_slash(const char *path) {
    while (*path != '\0') {
        if (*path == '/') {
            return true;
        }
        path++;
    }
    return false;
}

/**
 * Concatenates `src` to the end of `dest`. Assumes `dest` has enough space.
 *
 * @param dest The destination string to which `src` will be concatenated.
 * @param src The source string to concatenate to `dest`.
 */
void str_concat(char *dest, const char *src) {
    while (*dest) {  
        dest++;
    }
    while (*src) { 
        *dest++ = *src++;
    }
    *dest = '\0';  
}

/**
 * Custom `execvp` function that searches for `pathname` in specific paths and executes it.
 *
 * @param pathname The file to execute.
 * @param argv The argument list for the command.
 * @return Result of the `exec` function if successful; -1 if not found.
 */
int execvp(const char *pathname, char *const argv[]) {
    char path[MAX_PATH];

    if (contains_slash(pathname)) {
        return exec(pathname, (char **) argv);
    }

    path[0] = '/';
    path[1] = '\0';
    str_concat(path, pathname);

    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        close(fd);
        return exec(path, (char **) argv);
    }

    path[0] = '.';
    path[1] = '/';
    path[2] = '\0';
    str_concat(path, pathname);

    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        close(fd);
        return exec(path, (char **) argv);
    }

    return -1; 
}

/**
 * Executes a command pipeline, setting up pipes and redirections as needed.
 *
 * @param cmd Pointer to the first command in a pipeline of commands.
 *            Each command may specify output redirection, input redirection, or piping.
 */
void execute_pipeline(struct command *cmd) {
    int fd[2];
    if (cmd->stdout_pipe) {
        if (pipe(fd) == -1) {
            fprintf(2, "Could not create pipe.\n");
            exit(1);
        }
    } else {
        if (cmd->stdout_file != NULL || cmd->stdin_file != NULL) {
            int open_flags = 0;
            int file_fd = 0;

            if (cmd->stdin_file != NULL) {
                open_flags = O_RDONLY;
                file_fd = open(cmd->stdin_file, open_flags);

                if (file_fd == -1) {
                    fprintf(2, "Could not open file\n");
                    exit(1);
                }
                close(0);

                if (dup(file_fd) == -1) {
                    fprintf(2, "dup failed\n");
                    exit(1);
                }
                close(file_fd);
            }

            if (cmd->stdout_file != NULL) {
                open_flags = (cmd->stdout_append) ? O_RDWR | O_CREATE | O_APPEND : O_RDWR | O_CREATE | O_TRUNC;
                file_fd = open(cmd->stdout_file, open_flags);

                if (file_fd == -1) {
                    fprintf(2, "Could not open file\n");
                    exit(1);
                }
                close(1);

                if (dup(file_fd) == -1) {
                    fprintf(2, "dup failed\n");
                    exit(1);
                }
                close(file_fd);
            }
        }

        execvp(cmd->tokens[0], cmd->tokens); 
        close(fd[0]);
        close(fd[1]);
        return;
    }

    int pid = fork();
    if (pid == -1) {
        fprintf(2, "Fork failed!\n");
        exit(1);
    } else if (pid == 0) {
        close(fd[0]);   
        close(1);       
        dup(fd[1]);     
        close(fd[1]);        

        execvp(cmd->tokens[0], cmd->tokens);
        exit(1);
    } else {
        close(fd[1]);      
        close(0);           
        dup(fd[0]);          
        close(fd[0]);        
        execute_pipeline(cmd + 1);
    }
}

/**
 * Reads a multi-line command from a file descriptor until a line does not end with '\'.
 * Joins lines with spaces when lines end with a backslash for improved readability.
 *
 * @param fd The file descriptor to read from.
 * @param cmd A pointer to the command string to store the concatenated result.
 *            Allocates memory for `*cmd` and adjusts as needed to fit the input.
 */
void multi_line_command(int fd, char **cmd) {
    uint size = 128;
    *cmd = malloc(size);

    (*cmd)[0] = '\0';
    int curr_len = 0;
    char *input = NULL;
    uint input_size = 0;
    int len = 0;

    while ((len = getline(&input, &input_size, fd)) > 0) {
        if (len > 1 && input[len - 2] == '\\') {
            input[len - 2] = ' ';
            input[len - 1] = '\0';

            while (curr_len + len > size) {
                int new_size = size * 2;
                char *tmp = malloc(new_size);

                memcpy(tmp, *cmd, curr_len);
                free(*cmd);
                *cmd = tmp;
                size = new_size;
            }

            str_concat(*cmd, input);
            curr_len += len - 1;
        } else {
            str_concat(*cmd, input);
            curr_len += len;
            break;
        }
    }

    if (curr_len == 0) {
        free(*cmd);
        *cmd = NULL;
    }
    free(input);
}

/**
 * The main function for a custom shell that parses and executes commands with support for:
 * - Multi-line input
 * - Built-in commands (e.g., `exit`, `cd`, `history`)
 * - Command pipelines and I/O redirection
 * - Background execution of jobs
 * 
 * @param argc The argument count from the command line.
 * @param argv The argument vector containing a potential script file.
 * @return Exit status of the shell.
 */
int main(int argc, char *argv[]) {
    printf("Welcome to my shell🐚\n\n");
    int command_num = 1;
    int status = 0;
    uint64 start, end;
    int fd = open(argv[1], O_RDONLY);
    bool isScript = (fd > 0);
    struct LinkedList *list = createList();

    if (!isScript) {
        fd = 0;
    }

    while (true) {
        char dir[128];
        getcwd(dir, 128);

        if (!isScript) {
            printf("[%d]-[%d]-[%s]$ ", status, command_num, dir);
        }

        char *cmd = NULL;
        multi_line_command(fd, &cmd);

        if (cmd == NULL) {
            printf("Goodbye 🥲\n");
            break;
        }

        if (strcmp(cmd, "\n") == 0 || cmd[0] == '#') {
            continue;
        }

        start = utime();
        struct Node *myNode = createNode(cmd, command_num);

        if (cmd[0] == '!') {
            cmd = builtin_bang(cmd, list);
        }
        command_num++;

        char *tokens[128];
        int token_cnt = 0;
        char *next_tok = cmd;
        char *curr_tok;

        struct command *cmds = malloc(128 * sizeof(*cmd));
        int cnt = 0;
        bool re_out = false;
        bool re_in = false;
        bool pipe = false;

        cmds[cnt].tokens = &tokens[0];
        cmds[cnt].stdout_pipe = false;
        cmds[cnt].stdout_append = false;
        cmds[cnt].stdout_file = NULL;
        cmds[cnt].stdin_file = NULL;

        while ((curr_tok = next_token(&next_tok, " ,\t\r\n?")) != NULL) {
            if (curr_tok[0] == '#') {
                break;
            } else if (strcmp(curr_tok, "|") == 0) {
                tokens[token_cnt++] = NULL;
                cmds[cnt++].stdout_pipe = true;
                cmds[cnt].tokens = &tokens[token_cnt];
                cmds[cnt].stdout_pipe = false;
                cmds[cnt].stdout_append = false;
                cmds[cnt].stdout_file = NULL;
                cmds[cnt].stdin_file = NULL;
                pipe = true;
            } else if (strcmp(curr_tok, ">") == 0) {
                tokens[token_cnt++] = NULL;
                pipe = true;
                re_out = true;
            } else if (strcmp(curr_tok, "<") == 0) {
                tokens[token_cnt++] = NULL;
                pipe = true;
                re_in = true;
            } else if (strcmp(curr_tok, ">>") == 0) {
                tokens[token_cnt++] = NULL;
                cmds[cnt].stdout_append = true;
                pipe = true;
                re_out = true;
            } else if (re_out || re_in) {
                tokens[token_cnt++] = NULL;
                if (re_out) {
                    cmds[cnt].stdout_file = curr_tok;
                    re_out = false;
                }
                if (re_in) {
                    cmds[cnt].stdin_file = curr_tok;
                    re_in = false;
                }
            } else {
                tokens[token_cnt++] = curr_tok;
            }
        }

        tokens[token_cnt] = NULL;
        bool background = strcmp(tokens[token_cnt - 1], "&") == 0;

        if (background) {
            tokens[token_cnt - 1] = NULL;
        }

        int result = execute_func(tokens[0], tokens[1], list);

        if (result != 0) {
            end = utime();
            status = 0;
            myNode->runtime = end - start;
          
            if (result == 1) {
                break;
            } else if (result == -1) {
                status = 1;
            } else if (result == 3) {
                printList(list, 1); 
            } else if (result == 4) {
                printList(list, 0);
            }

            insertAtFront(list, myNode);
            continue;
        }

        int pid = fork();

        if (pid == -1) {
            fprintf(2, "fork failed\n");
        }

        if (pid == 0) {
            if (pipe) {
                execute_pipeline(cmds);
            } else {
                execvp(tokens[0], tokens);
            }

            fprintf(2, "Exec failed\n");
            exit(1);
        } else {
            if (!background) {
                wait(&status);
                end = utime();
                myNode->runtime = end - start;
            }

            if (status != 0) {
                printf("☠️ 🪦\n");
            }
            
            insertAtFront(list, myNode);
        }
        free(cmds);
        free(cmd);
    }
    close(fd);
    freeList(list);

    return 0;
}

