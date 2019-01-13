#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "myXml.h"


int finalcheck(buffblock *block_head) {
    int block_len = 0, num = 0;
    buffblock *block, *new_block;
    block = block_head;
    new_block = mallocBuffer();
    node *r = NULL;
    printf("finalcheck\n");
    while (block != NULL) {
//        printf("0\n");
        r = block->nodearr;
        num += 1;
        //printf("%d\n",num);

        while (r != NULL) {
            printf("2\n");
//            printf("r!=NULL: %d\n", r != NULL);
            //printf("block_len:%d\n", block_len);
//            if (r->next != NULL) {
//                printf("r!=NULL: %d\n", r != NULL);
//                printf("r->offset:%d,r->taglen:%d\n", r->offset, r->taglen);
//                printf("&(block->buf[r->offset]): *%s*\n", &(block->buf[r->offset]));
//            }

            strncat(new_block->buf,&(block->buf[r->offset]), r->taglen);
            //printf("3\n");
            //break;
            block_len += r->taglen;
            r = r->next;
            //printf("new_block->buf is *%s*\n",new_block->buf);
        }
        //printf("5\n");

        block = block->next;
//        printf("6\n");

    }

    new_block->buflen = block_len;//块长度
    new_block->bufnum = -1;//块号
    storeBlock(new_block, -1, block_len);//第一阶段
    firstcheck(new_block);//第二阶段

    printf("\t 最后处理待处理块有%d个!\n\n", num);
    if (new_block->nodearr == NULL)
        return 1;//合法
    else
        return 0;//非法
}

