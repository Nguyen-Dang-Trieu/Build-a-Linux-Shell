#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"

int main(int argc, char **argv)
{
    char *cmd;
    do
    {
        print_prompt1();
        cmd = read_cmd();
        if (!cmd) {  // Nếu cmd là NULL, thoát khỏi vòng lặp
            break;
        }
        if (cmd[0] == '\0' || strcmp(cmd, "\n") == 0)
        {
            free(cmd);
            continue;
        }
        printf("%s\n", cmd);
        free(cmd);
    } while (1);
    exit(EXIT_SUCCESS);
}

char *read_cmd(void)
{
    char buf[1024];
    char *ptr = NULL;
    int ptrlen = 0;
    
    while (fgets(buf, 1024, stdin))  // điều kiện đúng khi fgets() thành công
    {
        int buflen = strlen(buf);

        if (!ptr)
        {
            ptr = (char *)malloc(buflen + 1);
            if (!ptr) {
                fprintf(stderr, "error: failed to allocate buffer: %s\n", strerror(errno));
                return NULL;
            }
        }
        else {
            char *ptr2 = (char *)realloc(ptr, ptrlen + buflen + 1);
            if (ptr2) {
                ptr = ptr2;
            }
            else {
                free(ptr);
                fprintf(stderr, "error: failed to allocate buffer: %s\n", strerror(errno));
                return NULL;
            }
        }

        strcpy(ptr + ptrlen, buf);

        if (buf[buflen - 1] == '\n')
        {
            if (buflen == 1 || buf[buflen - 2] != '\\')
            {
                if (strcmp(ptr, "exit\n") == 0) {
                    free(ptr);  // Giải phóng bộ nhớ trước khi thoát
                    return NULL;  // Trả về NULL để kết thúc vòng lặp
                }

                // echo Hello world -> Hello world
                char *ptr_result = (char *)malloc(strlen(ptr + 4) + 1);
                if (!ptr_result) {
                    fprintf(stderr, "error: failed to allocate memory for ptr_result: %s\n", strerror(errno));
                    free(ptr);
                    return NULL;
                }
                strcpy(ptr_result, ptr + 4);
                free(ptr);
                return ptr_result;
            }

            ptr[ptrlen + buflen - 2] = '\0';
            buflen -= 2;
            print_prompt2();
        }

        ptrlen += buflen;
    }

    return ptr;
}
