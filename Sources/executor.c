#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "shell.h"
#include "node.h"
#include "executor.h"


// hàm này có tác dụng lấy đường dẫn thư mục chứa file nhị phân thực thi của từng lệnh trong shell
// Ví dụ: search_path("echo")
char *search_path(char *file)
{
    // PATH cũng kết thúc chuỗi là kí tự /0
    char *PATH = getenv("PATH");  // hàm getenv dùng để lấy giá trị của biến môi trường PATH
    // *PATH = /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin

    
    char *p    = PATH;
    char *p2;
    
    while(*p) // nếu PATH là NULL thì vòng lặp while sẽ không chạy
    {
        p2 = p;

        while(*p2 && *p2 != ':') { // điều kiện này dùng để kiểm tra dấu ":" dùng để phân tách từng đường dẫn thư mục
            p2++;
        }
        
        // tính số lượng kí tự dựa vào địa chỉ mà 2 con trỏ đang trỏ tới
	    int  plen = p2 - p;
        if(!plen){
            plen = 1;
        }
        
        int  alen = strlen(file);
        char path[plen + 1 + alen +1];
        // strncpy() và strcat(): Xây dựng đường dẫn đầy đủ cho file bằng cách nối thư mục và tên file lại với nhau.
	    strncpy(path, p, p2-p);
        path[p2-p] = '\0';
        
	    if(p2[-1] != '/'){
            strcat(path, "/");
        }

        strcat(path, file);
        
        /**
         * 
         * stat(path, &st): Kiểm tra xem file tồn tại và có phải là file thông thường không. Nếu tìm thấy file, hàm sẽ trả về 
         * đường dẫn đầy đủ đến file. Nếu không, nó tiếp tục tìm kiếm.
         */
	    struct stat st;
        if(stat(path, &st) == 0){
            // File regular (fle thông thường) là loại phải lưu trữ dữ liệu trực tiếp

            if(!S_ISREG(st.st_mode)){ // kiểm tra xem 1 file có phải là file thông thường hay không,
                errno = ENOENT; // No such file or directory"
                p = p2;
                if(*p2 == ':'){
                    p++;
                }
                continue;
            }

            p = malloc(strlen(path)+1);
            if(!p){
                return NULL;
            }
            
	        strcpy(p, path);
            return p;
        }
        else {   /* file not found */
            p = p2;
            if(*p2 == ':'){
                p++;
            }
        }
    }
/**
 * 
 * Trả về: Nếu tìm thấy file, hàm trả về đường dẫn đầy đủ. Nếu không tìm thấy, hàm trả về NULL và thiết lập lỗi ENOENT 
 * (file không tồn tại).
 * 
 */
    errno = ENOENT;
    return NULL; // dùng đề vào điều kiện if(!path) trong hàm do_exec_cmd()
}

// hàm thực thi lệnh bằng cách gọi execv
int do_exec_cmd(int argc, char **argv)
{
    if(strchr(argv[0], '/'))
    {
        execv(argv[0], argv);
    }
    else
    {
        char *path = search_path(argv[0]);
        if(!path)
        {
            return 0;
        }
        execv(path, argv);
        free(path);
    }
    return 0;
}


static inline void free_argv(int argc, char **argv)
{
    if(!argc)
    {
        return;
    }

    while(argc--)
    {
        free(argv[argc]);
    }
}

// hàm là hàm chính trong trình thực thi của chúng ta. Nó lấy AST của lệnh và chuyển đổi nó thành danh sách đối số 
// do_simple_command(cmd);
int do_simple_command(struct node_s *node)
{   
    // KIểm tra xem node có null hay không --> Nếu node = null thì điều kiện if đúng 
    if(!node) 
    {
        return 0;
    }

    struct node_s *child = node->first_child;
    if(!child)
    {
        return 0;
    }
    
    int argc = 0;
    long max_args = 255;
    char *argv[max_args+1];     /* keep 1 for the terminating NULL arg */
    char *str;
    
    while(child) // điều kiện while đúng khi child khác NULL
    {
        str = child->val.str;
        argv[argc] = malloc(strlen(str)+1);
        
	    if(!argv[argc])
        {
            free_argv(argc, argv);
            return 0;
        }
        
	    strcpy(argv[argc], str);
        // ++a tăng giá trị rồi mới thực thi, a++ thực thi xong rồi mới tăng giá trị
        if(++argc >= max_args)
        {
            break;
        }
        child = child->next_sibling;
    }
    argv[argc] = NULL;

    pid_t child_pid = 0;
    if((child_pid = fork()) == 0)
    {
        do_exec_cmd(argc, argv);
        fprintf(stderr, "error: failed to execute command: %s\n", strerror(errno));
        if(errno == ENOEXEC)
        {
            exit(126);
        }
        else if(errno == ENOENT)
        {
            exit(127);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else if(child_pid < 0)
    {
        fprintf(stderr, "error: failed to fork command: %s\n", strerror(errno));
        return 0;
    }

    int status = 0;
    waitpid(child_pid, &status, 0);
    free_argv(argc, argv);
    
    return 1;
}
