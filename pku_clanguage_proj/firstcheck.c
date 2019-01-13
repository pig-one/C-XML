#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myXml.h"

lable *creatData(char *tagName, nodetype bt) {        //创建一个栈数据结点并赋值
    lable *node = (lable *) malloc(sizeof(lable));
    strcpy(node->tagname, tagName);
    node->bt = bt;
    return node;
}

int Push(stack *st, lable *node) {                   //入栈操作
    if (st->top == STACKSIZE - 1) {
        printf("firstcheck栈满，入栈失败！\n");                   //栈已满，不能入栈
        exit(-1);
    }
    st->data[++(st->top)] = node;
    return 1;                                       //入栈成功
}

lable *Pop(stack *st) {                              //出栈操作(出栈后记得Free释放掉)
    if (st->top == -1) {
        printf("firstcheck栈空，出栈失败！\n");                   //栈已空，不能出栈
        exit(-1);
    }
    return st->data[(st->top)--];
}

char *getname(char *strarr, char *str, char ch) {                    //提取表签名
    int i = 0;
    for (i = 0; i < NAMELEN && str[i] != ch && str[i] != ' '; i++) strarr[i] = str[i];
    strarr[i] = '\0';
    return strarr;
}

char *getTagName(char *strarr, char *str, nodetype bt) {              //提取标签名
    if (bt == Stag_start) {
        getname(strarr, str, '>');
    } else if (bt == Etag_start) {
        getname(strarr, str, '>');
    } else {//default
        strarr[0] = '\0';
    }
    return strarr;
}

void stackInOut(buffblock *block, char *loc, nodetype bt, stack *st, node *r) {
    char *str = (char *) malloc(NAMELEN * sizeof(char));
    strcpy(str, getTagName(str, loc, bt));
    if (st->top > -1 && strcmp(str, st->data[st->top]->tagname) == 0) {
        //printf("块号：%d\t出栈前\t栈顶元素：%s\t栈内元素个数%d\n", block->bufnum, st->data[st->top]->tagname, st->top+1);
        lable *delnode = Pop(st);
        free(delnode);
        free(str);
    } else {
        if (str[0] == '\0') return;
        lable *node = creatData(str, bt);
        Push(st, node);
        //printf("块号：%d\t入栈后\t栈顶元素：%s\t栈内元素个数%d\n", block->bufnum, str,st->top+1);
    }
}

void stackMatching(buffblock *block) {                                     //每个缓存块内的栈匹配
    if (block == NULL) return;
    node *r = block->nodearr;
    stack *st = (stack *) malloc(sizeof(stack));
    st->top = -1; //创建栈并初始化
    char *name = (char *) malloc(NAMELEN * sizeof(char));
    strcpy(name, getTagName(name, &(block->buf[r->offset]), r->bt));
    lable *node = creatData(name, r->bt);
    Push(st, node);
    r = r->next;
    while (r->next != NULL) {                                                 //该块最后一个标签之前的表签名分析
        stackInOut(block, &(block->buf[r->offset]), r->bt, st, r);
        r = r->next;
    }
    free(st);//  */
}

//  第二阶段
void *firstcheck(void *buf) {
    buffblock *block = (buffblock *) buf;
    stackMatching(block);
    return (void *) NULL;
}
