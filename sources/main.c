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
		if(!cmd){
			exit(EXIT_SUCCESS);
		}
		if(cmd[0] == '\0' || strcmp(cmd, "\n") == 0)
		{
			free(cmd);
			continue;
		}
		if(strcmp(cmd, "exit\n") == 0)
		{
			free(cmd);
			break;  // Chỉ kết thúc vòng lặp gần nhất mà nó nằm bên trong đó - Trong trường hợp này là while(1)
		}
		printf("%s\n", cmd);
		free(cmd);
	} while(1);
	exit(EXIT_SUCCESS); // Dùng để kết thúc process bình thường
}

char *read_cmd(void)
{
	char buf[1024];
	char *ptr = NULL;
	int ptrlen = 0;
	
	while(fgets(buf, 1024, stdin))  // điều kiện đúng khi fgets() thành công
	{
		int buflen = strlen(buf);
		
		/*
		 * !ptr == true khi ptr = NULL
		 * !ptr == false khi ptr trỏ tới vùng nhớ hợp lệ
		 */
		if(!ptr)
		{
			ptr = (char *)malloc(buflen + 1);
		}
		else{ // mở rộng vùng nhớ do ptr trỏ tới
			char *ptr2 = (char *)realloc(ptr, ptrlen + buflen + 1);
			if(ptr2) // vùng nhớ cấp phát thành công
			{
				ptr = ptr2; // con trỏ ptr được cập nhập vùng nhớ mới lớn hơn
			}
			else  // nếu cấp phát vùng nhớ thất bại thì sẽ giải phóng luôn vùng nhớ cũ ptr
			{
				free(ptr);
				ptr = NULL; // để tránh con trỏ trỏ tới vùng nhớ không hợp lệ
			}
		}
		
		if(!ptr) // !ptr == true khi ptr = NULL
		{
			fprintf(stderr, "error: failed to allo buffer: %s\n", strerror(errno));
			return NULL;
		}
		
		strcpy(ptr + ptrlen, buf); 
		/* strcpy(ptr + ptrlen, buf);
		 * ptr là vùng nhớ, ptrlen dùng để định vùng nhớ lưu
		 * ví dụ: ptrlen = 2, bắt đầu lưu chuỗi buf tại ví trí 2 (nghĩa là bỏ qua 2 vị trí ban đầu là 0,1
		 *
		 */
		
		if(buf[buflen - 1] == '\n')
		{
			// buf[buflen - 2] != '\\' :dùng để kiểm tra xem có kí tự \ hay không 
			if(buflen == 1 || buf[buflen - 2] != '\\') 
			{
				return ptr;
			}
			
			ptr[ptrlen + buflen - 2] = '\0';
			buflen -= 2;
			print_prompt2();
		}
		
		ptrlen += buflen;
	}
	
	return ptr;
}
		
			
