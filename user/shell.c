#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define NBUILTINS 3
#define MAX_SIZE 100
#define MAX_PATH 128

uint
strspn(const char *str, const char *chars){
    uint i, j;
    for (i = 0; str[i] != '\0'; i++) {
    for (j = 0; chars[j] != str[i]; j++) {
        if (chars[j] == '\0')
        return i;
    }
    }
    return i;
}

uint
strcspn(const char *str, const char *chars){
    const char *p, *sp;
    char c, sc;
    for (p = str;;) {
    c = *p++;
    sp = chars;
    do {
        if ((sc = *sp++) == c) {
        return (p - 1 - str);
        }
    } while (sc != 0);
    }
}

char
*next_token(char **str_ptr, const char *delim){
    if (*str_ptr == NULL) {
    return NULL;
    }

    uint tok_start = strspn(*str_ptr, delim);
    uint tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
    *str_ptr = NULL;
    return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
    /* If the end of the current token is also the end of the string, we
            * must be at the last token. */
    *str_ptr = NULL;
    } else {
    /* Replace the matching delimiter with a NUL character to terminate the
            * token string. */
    **str_ptr = '\0';

    /* Shift forward one character over the newly-placed NUL so that
            * next_pointer now points at the first character of the next token. */
    (*str_ptr)++;
    }

    return current_ptr;
}


struct Node{
    const char* command;
    struct Node* next;
    struct Node* prev;
    int command_num;
    int runtime;
};

// Define the structure for the linked list
struct LinkedList {
    struct Node* head; 
    struct Node* tail; 
    int size;          
};

// Custom strdup function
char *strdup(const char *src) {
    char *dst = (char *)malloc(strlen(src) + 1); 
    if (dst == NULL) {
        printf("Memory allocation failed\n");
        exit(1);  
    }
    strcpy(dst, src); 
    return dst;
}

//create new node
struct Node* createNode(char *cmd, int num){
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->command = strdup(cmd);
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->command_num = num;
    newNode->runtime = 0;
    return newNode;
};

// Function to create a linked list with a sentinel node
struct LinkedList* createList() {
    struct LinkedList* list = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    list->head = createNode(0, 0);  //points the first val to the head
    list->tail = createNode(0, 0);
    list->head->next = list->tail;
    list->tail->prev = list->head; 
    list->size = 0;              
    return list;
}


// Function to insert a node at the front of the list
//gonna make this a doubly linked list, that should speed up reassinging of tail sentinal
void insertAtFront(struct LinkedList* list,struct Node* newNode) {
    //points new node to actual head
       // If the list is empty (size == 0), handle it differently
    if (list->size == 0) {
        newNode->next = list->tail;   // Point to tail sentinel
        newNode->prev = list->head;   // Point to head sentinel
        list->head->next = newNode;   // Update head sentinel to point to newNode
        list->tail->prev = newNode;   // Update tail sentinel to point to newNode
    } else {
        // Insert newNode at the front
        newNode->next = list->head->next;  // Point to the current first node
        newNode->prev = list->head;        // Point back to the head sentinel

        list->head->next->prev = newNode;  // Update current first node's previous to newNode
        list->head->next = newNode;        // Update head sentinel to point to newNode
    }

    list->size++;

    // Check if the list exceeds MAX_SIZE
    if (list->size > MAX_SIZE) {
        struct Node* temp = list->tail->prev; 
        list->tail->prev = temp->prev; 
        temp->prev->next = list->tail;
        free(temp);          
        list->size--;      
    }
}

// Function to print the linked list
void printList(struct LinkedList* list, int t) {
    struct Node* current = list->head->next; 
    while (current != list->tail) {
        if(t == 0){
            printf("[%d] %s", current->command_num, current->command);
        }else{
            printf("[%d][%d] %s", current->command_num, current->runtime, current->command);
        }
        current = current->next;
    }
}

// Function to print the linked list
char *getCommand(struct LinkedList* list, int num) {
    struct Node* current = list->head->next;

    if(num == -1){
        if(list->size == 0){
            return NULL;
        }
        return strdup(current->command);
    }

    while (current != list->tail) {
        if(current->command_num == num){
            return strdup(current->command);
        }
        current = current->next;
    }
    return NULL;
}

bool
isPrefix(const char *word, const char *prefix){
    while(*prefix){
        if(*prefix != ' ' && *prefix != '\n' && *prefix != *word){
            return false;
        }
        prefix++;
        word++;
    }
    return true; 
}
// Function to print the linked list
char *getCommandPrefix(struct LinkedList* list, char *cmd) {
    struct Node* current = list->head->next; 
    char *prefix = cmd + 1;
    char first_word[128];

    int i = 0;
    while (prefix[i] != '\0' && prefix[i] != ' ') {
        first_word[i] = prefix[i];
        i++;
    }
    first_word[i] = '\0'; 

    while (current != list->tail) {
        char* other = strdup(current->command);
        if(isPrefix(other, first_word)){
            free(other);
            return other;
        }
        free(other);
        current = current->next;
    }
    return NULL;
}

