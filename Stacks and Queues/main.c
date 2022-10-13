#include "stdio.h"
#include "string.h"
#include "stdlib.h"		//For random_example.
#include "time.h"		//For random_example.
#include "windows.h"	//For timer_countdown.

# define OK 1
# define ERROR 0
# define OVERFLOW -2

/*������Ŀ�ṹ���Լ��ṹ��ָ��*/
typedef struct
{
    int id;
    char name[50], author[20], publisher[50], year[20];
    struct DuLNode *next;
}DuLNode, *DuLinkList;


/*����ԭ����/��Ʒ�Ľṹ������*/
typedef struct
{
    int id;
    char name[50], author[20], publisher[50], year[20];
}Product;


/*ʵ��1�еĺ���*/
//������Ŀ�����ͷ�ڵ�
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

//���ļ�ָ���ƶ�����һ��
int moveToNextLine(FILE *fp)
{
    int c;
    if(fp == NULL) return -1;//�ļ�ָ��Ƿ�
    while(1)
    {
        c = fgetc(fp);
        if(c == EOF) return EOF;//���ļ�β����������һ��
        if(c == '\n') break;//������Ҫ�Ļ��з���
    }
    return OK;
}

//����csv�ļ��е���Ŀ��Ϣ����������Ŀ������
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
    //printf("���\t����\t����\t������\t�������\n");
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

//�ͷ�����Ŀռ�
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


/*����һ���ȴ�����(���ʹ�)�����ݽṹ*/
//��������������
#define QUERY_SIZE 11

//����ѭ�����нṹ��
typedef struct
{
    Product *base;
    int front;
    int rear;
}SqQueue;

//��ʼ��һ���յ�ѭ������
int InitQueue (SqQueue *Qp)
{
    Qp -> base = (Product *) malloc(QUERY_SIZE * sizeof(Product));
    if(!Qp -> base) exit(OVERFLOW);
    Qp -> front = Qp -> rear=0;
    return OK;
}

//���ٶ���
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

//��ն���
void ClearQueue (SqQueue *Qp)
{
    Qp -> front = 0;
    Qp -> rear = 0;
}

//����г���
int QueueLength (SqQueue Q)
{
    return (Q.rear-Q.front + QUERY_SIZE) % QUERY_SIZE;
}

//��Ӳ���
int EnQueue (SqQueue *Qp, Product e)
{
    if((Qp -> rear + 1) % QUERY_SIZE == Qp -> front) return ERROR;
    Qp -> base[Qp -> rear] = e;
    Qp -> rear = (Qp -> rear + 1) % QUERY_SIZE;
    return OK;
}

//���Ӳ���
int DeQueue (SqQueue *Qp, Product *ep)
{
    if(Qp -> front == Qp -> rear)
        return ERROR;
    *ep = Qp -> base[Qp -> front];
    Qp -> front = (Qp -> front + 1) % QUERY_SIZE;
    return OK;
}


/*����һ��ջ(����Ų���Ʒ�Ķѻ�)�����ݽṹ*/
//����ѻ����������
#define STACK_SIZE 20

//����ջ�Ľṹ
typedef struct
{
    Product *base;
    Product *top;
}SqStack;

//��ʼ��������һ����ջ
int InitStack(SqStack *Sp)
{
    Sp -> base = (Product *) malloc(STACK_SIZE * sizeof(Product));
    if(!Sp -> base)
        return ERROR;
    Sp -> top = Sp -> base;
    return OK;
}

//����ջ
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

//���ջ
void ClearStack(SqStack *Sp)
{
    Sp->top = Sp ->base;
}

//��ջ�ĳ���
int StackLength(SqStack S)
{
    return (S.top - S.base);
}

//��ջ��
int StackEmpty(SqStack S)
{
    return S.top == S.base;
}

//��ջ��
int StackFull(SqStack S)
{
    return ((S.top -S.base) == STACK_SIZE);
}

//��ȡջ��Ԫ��
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

//��ջ
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

//��ջ
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


/*��������*/
//����̨��غ���
int Work_Efficiency ()
{
    int x;
    printf("������һ�����������ڱ�ʾ����̨�Ĺ���Ч��(��λ����/��)��");
    scanf("%d", &x);
    return x;
}

//���������ܹ���ʱ��
int Work_Time()
{
    int tick = 0;
    printf("�����������ܹ���ʱ��(��λ����): ");
    scanf("%d", &tick);
    return tick;
}

//�������������ĺ���
int random_between_a_and_b(int a, int b)
{
    int result = 0;
    srand((unsigned)time(NULL));
    result = rand() % (b - a + 1) + a;
    return result;
}

