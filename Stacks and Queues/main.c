#include "stdio.h"
#include "string.h"
#include "stdlib.h"		//For random_example.
#include "time.h"		//For random_example.
#include "windows.h"	//For timer_countdown.

# define OK 1
# define ERROR 0
# define OVERFLOW -2

/*定义书目结构体以及结构体指针*/
typedef struct
{
    int id;
    char name[50], author[20], publisher[50], year[20];
    struct DuLNode *next;
}DuLNode, *DuLinkList;


/*定义原材料/产品的结构体类型*/
typedef struct
{
    int id;
    char name[50], author[20], publisher[50], year[20];
}Product;


/*实验1中的函数*/
//创建书目链表的头节点
DuLinkList InitList()
{
    DuLinkList H;
    H = (DuLNode *)malloc(sizeof(DuLNode));
    if (!H)
    {
        //printf("Book list initialization false!\n");
        return ERROR;
    }
    H -> next = NULL;
    //printf("Book list initialization success!\n");
    return H;
}

//将文件指针移动到下一行
int moveToNextLine(FILE *fp)
{
    int c;
    if(fp == NULL) return -1;//文件指针非法
    while(1)
    {
        c = fgetc(fp);
        if(c == EOF) return EOF;//到文件尾，不存在下一行
        if(c == '\n') break;//读到需要的换行符。
    }
    return OK;
}

//导入csv文件中的书目信息，并创建书目的链表
int create_list(DuLinkList BookList, char *filename)
{
    FILE *fp;
	char temp[50], *t = temp;
    int i = 0;
    DuLNode *p = NULL, *q = NULL;
    p = BookList;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open this file.\n");
    }
    moveToNextLine(fp);
    //printf("编号\t名称\t作者\t出版社\t出版年份\n");
    while (!feof(fp))
    {
        q = (DuLNode *)malloc(sizeof(DuLNode));
        q -> next = NULL;
        while (!feof(fp))
        {
            int flag = 0;
            *t = fgetc(fp);
            if(*t == ',' || *t == '\n')
            {
                *t = '\0';
                switch (i)
                {
                    case 0: q -> id = atoi(temp); i++; break;
                    case 1: strcpy(q -> name, temp); i++; break;
                    case 2: strcpy(q -> author, temp); i++; break;
                    case 3: strcpy(q -> publisher, temp); i++; break;
                    case 4: strcpy(q -> year, temp);
                        i = 0;
                        flag = 1;
                        //printf("%d\t%s\t%s\t%s\t%s\n",q -> id, q -> name, q -> author, q -> publisher, q -> year);
                        p -> next = q;
                        p = q;
                }
                t = temp;
                if(flag) break;
            }
            else
            {
                t++;
            }
        }
    }
    fclose(fp);
    return OK;
}

//释放链表的空间
void ClearList(DuLinkList L)
{
    DuLNode *p = NULL;
    while(L -> next)
    {
        p = L -> next;
        L -> next = p -> next;
        free(p);
    }
    free(L);
}


/*构建一个等待队列(传送带)的数据结构*/
//定义队列最大容量
#define QUERY_SIZE 11

//定义循环队列结构体
typedef struct
{
    Product *base;
    int front;
    int rear;
}SqQueue;

//初始化一个空的循环队列
int InitQueue (SqQueue *Qp)
{
    Qp -> base = (Product *) malloc(QUERY_SIZE * sizeof(Product));
    if(!Qp -> base) exit(OVERFLOW);
    Qp -> front = Qp -> rear=0;
    return OK;
}

//销毁队列
int DestroyQueue (SqQueue *Qp)
{
    if (Qp -> base)
    {
        free(Qp->base);
        return OK;
    }
    else
        return ERROR;
    //Qp -> base = NULL;
    //Qp -> front = Qp -> rear = 0;
}

//清空队列
void ClearQueue (SqQueue *Qp)
{
    Qp -> front = 0;
    Qp -> rear = 0;
}

//求队列长度
int QueueLength (SqQueue Q)
{
    return (Q.rear-Q.front + QUERY_SIZE) % QUERY_SIZE;
}

//入队操作
int EnQueue (SqQueue *Qp, Product e)
{
    if((Qp -> rear + 1) % QUERY_SIZE == Qp -> front) return ERROR;
    Qp -> base[Qp -> rear] = e;
    Qp -> rear = (Qp -> rear + 1) % QUERY_SIZE;
    return OK;
}

