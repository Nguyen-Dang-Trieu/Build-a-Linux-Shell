#include <errno.h>
#include "shell.h"
#include "source.h"

void unget_char(struct source_s *src)
{
    if(src->curpos < 0)
    {
        return;
    }

    src->curpos--;
}

// Dùng để chuyển tới kí tự tiếp theo có trong chuỗi
char next_char(struct source_s *src)
{
    if(!src || !src->buffer){
        errno = ENODATA;
        return ERRCHAR;
    }
    // đoạn này dùng để khởi tạo vị trí ban đầu
    if(src->curpos == INIT_SRC_POS){
        src->curpos  = -1;
    }

    if(++src->curpos >= src->bufsize)
    {
        src->curpos = src->bufsize;
        return EOF;
    }
    
    return src->buffer[src->curpos];
}

// muốn xem trước kí tự tiếp theo để đọc mà không di chuyển con trỏ index chính: src.curpos
char peek_char(struct source_s *src)
{
    if(!src || !src->buffer)
    {
        errno = ENODATA;
        return ERRCHAR;
    }

    long pos = src->curpos;   // khi mới khơi tạo thi src->curpos = -2 -> pos = -2
    
    if(pos == INIT_SRC_POS)
    {
        pos++; // pos = -1
    }
    pos++;  // pos = 0

    if(pos >= src->bufsize)
    {
        return EOF;
    }

    return src->buffer[pos];
}


void skip_white_spaces(struct source_s *src)
{
    char c;
    
    if(!src || !src->buffer)
    {
        return;
    }

    while(((c = peek_char(src)) != EOF) && (c == ' ' || c == '\t'))
    {
        next_char(src);
    }
}