//�������
void OutPut_CSV(int flag, int time, int material_arrive, int product_move, int product_out, int queue_length, int stack_length)
{
    FILE *fp;
    char filename[] = { "Output.csv" };
    if ((fp = fopen(filename,"a")) == NULL)
    {
        printf("Can't open the output file.\n");
    }
    if (flag == 1)
        fwrite("����,����,ת��,����,���ʹ�,�ѻ���\n",strlen("����,����,ת��,����,���ʹ�,�ѻ���\n"),1,fp);
    fprintf(fp, "%d", time); fwrite(",",1,1,fp);
    fprintf(fp, "%d", material_arrive); fwrite(",",1,1,fp);
    fprintf(fp, "%d", product_move); fwrite(",",1,1,fp);
    fprintf(fp, "%d", product_out); fwrite(",",1,1,fp);
    fprintf(fp, "%d", queue_length); fwrite(",",1,1,fp);
    fprintf(fp, "%d", stack_length); fwrite("\n",1,1,fp);
}

/*����ģ�⺯��*/
void simulation(SqQueue *Transporter_p, SqStack *Stack_p, DuLinkList WorkList)
{
    int MaterialLow  = 3, MaterialTop = 5, ProductOutLow = 0, ProductOutTop =4; //����ԭ�ϵ�����ٶ��Լ���Ʒ�������������޵�������
    int work_efficiency = 0;    //���幤��̨����Ч�ʱ���
    int work_time = 0;  //���������ʱ������
    int tick = 0;   //�����ʱ������
    int material_arrive = 0, product_move = 0, product_out= 0;  //����ԭ���ϵ�����������Ʒ�ƶ���������Ʒ������
	int i = 0;  //ѭ�����Ʊ���
    int flag = 1;   //��ע�Ƿ�Ϊ�״����
    DuLNode *work_p = WorkList -> next; //����һ������ָ�룬���ڲ���ָ��WorkList�ĺ�һ��Ԫ��
    Product current_product; //����һ��������ÿ�δ����µ�ԭ����ʱ�򣬽�����ڵ�ṹ�ṹ��ԭ������Ϣת����Product�ṹ����Ϣ

    /*���ú���Ϊ����̨����Ч�ʱ�����ֵ*/
    work_efficiency = Work_Efficiency();


    /*���ú�����Ϊ������ʱ��������ֵ*/
    work_time = Work_Time();
    tick = work_time;

    /*��ʼ����*/
    while (tick >= 1)
    {
        Sleep(1000);
        material_arrive = random_between_a_and_b(MaterialLow, MaterialTop);
        product_move = work_efficiency;
        product_out = random_between_a_and_b(ProductOutLow, ProductOutTop);

        /*�жϴ��ʹ����Ƿ����㹻��Ŀ�λ,���û����ֻ�����λ������ԭ����*/
        if(material_arrive > QUERY_SIZE - 1 - QueueLength(*Transporter_p))
            material_arrive = QUERY_SIZE - 1 - QueueLength(*Transporter_p);
		/*�жϴ��ʹ����Ƿ����㹻�����ת�ƵĲ�Ʒ�����û����ֻת�����еĲ���Ʒ����*/
        if(product_move > QueueLength(*Transporter_p))
            product_move = QueueLength(*Transporter_p);
        /*�ж϶ѻ����Ƿ����㹻��Ŀ�λ�����û����ֻ�����λ�����Ĳ���Ʒ*/
        if(product_move > STACK_SIZE - StackLength(*Stack_p))
            product_move = STACK_SIZE - StackLength(*Stack_p);
        /*�ж϶ѻ����Ƿ����㹻����ܷ�����Ʒ�����û����ֻ�������������Ĳ�Ʒ*/
        if(product_out > StackLength(*Stack_p))
            product_out = StackLength(*Stack_p);

        /*���ʹ������в���material_arrive��Ԫ��*/
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
        /*�Ӵ��ʹ������е���product_move��Ԫ�أ��ƶ����ѻ���*/
        for(i = 0; i < product_move; i++)
        {
            DeQueue(Transporter_p, &current_product);
            Push(Stack_p, current_product);
        }
        /*�Ӷѻ�������product_out��Ԫ��*/
        for(i = 0; i < product_out; i++)
        {
            Pop(Stack_p, &current_product);
        }
        /*��ӡ��һ�빤��������ʹ��Ͷѻ������������*/
		printf("-----------------------------------��%d��-----------------------------------\n", work_time - tick + 1);
        printf("����:%d��\tת��:%d��\t����:%d��\t���ʹ�:%d��\t�ѻ���:%d��\n", material_arrive, product_move, product_out, QueueLength(*Transporter_p), StackLength(*Stack_p));
        OutPut_CSV(flag, work_time - tick + 1, material_arrive, product_move, product_out, QueueLength(*Transporter_p), StackLength(*Stack_p));
        tick -= 1;
        flag = 0;
    }
}


int main()
{
    char datafile[] = { "Appendix_Recommended_Bibliography_ANSI.csv" };
    int flag = 1;
    DuLinkList WorkList; //��������������׵�ַ
    SqQueue Transporter;    //���崫�ʹ����б���
    SqStack Stack;  //����ѻ�������

    /*��ʼ����������*/
    WorkList = InitList();
    /*��CSV�ļ��е����ݵ���������*/
    create_list(WorkList, datafile);

    /*��ʼ�����ʹ������Ͷѻ�������*/
    InitQueue(&Transporter);
    InitStack(&Stack);

    /*���з���*/
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