//出队操作
int DeQueue (SqQueue *Qp, Product *ep)
{
    if(Qp -> front == Qp -> rear)
        return ERROR;
    *ep = Qp -> base[Qp -> front];
    Qp -> front = (Qp -> front + 1) % QUERY_SIZE;
    return OK;
}


/*构建一个栈(即码放产成品的堆货)的数据结构*/
//定义堆货的最大容量
#define STACK_SIZE 20

//定义栈的结构
typedef struct
{
    Product *base;
    Product *top;
}SqStack;

//初始化，构造一个空栈
int InitStack(SqStack *Sp)
{
    Sp -> base = (Product *) malloc(STACK_SIZE * sizeof(Product));
    if(!Sp -> base)
        return ERROR;
    Sp -> top = Sp -> base;
    return OK;
}

//销毁栈
int DestroyStack(SqStack *Sp)
{
    if (Sp -> base)
    {
        free(Sp->base);
        return OK;
    }
    else
        return ERROR;
}

//清空栈
void ClearStack(SqStack *Sp)
{
    Sp->top = Sp ->base;
}

//求栈的长度
int StackLength(SqStack S)
{
    return (S.top - S.base);
}

//判栈空
int StackEmpty(SqStack S)
{
    return S.top == S.base;
}

//判栈满
int StackFull(SqStack S)
{
    return ((S.top -S.base) == STACK_SIZE);
}

//读取栈顶元素
int GetTop(SqStack S, Product *ep)
{
    if(S.top == S.base)
    {
        printf("Stack is empty!\n");
        return ERROR;
    }
    *ep = *(S.top-1);
    return OK;
}

//入栈
int Push(SqStack *Sp, Product e)
{
    if(Sp -> top - Sp -> base >= STACK_SIZE)
    {
        printf("Stack is full!\n");
        return ERROR;
    }
    *Sp -> top++ = e;
    return  OK;
}

//出栈
int Pop(SqStack *Sp, Product *e)
{
    if(Sp -> top == Sp -> base)
    {
        printf("Stack is empty!\n");
        return ERROR;
    }
    *e = *(--Sp -> top);
    return OK;
}


/*其他函数*/
//工作台相关函数
int Work_Efficiency ()
{
    int x;
    printf("请输入一个整数，用于表示工作台的工作效率(单位：件/秒)：");
    scanf("%d", &x);
    return x;
}

//输入仿真的总工作时长
int Work_Time()
{
    int tick = 0;
    printf("请输入仿真的总工作时长(单位：秒): ");
    scanf("%d", &tick);
    return tick;
}

//定义产生随机数的函数
int random_between_a_and_b(int a, int b)
{
    int result = 0;
    srand((unsigned)time(NULL));
    result = rand() % (b - a + 1) + a;
    return result;
}

//输出函数
void OutPut_CSV(int flag, int time, int material_arrive, int product_move, int product_out, int queue_length, int stack_length)
{
    FILE *fp;
    char filename[] = { "Output.csv" };
    if ((fp = fopen(filename,"a")) == NULL)
    {
        printf("Can't open the output file.\n");
    }
    if (flag == 1)
        fwrite("秒数,传入,转移,发货,传送带,堆货区\n",strlen("秒数,传入,转移,发货,传送带,堆货区\n"),1,fp);
    fprintf(fp, "%d", time); fwrite(",",1,1,fp);
    fprintf(fp, "%d", material_arrive); fwrite(",",1,1,fp);
    fprintf(fp, "%d", product_move); fwrite(",",1,1,fp);
    fprintf(fp, "%d", product_out); fwrite(",",1,1,fp);
    fprintf(fp, "%d", queue_length); fwrite(",",1,1,fp);
    fprintf(fp, "%d", stack_length); fwrite("\n",1,1,fp);
}

