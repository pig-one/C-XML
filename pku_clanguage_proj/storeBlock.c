#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myXml.h"

buffblock *mallocBuffer() {                                        //申请缓冲区块并初始化
    buffblock *p = (buffblock *) malloc(sizeof(buffblock));
    p->nodearr = NULL;
    p->bufnum = -1;
    p->buflen = -1;
    p->next = NULL;
    return p;
}


int strCmp(char *str, int start, int len, char str2[]) {                //字符串比较
    for (int i = 0; i < len; ++i)
        if (str[start + i] != str2[i]) return 0;
    return 1;
}


node *nodenode(nodetype type, int offset, int taglen) {                              //创建标记信息链表
    node *nodelist = (node *) malloc(sizeof(node));
    nodelist->offset = offset;
    nodelist->bt = type;
    nodelist->taglen = taglen;
    nodelist->next = NULL;
    return nodelist;
}


buffblock *storeBlock(buffblock *block, int blocknum, int buflen) {         //分析数据块，找到个标签的位置并存入链表中
    node *p = NULL;
    int i = 0, j = 0, taglen = 0;
    block->bufnum = blocknum;
    block->buflen = buflen;
    //处理块头非标签字符
    while (block->buf[i] != '<' && i < BUFLEN) {
        i++;
    };
    if (i > 0) {
        taglen = i - j;
        block->nodearr = nodenode(Content, j, taglen);
        p = block->nodearr;
    };
    while (i < BUFLEN) {
        j = i;
        i++; //当前字符是'<'时判断下一个字符的类型
        if (block->buf[i] != '/' && block->buf[i] != '?' && block->buf[i] != '!') {     //该块为<xxxx/>或</xxxx>
            while (i < BUFLEN) {
                if (block->buf[i] == '/' && block->buf[i + 1] == '>') {
                    break; // 自身成为一个完整的开始结束标签，后期不用再处理 <xxxx/>
                } else if (block->buf[i] == '>') { // 开始标签 <xxxx>
                    if (p == NULL) {//node表头
                        block->nodearr = nodenode(Content, j, i - j);
                        p = block->nodearr;
                    } else {
                        p->next = nodenode(Stag_start, j, i - j);
                        p = p->next;
                    }
                    break;
                }
                i++;
            }
        } else if (block->buf[i] == '/') { //</xxxx>
            j = i;
            while (i < BUFLEN) {
                if (block->buf[i++] == '>') {
                    if (p == NULL) {
                        block->nodearr = nodenode(Content, j, i - j);
                        p = block->nodearr;//表头
                    } else {
                        p->next = nodenode(Etag_start, i, i - j);
                        p = p->next;
                    }
                    break;
                }

            }
        } else if (block->buf[i] == '?') {  //<?xxxx?>
            j = i;
            while (i < BUFLEN) {
                if (strCmp(block->buf, i++, 2, "?>")) {
                    p->next = nodenode(PI_start, j, i - j);
                    p = p->next;
                    break;
                }
            }
        } else if (strCmp(block->buf, i, 3, "!--")) {//<!--xxxx-->
            j = i;
            while (i < BUFLEN) {
                if (strCmp(block->buf, i++, 3, "-->")) {
                    p->next = nodenode(COMMENT_start, j, i - j );
                    p = p->next;
                    break;
                }
            }
        } else if (strCmp(block->buf, i, 8, "![CDATA[")) { //<![CDATA［xxxx]]>
            j = i;
            while (i < BUFLEN) {
                if (strCmp(block->buf, i++, 3, "]]>")) {
                    p->next = nodenode(CDSECT_start, j, i - j );
                    p = p->next;
                    break;
                }
            }
        }
    }
    //处理块 尾部非标签字符
//    printf("i=%d, BUFLEN:%d\n", i, BUFLEN);
    while (block->buf[i] <= 33 || block->buf[i] >= 127) {
//        printf("i=:%d,block->buf[i]=%c\n", i, block->buf[i]);
        i--;
    };
    if (block->buf[i] != '>') {//不是以 > 结尾必然是有问题的标签
//        printf("block->buf[i]: -%d-*%c*\n", block->buf[i], block->buf[i]);
        for (--i; i > 0; i--) {
            if (block->buf[i] == '<') {
                p->next = nodenode(Content, i, BUFLEN - i);
                p = p->next;
                break;
            }
        }
    }
//    printf("\nblocknum: %d buflen: %d\n\n\n", blocknum, buflen);
    return block;
}

