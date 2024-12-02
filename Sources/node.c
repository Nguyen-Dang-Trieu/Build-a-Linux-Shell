#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "shell.h"
#include "node.h"
#include "parser.h"


struct node_s *new_node(enum node_type_e type)
{
    struct node_s *node = malloc(sizeof(struct node_s));

    if(!node) { return NULL; }
    
    memset(node, 0, sizeof(struct node_s));
    node->type = type;
    
    return node;
}


/*
Hàm này thêm một nút con vào một nút cha. Nếu nút cha chưa có con, thì nút con được gán làm con đầu tiên. 
Nếu đã có con, hàm sẽ tìm anh em cuối cùng trong danh sách con và thêm nút con mới vào cuối danh sách.
*/
// add_child_node(cmd, word)  

// Hàm add_child_node thực hiện cấu trúc dữ liệu danh sách liên kết
void add_child_node(struct node_s *parent, struct node_s *child)
{
    if(!parent || !child) { return; }

    // cmd.first_child = 0 <-> cmd.first_child = NULL;
    if(!parent->first_child) { // ĐIỀU KIỆN IF ĐÚNG KHI: cmd.first_child = NULL
        parent->first_child = child;  // ta sẽ gán địa chỉ of word cho cmd.first_child
    }
    else{  // điều kiện ELSE xảy ra khi đã có 1 node child rồi
        struct node_s *sibling = parent->first_child;
    
    	while(sibling->next_sibling) // điều kiện while đúng khi sibling->next_sibling khác NULL (0)
        {
            sibling = sibling->next_sibling;
        }
    
    	sibling->next_sibling = child;
        child->prev_sibling = sibling;
    }
    parent->children++;
}

// set_node_val_str(word, tok->text);
void set_node_val_str(struct node_s *node, char *val)
{
    node->val_type = VAL_STR;

    if(!val){
        node->val.str = NULL;
    }
    else
    {
        char *val2 = malloc(strlen(val)+1);
    
    	if(!val2){
            node->val.str = NULL;
        }
        else{
            strcpy(val2, val);
            node->val.str = val2;
        }
    }
}

// free_node_tree(cmd);
void free_node_tree(struct node_s *node)
{
    if(!node)
    {
        return;
    }

    struct node_s *child = node->first_child;
    
    while(child)
    {
        struct node_s *next = child->next_sibling;
        free_node_tree(child);
        child = next;
    }
    
    if(node->val_type == VAL_STR)
    {
        if(node->val.str)
        {
            free(node->val.str);
        }
    }
    free(node);
}

