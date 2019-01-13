#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myXml.h"

int main() {
    pthread_t pt[THREADNUM];
    int result[THREADNUM];
    int readCount, blocknum = 0;
    buffblock *bufhead = NULL, *buf, *tmp;
    FILE *fp;
    fp = fopen("test.xml", "r");

    //读取申请第一个缓冲块
    tmp = mallocBuffer();
    //循环读入数据到缓冲区并进行第一阶段的处理
    while ((readCount = fread(tmp->buf, sizeof(char), BUFLEN, fp)) > 0) {
        if (blocknum > 0) {       //中间的缓存块链表的链接
            printf("blocknum: %d\n", blocknum);
            buf->next = storeBlock(tmp, blocknum++, readCount);
            buf = buf->next;
            //printf("*\n");
        } else {  //第一个缓冲区作为缓冲区链表头部的处理
            bufhead = storeBlock(tmp, blocknum++, readCount);
            buf = bufhead;
        }
        tmp = mallocBuffer();
    }
    fclose(fp);
    printf("\n\t 分块读入内存成功！划分文件块%d个！\n\n", blocknum);


    //多线程进行第二阶段处理
    int i;
    buf = bufhead;
    for (i = 0; i < THREADNUM; i++) {                                                               //循环创建线程开始第二阶段的处理
        result[i] = pthread_create(&(pt[i]), NULL, firstcheck, (void *) buf);
        buf = buf->next;
        if (result[i] != 0) {
            printf("create pthread error!\n");
            exit(-1);
        }
    }
    for (i = 0; i < THREADNUM; i++) {                                                               //等待线程结束
        printf("第%d号线程处理完毕！\n\n",i);
        pthread_join(pt[i], NULL);
    }
    printf("多线程识别xml语法格式！ 并行线程数为%d个!\n\n", THREADNUM);
//  */
   //    printf("\t 未完成！\n\nn");

/*  进行第三阶段的处理   */
//    if (finalcheck(bufhead) == 1)
//        printf("\t该XML文件符合规范！\n");
//    else
//        printf("\t该XML文件符合规范！\n");
////  */
//    return 0;s
}

//打印相应标签类型
char *printEnum(nodetype type) {
    switch (type) {
        case Stag_start:
            return "Stag";
        case Etag_start:
            return "Etag";
        case PI_start:
            return "PI";
        case Content:
            return "content";
        case CDSECT_start:
            return "CDATA";
        case COMMENT_start:
            return "comment";
        default:
            return "Tag";
    }
}
