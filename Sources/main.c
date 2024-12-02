#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"
#include "source.h"
#include "parser.h"
#include "executor.h"


int main(int argc, char **argv)
{
    char *cmd;
    do
    {
        print_prompt1();   // tạo dấu nhắc lệnh $
        cmd = read_cmd();  // đọc kí tự đầu vào
        if(!cmd)
        {
            exit(EXIT_SUCCESS);
        }
        if(cmd[0] == '\0' || strcmp(cmd, "\n") == 0)
        {
            free(cmd);
            continue;
        }
        if(strcmp(cmd, "exit\n") == 0) // strcmp() so sánh 2 string
        {
            free(cmd);
            break;
        }
	    struct source_s src;
        src.buffer   = cmd;
        src.bufsize  = strlen(cmd);
        src.curpos   = INIT_SRC_POS;  // src.curpos = -2; #define INIT_SRC_POS -2 in source.h
        parse_and_execute(&src);
        free(cmd);
    } while(1);
    exit(EXIT_SUCCESS);
}


char *read_cmd(void)
{
    char buf[1024];
    char *ptr = NULL;
    char ptrlen = 0;
    while(fgets(buf, 1024, stdin))
    {
        int buflen = strlen(buf);
        if(!ptr)
        {
            ptr = malloc(buflen+1);
        }
        else
        {
            char *ptr2 = realloc(ptr, ptrlen+buflen+1);
            if(ptr2)
            {
                ptr = ptr2;
            }
            else
            {
                free(ptr);
                ptr = NULL;
            }
        }
        if(!ptr)
        {
            fprintf(stderr, "error: failed to alloc buffer: %s\n", strerror(errno));
            return NULL;
        }
        strcpy(ptr+ptrlen, buf);
        if(buf[buflen-1] == '\n')
        {
            if(buflen == 1 || buf[buflen-2] != '\\')
            {
                return ptr;
            }
            ptr[ptrlen+buflen-2] = '\0';
            buflen -= 2;
            print_prompt2();
        }
        ptrlen += buflen;
    }
    return ptr;
}


int parse_and_execute(struct source_s *src)
{
    skip_white_spaces(src);

    struct token_s *tok = tokenize(src);

    if(tok == &eof_token)
    {
        return 0;
    }

    while(tok && tok != &eof_token) // token không rỗng và token khác với eof_token
    {
        struct node_s *cmd = parse_simple_command(tok);

        if(!cmd)
        {
            break;
        }

        do_simple_command(cmd);
        free_node_tree(cmd);

        // Đây là điều kiện để thoát khỏi while(tok & tok != &eof_token);
        tok = tokenize(src); // dùng để lấy token có phải là eof_tọken không - Kiểm tra lần cuối 
    }
    return 1;
}
