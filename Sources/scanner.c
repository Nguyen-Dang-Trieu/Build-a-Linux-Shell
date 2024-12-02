#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "shell.h"
#include "scanner.h"
#include "source.h"

char *tok_buf = NULL;
int   tok_bufsize  = 0;
int   tok_bufindex = -1;

/* special token to indicate end of input */
struct token_s eof_token = 
{
    .text_len = 0,
};


void add_to_buf(char c)
{
    tok_buf[tok_bufindex++] = c; // thêm kí tự c vào trong bộ đệm buf

    if(tok_bufindex >= tok_bufsize)
    {
        char *tmp = realloc(tok_buf, tok_bufsize*2);

        if(!tmp)
        {
            errno = ENOMEM;
            return;
        }

        tok_buf = tmp;
        tok_bufsize *= 2;
    }
}

// hàm này trả về token
struct token_s *create_token(char *str)
{
    struct token_s *tok = malloc(sizeof(struct token_s));
    
    if(!tok)
    {
        return NULL;
    }

    memset(tok, 0, sizeof(struct token_s));
    tok->text_len = strlen(str);
    
    char *nstr = malloc(tok->text_len+1);
    
    if(!nstr)
    {
        free(tok);
        return NULL;
    }
    
    strcpy(nstr, str);
    tok->text = nstr;
    
    return tok;
}

//free_token(tok);
void free_token(struct token_s *tok)
{
    if(tok->text)
    {
        free(tok->text);
    }
    free(tok);
}


struct token_s *tokenize(struct source_s *src)
{
    int  endloop = 0; // dùng để thoát khỏi vòng lặp khi đã đọc đủ 1 token

    // kiem tra chuoi dau vao co rong hay khong ?
    if(!src || !src->buffer || !src->bufsize)
    {
        errno = ENODATA;
        return &eof_token;
    }
    
    //
    if(!tok_buf) // dieu kien if = true khi tok_buf = NULL - chua duoc khoi tao de luu chuoi
    {
        tok_bufsize = 1024;
        tok_buf = malloc(tok_bufsize);
        if(!tok_buf)
        {
            errno = ENOMEM;
            return &eof_token;
        }
    }

    tok_bufindex     = 0;
    tok_buf[0]       = '\0';

    // -------- tach lay token
    // Vidu: src = echo Hello World
    char nc = next_char(src);

    if(nc == ERRCHAR || nc == EOF)
    {
        return &eof_token;
    }

    do
    {
        switch(nc)
        {
            // khi gặp kí tự " " và "\t" thì câu lệnh thực thi bên dưới sẽ được kích hoạt
            case ' ':
            case '\t':
                if(tok_bufindex > 0)
                {
                    endloop = 1;
                }
                break;
                
            case '\n':
                if(tok_bufindex > 0)
                {
                    unget_char(src);
                }
                else
                {
                    add_to_buf(nc);
                }
                endloop = 1;
                break;
                
            default:
                add_to_buf(nc);
                break;
        }

        if(endloop)
        {
            break;
        }

    } while((nc = next_char(src)) != EOF);

    if(tok_bufindex == 0)
    {
        return &eof_token;
    }
    
    if(tok_bufindex >= tok_bufsize)
    {
        tok_bufindex--;
    }
    tok_buf[tok_bufindex] = '\0';


    // ----- tao token -----------------------------------------------------------

    struct token_s *tok = create_token(tok_buf);
    if(!tok)
    {
        fprintf(stderr, "error: failed to alloc buffer: %s\n", strerror(errno));
        return &eof_token;
    }

    tok->src = src;
    return tok;
}