int
isDigit(char c){
    if(c >= '0' && c <= '9'){
        return 1;
    }else{
        return 0;
    }
}

// Function to free the linked list
void freeList(struct LinkedList* list) {
    struct Node* current = list->head;
    while (current != NULL) {
        struct Node* temp = current;
        current = current->next;
        free((void*)temp->command);
        free(temp); 
    }
    free(list); 
}

typedef int (*builtin_func)(char *cmd, char *arg, struct LinkedList* list);


struct builtin{
    const char* name;
    builtin_func func;
};



int
builtin_exit(char *cmd, char *arg, struct LinkedList* list){
    printf("Aight, I'm gonna head out\n");
    return 1;
}

int
builtin_cd(char *cmd, char* arg, struct LinkedList* list){
    if(arg == NULL){
        chdir("/");
    }else{
        if(chdir(arg) == -1){
            fprintf(2, "chdir: no such file or directory:  %s\n", cmd);
            return -1;
        }
    }
    return 2;
}

int
builtin_history(char *cmd, char *arg, struct LinkedList* list){
    if(arg != 0){
        if(strcmp(arg, "-t") == 0){
            return 3;
        }else{
            fprintf(2, "history command failed\n");
            return -1;
        }
    }
    return 4;
}

char *
builtin_bang(char *cmd, struct LinkedList* list){ 
    if(cmd[0] == '!'){
        char* last = "";
        if(strcmp(cmd, "!!\n") == 0){
            last = getCommand(list, -1);
        }else{
            int num = 0;
            int i = 1;

            bool prefix = false;
            while(cmd[i] != '\0' && cmd[i] != ' ' && cmd[i] != '\n'){
                if(isDigit(cmd[i])){
                    num = num * 10 + ((cmd[i] - '0') % 10);
                }else{
                    prefix = true;
                    break;
                }
                i+=1;
            }

            if(!prefix){
                last = getCommand(list, num);
            }else{
                last = getCommandPrefix(list, cmd);
            }
        }
        if(last != NULL){
            free(cmd);
            cmd = strdup(last);
        }
        if(last == NULL){
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



int
execute_func(char *cmd, char *arg, struct LinkedList* list){
    for(int i = 0; i < NBUILTINS; i++){
        if(strcmp(cmd, builtins[i].name) == 0){
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


//Check if a path contains a '/'
bool contains_slash(const char *path) {
    while (*path != '\0') {
        if (*path == '/') {
            return true;
        }
        path++;
    }
    return false;
}

// Concatenate src to dest. Assumes dest has enough space.
void str_concat(char *dest, const char *src) {
    while (*dest) {  // Move to the end of dest
        dest++;
    }
    while (*src) {  // Copy src to dest
        *dest++ = *src++;
    }
    *dest = '\0';  // Null-terminate the string
}

// Custom execvp function for path searching and execution
int execvp(const char *pathname, char *const argv[]) {
    char path[MAX_PATH];

    // 1. Check if pathname contains a '/'
    if (contains_slash(pathname)) {
        return exec(pathname, (char **) argv);
    }

    // 2. Search in the root directory
    path[0] = '/';
    path[1] = '\0'; 
    str_concat(path, pathname);

    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        close(fd);
        return exec(path, (char **) argv);
    }

    // 3. Build path for the current directory
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


void
execute_pipeline(struct command *cmd)
{
  int fd[2];
  if (cmd->stdout_pipe){
    if(pipe(fd) == -1) {
      fprintf(2, "Could not create pipe.\n");
      exit(1);
    }
  }else{
    if(cmd->stdout_file != NULL || cmd->stdin_file != NULL){
      int open_flags = 0;
      int file_fd = 0;

      if(cmd->stdin_file != NULL){
        open_flags = O_RDONLY;
        file_fd = open(cmd->stdin_file, open_flags);

        if (file_fd == -1) {
            fprintf(2, "Could not open file\n");
            exit(1);
        }
        close(0);

        if(dup(file_fd) == -1){
            fprintf(2, "dup failed\n");
            exit(1);
        }

        close(file_fd); 
      }

      if(cmd->stdout_file != NULL){
        if(cmd->stdout_append){
            open_flags = O_RDWR | O_CREATE | O_APPEND;
        }else{
            open_flags = O_RDWR | O_CREATE | O_TRUNC;
        }

        file_fd = open(cmd->stdout_file, open_flags);

        if (file_fd == -1) {
            fprintf(2, "Could not open file\n");
            exit(1);
        }

        close(1);

        if(dup(file_fd) == -1){
            fprintf(2, "dup failed\n");
            exit(1);
        }

        close(file_fd);
      }

    }
    


    execvp(cmd->tokens[0], cmd->tokens); //execute command if there is no need to pipe
    close(fd[0]);
    close(fd[1]);
    return;
  }

  int pid = fork();
  if (pid == -1) {
    fprintf(2, "Fork failed!\n");
    exit(1);
  } else if (pid == 0) {
    // Child
    close(fd[0]); // close the read end of the pipe
    close(1); // stdout
    dup(fd[1]); // stdout now goes to the pipe
    close(fd[1]); //closes original write end

    execvp(cmd->tokens[0], cmd->tokens);
    exit(1);
  } else {
    close(fd[1]); // close write end of pipe
    close(0); // stdin
    dup(fd[0]); // stdin now comes from the pipe
    close(fd[0]); // closes the og read end
    execute_pipeline(cmd + 1);
  }
}

void multi_line_command(int fd, char **cmd){
    uint size = 128;
    *cmd = malloc(size);

    (*cmd)[0] = '\0';
    int curr_len = 0;
    char *input = NULL;
    uint input_size = 0;
    int len = 0;

    while((len = getline(&input, &input_size, fd)) > 0){
        if(len > 1 && input[len - 2] == '\\'){
            input[len - 2] = ' ';
            input[len - 1] = '\0';

            while(curr_len + len > size){
                int new_size = size * 2;
                char* tmp = malloc(new_size);

                memcpy(tmp, *cmd, curr_len);
                free(*cmd);
                *cmd = tmp;
                size = new_size;
            }

            str_concat(*cmd, input);
            curr_len += len - 1;
        }else{
            str_concat(*cmd, input);
            curr_len += len;
            break;
        }
    }

    if(curr_len == 0){
        free(*cmd);
        *cmd = NULL;
    }

    free(input);
}


int
main(int argc,char *argv[]){
    printf("Welcome to my shell🐚\n\n");
    int command_num = 1;
    int status = 0;
    uint64 start, end;
    int fd = open(argv[1], O_RDONLY);
    bool isScript = false;
    struct LinkedList* list = createList();

    if(fd > 0){
        isScript = true;
    }
    if(!isScript){
        fd = 0;
    }
    
    while(true){

        char dir[128];
        getcwd(dir, 128);

        if(!isScript){
            printf("[%d]-[%d]-[%s]$ ", status, command_num, dir);
        }

        char *cmd = NULL;
        multi_line_command(fd, &cmd);

        if(cmd == NULL){
            printf("Goodbye 🥲\n");
            break;
        }

        if(strcmp(cmd, "\n") == 0 || cmd[0] == '#'){//skipable before executing
            continue;
        }

        start = utime();
        struct Node* myNode = createNode(cmd, command_num);

        if(cmd[0] == '!'){
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
            
            if(curr_tok[0] == '#'){ //handles comments
                break;
            }else if(strcmp(curr_tok, "|") == 0){
                tokens[token_cnt++] = NULL;
                cmds[cnt++].stdout_pipe = true;
                cmds[cnt].tokens = &tokens[token_cnt];
                cmds[cnt].stdout_pipe = false;
                cmds[cnt].stdout_append = false;
                cmds[cnt].stdout_file = NULL;
                cmds[cnt].stdin_file = NULL;
                pipe = true;
            }else if(strcmp(curr_tok, ">") == 0){
                tokens[token_cnt++] = NULL;
                pipe = true;
                re_out = true;
            }else if(strcmp(curr_tok, "<") == 0){
                tokens[token_cnt++] = NULL;
                pipe = true;
                re_in = true;
            }else if(strcmp(curr_tok, ">>") == 0){
                tokens[token_cnt++] = NULL;
                cmds[cnt].stdout_append = true;
                pipe = true;
                re_out = true;
            }else if(re_out || re_in){
                tokens[token_cnt++] = NULL;
                if(re_out){
                    cmds[cnt].stdout_file = curr_tok;
                    re_out = false;
                }
                if(re_in){
                    cmds[cnt].stdin_file = curr_tok;
                    re_in = false;
                }
            }else{
                tokens[token_cnt++] = curr_tok;
            }
        }


        tokens[token_cnt] = 0;

        bool background = strcmp(tokens[token_cnt - 1], "&") == 0;

        if(background){
            tokens[token_cnt - 1] = NULL;
        }
        
        
        int result = 0;
        result = execute_func(tokens[0], tokens[1], list);



        if(result != 0){
            end = utime();
            status = 0;
            myNode->runtime = end - start;
            insertAtFront(list, myNode);
            if(result == 1){
                break;
            }else if(result == -1){
                status = 1;
            }else if(result == 3){
                printList(list, 1); // t flag is used
            }else if(result == 4){
                printList(list, 0);
            }
            continue;
        }

        int pid;

        pid = fork();

        if(pid == -1){
            fprintf(2, "fork failed\n");
        }

        if(pid == 0){
            //child process
            if(pipe){
                execute_pipeline(cmds);
            }else{
                execvp(tokens[0], tokens);
            }
            fprintf(2, "Exec failed\n");
            exit(1);
        }else{
            //parent process
            if(!background){ 
                wait(&status);
            }else{
                printf("Doing background job\n");
            }

            end = utime();
            myNode->runtime = end - start;
            insertAtFront(list, myNode);
            if(status != 0){
                printf("☠️ 🪦\n");
            }
        }
        free(cmds);
        free(cmd);
    }
    close(fd);
    freeList(list);

    //what is missing
    // 0. good code quality

    return 0;
}