/*仿真模拟函数*/
void simulation(SqQueue *Transporter_p, SqStack *Stack_p, DuLinkList WorkList)
{
    int MaterialLow  = 3, MaterialTop = 5, ProductOutLow = 0, ProductOutTop =4; //定义原料到达的速度以及产品发货量的上下限的上下限
    int work_efficiency = 0;    //定义工作台工作效率变量
    int work_time = 0;  //定义仿真总时长变量
    int tick = 0;   //定义计时器变量
    int material_arrive = 0, product_move = 0, product_out= 0;  //定义原材料到达量、产成品移动量、产成品发货量
	int i = 0;  //循环控制变量
    int flag = 1;   //标注是否为首次输出
    DuLNode *work_p = WorkList -> next; //定义一个工作指针，用于不断指向WorkList的后一个元素
    Product current_product; //定义一个变量，每次传入新的原材料时候，将链表节点结构结构的原材料信息转换成Product结构的信息

    /*调用函数为工作台工作效率变量赋值*/
    work_efficiency = Work_Efficiency();


    /*调用函数，为仿真总时长变量赋值*/
    work_time = Work_Time();
    tick = work_time;

    /*开始仿真*/
    while (tick >= 1)
    {
        Sleep(1000);
        material_arrive = random_between_a_and_b(MaterialLow, MaterialTop);
        product_move = work_efficiency;
        product_out = random_between_a_and_b(ProductOutLow, ProductOutTop);

        /*判断传送带上是否有足够多的空位,如果没有则只传入空位数量的原材料*/
        if(material_arrive > QUERY_SIZE - 1 - QueueLength(*Transporter_p))
            material_arrive = QUERY_SIZE - 1 - QueueLength(*Transporter_p);
		/*判断传送带上是否有足够多的能转移的产品，如果没有则只转移现有的产成品数量*/
        if(product_move > QueueLength(*Transporter_p))
            product_move = QueueLength(*Transporter_p);
        /*判断堆货区是否有足够多的空位，如果没有则只传入空位数量的产成品*/
        if(product_move > STACK_SIZE - StackLength(*Stack_p))
            product_move = STACK_SIZE - StackLength(*Stack_p);
        /*判断堆货区是否有足够多的能发货产品，如果没有则只发货现有数量的产品*/
        if(product_out > StackLength(*Stack_p))
            product_out = StackLength(*Stack_p);

        /*向传送带队列中插入material_arrive个元素*/
        for(i = 0; i < material_arrive; i++)
        {
            current_product.id = work_p -> id;
            strcpy(current_product.name,work_p -> name);
            strcpy(current_product.author,work_p -> author);
            strcpy(current_product.publisher,work_p -> publisher);
            strcpy(current_product.year,work_p -> year);
            EnQueue(Transporter_p, current_product);
            work_p = work_p -> next;
        }
        /*从传送带队列中弹出product_move个元素，移动到堆货区*/
        for(i = 0; i < product_move; i++)
        {
            DeQueue(Transporter_p, &current_product);
            Push(Stack_p, current_product);
        }
        /*从堆货区弹出product_out个元素*/
        for(i = 0; i < product_out; i++)
        {
            Pop(Stack_p, &current_product);
        }
        /*打印这一秒工作传输后传送带和堆货区的留存件数*/
		printf("-----------------------------------第%d秒-----------------------------------\n", work_time - tick + 1);
        printf("传入:%d件\t转移:%d件\t发货:%d件\t传送带:%d件\t堆货区:%d件\n", material_arrive, product_move, product_out, QueueLength(*Transporter_p), StackLength(*Stack_p));
        OutPut_CSV(flag, work_time - tick + 1, material_arrive, product_move, product_out, QueueLength(*Transporter_p), StackLength(*Stack_p));
        tick -= 1;
        flag = 0;
    }
}


int main()
{
    char datafile[] = { "Appendix_Recommended_Bibliography_ANSI.csv" };
    int flag = 1;
    DuLinkList WorkList; //定义生产链表的首地址
    SqQueue Transporter;    //定义传送带队列变量
    SqStack Stack;  //定义堆货区变量

    /*初始化生产链表*/
    WorkList = InitList();
    /*将CSV文件中的数据导入链表中*/
    create_list(WorkList, datafile);

    /*初始化传送带变量和堆货区变量*/
    InitQueue(&Transporter);
    InitStack(&Stack);

    /*进行仿真*/
    while(flag)
    {
        printf("\nPress '1' simulation and output result to csv\nPress '0' for exit and any key to return\nInput: ");
        switch (getchar())
        {
            case '1': simulation(&Transporter, &Stack, WorkList); ClearQueue(&Transporter); ClearStack(&Stack); break;
            case '0': flag = 0; DestroyQueue(&Transporter); DestroyStack(&Stack); ClearList(WorkList); puts("\n");		//Exit.
        }
        getchar();	//Digest.
    }
    return 0;
}