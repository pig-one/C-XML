#include <pthread.h>

#ifndef MYXML_H_INCLUDED
#define MYXML_H_INCLUDED

#define BUFLEN 10240                    //缓冲区字符区大小10K
#define APPEND 512                      //填充字符区大小
#define NAMELEN 256                     //标签名字最大长度
#define STACKSIZE 16                    //栈的深度
#define THREADNUM 10                    //第二阶段解析并行的线程数


typedef enum nodetype_t {                 //枚举标签类型
    Stag_start,
    Etag_start,
    PI_start,
    Content,
    CDSECT_start,
    COMMENT_start
} nodetype;

typedef struct node_t {                   //标签节点的数据结构
    int offset;                         //相对于该数据块的偏移
    int taglen;                         //该标签的长度（计算结束位置）
    nodetype bt;                         //该标签的类型
    struct node_t *next;
} node;

typedef struct bufferarry_t {            //缓冲区块的数据结构
    char buf[BUFLEN + APPEND];          //存放读入的数据
    node *nodearr;                        //识别标签后存放结果的链表
    int bufnum;                         //缓冲块块号
    int buflen;                         //缓冲区所存数据长度
    struct bufferarry_t *next;          //下一个缓冲区块指针
} buffblock;

//第二阶段用到的结构体
typedef struct lable_t {                 //栈内数据的数据结构
    char tagname[NAMELEN];
    nodetype bt;
} lable;

typedef struct stack_t {                 //栈的数据结构
    int top;                            //栈的指针
    lable *data[STACKSIZE];             //栈的数据(标签名字、标签类型)
} stack;

//各阶段的函数声明

//打印相应标签类型
char *printEnum(nodetype type);                                                      //main.c


//申请缓冲区并初始化
buffblock *mallocBuffer();                                                        //step1.c
//字符串比较
int strCmp(char *str, int start, int len, char str2[]);                                //step1.c
//创建标记信息链表
node *nodenode(nodetype type, int offset, int taglen);                                              //step1.c
//分析数据块，找到个标签的位置并存入链表中
buffblock *storeBlock(buffblock *block, int blocknum, int buflen);            //step1.c

//创建一个栈数据结点并赋值
lable *creatData(char *tagName, nodetype bt);                        //step2.c
//创建并初始化栈
stack *creatStack();                                                //step2.c
//入栈操作
int Push(stack *st, lable *node);                                   //step2.c
//出栈操作(出栈后记得Free释放掉)
lable *Pop(stack *st);                                              //step2.c
//提取名字
char *getname(char *strarr, char *str, char ch);                    //step2.c
//提取标签名
char *getTagName(char *strarr, char *str, nodetype bt);              //step2.c
//与栈顶对比，判断进栈或出栈
void stackInOut(buffblock *block, char *loc, nodetype bt, stack *st, node *r);          //step2.c
//每个缓存块内的栈匹配
void stackMatching(buffblock *block);                             //step2.c
//线程调用函数
void * firstcheck(void *arg);                                             //step2.c

int finalcheck(buffblock *block_head);                                          //step3.c

#endif // MYXML_H_INCLUDED
