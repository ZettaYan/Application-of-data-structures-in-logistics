#include "stdio.h"
#include "string.h"
#include "stdlib.h"


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


/*判断输入的字符串是否全为数字*/
int is_num(char *str)
{
    int i = 0;
    int len = strlen(str);
    for(; i < len; i++)
    {
        if(*(str + i) < '0' || *(str + i) > '9')
        {
            return ERROR;
        }
    }
    return OK;
}


/*打印CSV文件内容*/
void view_file(char *filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open this file.\n");
    }
    while (!feof(fp))
        putchar(fgetc(fp));
    fclose(fp);
}


/*创建书目链表的头节点*/
DuLinkList InitList()
{
    DuLinkList H;
    H = (DuLNode *)malloc(sizeof(DuLNode));
    if (!H)
    {
        printf("Book list initialization false!\n");
        return ERROR;
    }
    H -> next = NULL;
    printf("Book list initialization success!\n");
    return H;
}


/*将文件指针移动到下一行*/
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


/*导入csv文件中的书目信息，并创建书目的链表*/
int create_list(DuLinkList BookList, char *filename)
{
    FILE *fp;
    DuLNode *p = NULL, *q = NULL;
    char temp[50], *t = temp;
    int i = 0;
    p = BookList;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open this file.\n");
    }
    moveToNextLine(fp);
    printf("编号\t名称\t作者\t出版社\t出版年份\n");
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
                        printf("%d\t%s\t%s\t%s\t%s\n",q -> id, q -> name, q -> author, q -> publisher, q -> year);
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


/*在位置i插入新书目及其基本信息*/
int ListInsert(DuLinkList L, int i)
{
    DuLNode *p = L, *s = NULL;
    int j = 0;
    char temp[50];
    while(p && j < i - 1)
    {
        p = p -> next;
        j++;
    }
    if(!p || j > i - 1)
    {
        printf("Insert place error!");
        return OVERFLOW;
    }
    s = (DuLNode *)malloc(sizeof(DuLNode));
    s -> id = i;
    printf("名称："); scanf("%s", temp); strcpy(s -> name, temp);
    printf("作者："); scanf("%s", temp); strcpy(s -> author, temp);
    printf("出版社："); scanf("%s", temp); strcpy(s -> publisher, temp);
    printf("出版年份："); scanf("%s", temp); strcpy(s -> year, temp);
    while(!is_num(s -> year))
    {
        printf("年份输入有误！请重新输入："); scanf("%s", temp);
        strcpy(s -> year, temp);
    }
    s -> next = p -> next;
    p -> next = s;
    p = p -> next;
    /*修改后面的节点的id号*/
    while(p -> next)
    {
        p = p -> next;
        p -> id += 1;
    }
    return OK;
}


/*在链表末尾新增书目及其基本信息*/
void ListAdd(DuLinkList L)
{
    DuLNode *p = L, *s = NULL;
    int j = 0;
    char temp[50];
    while(p -> next)
    {
        p = p -> next;
        j++;
    }
    s = (DuLNode *)malloc(sizeof(DuLNode));
    s -> id = j + 1;
    printf("名称："); scanf("%s", temp); strcpy(s -> name, temp);
    printf("作者："); scanf("%s", temp); strcpy(s -> author, temp);
    printf("出版社："); scanf("%s", temp); strcpy(s -> publisher, temp);
    printf("出版年份："); scanf("%s", temp); strcpy(s -> year, temp);
    while(!is_num(s -> year))
    {
        printf("年份输入有误！请重新输入："); scanf("%s", temp);
        strcpy(s -> year, temp);
    }
    p -> next = s;
    s -> next = NULL;
}


/*删除给定编号的书目及其信息*/
int ListDelete(DuLinkList L, int i)
{
    DuLNode *p = L, *q =NULL;
    int j = 0;
    while(p && j < i - 1)
    {
        p = p -> next;
        j++;
    }
    if(!p || !(p -> next) || j > i - 1)
    {
        printf("Not found this record!");
        return OVERFLOW;
    }
    q = p -> next;
    p -> next = q -> next;
    free(q);
    while(p -> next)
    {
        p = p -> next;
        p -> id -= 1;
    }
    return OK;
}


/*修改给定编号的书目信息*/
int RectifyList(DuLinkList L, int i)
{
    DuLNode *p = L;
    int j = 0;
    char temp[50];
    while(p && j < i)
    {
        p = p -> next;
        j++;
    }
    while(!p || j > i || i == 0)
    {
        printf("Not found this record!");
        return OVERFLOW;
    }
    printf("名称："); scanf("%s", temp); strcpy(p -> name, temp);
    printf("作者："); scanf("%s", temp); strcpy(p -> author, temp);
    printf("出版社："); scanf("%s", temp); strcpy(p -> publisher, temp);
    printf("出版年份："); scanf("%s", temp); strcpy(p -> year, temp);
    while(!is_num(p -> year))
    {
        printf("年份输入有误！请重新输入："); scanf("%s", temp);
        strcpy(p -> year, temp);
    }
    return OK;
}


/*输出最新出版的书目及其信息*/
void LatestBook(DuLinkList L)
{
    int latest_year = 0;
    int num = 1; //用于表示是否有多哥相同年份的最新书信息
    DuLNode *p = L -> next, *s = NULL;
    while(p)
    {
        if(atoi(p -> year) > latest_year)
        {
            latest_year = atoi(p -> year);
            s = p;
            num = 1;
        }
        else if(atoi(p -> year) == latest_year)
        {
            num += 1;
        }
        p = p -> next;
    }
    printf("---------最新出版的书目信息如下---------\n");
    printf("编号\t名称\t作者\t出版社\t出版年份\n");
    while(num)
    {
        if(atoi(s -> year) == latest_year)
        {
            printf("%d\t%s\t%s\t%s\t%s\n",s -> id, s -> name, s -> author, s -> publisher, s -> year);
            num -= 1;
            s = s -> next;
        }
        else
        {
            s = s -> next;
        }
    }
}

/*打印当前链表中的所有信息*/
void PrintList(DuLinkList L)
{
    DuLNode *p = L -> next;
    printf("编号\t名称\t作者\t出版社\t出版年份\n");
    while(p)
    {
        printf("%d\t%s\t%s\t%s\t%s\n",p -> id, p -> name, p -> author, p -> publisher, p -> year);
        p = p -> next;
    }
}


/*输出函数*/
void Output(DuLinkList L)
{
    DuLNode *p = L -> next;
    FILE *fp;
    char filename[] = { "Output.csv" };
    char temp[50];
    if ((fp = fopen(filename,"w")) == NULL)
    {
        printf("Can't open the output file.\n");
    }
    fwrite("编号,名称,作者,出版社,出版年份\n",strlen("编号,名称,作者,出版社,出版年份\n"),1,fp);
    while(p)
    {
        sprintf(temp, " %d" , p -> id);
        fwrite(temp + 1, strlen(temp + 1),1, fp);
        fwrite(",",1,1,fp);
        fwrite(p -> name, strlen(p -> name), 1, fp);
        fwrite(",",1,1,fp);
        fwrite(p -> author, strlen(p -> author), 1, fp);
        fwrite(",",1,1,fp);
        fwrite(p -> publisher, strlen(p -> publisher), 1, fp);
        fwrite(",",1,1,fp);
        fwrite(p -> year, strlen(p -> year), 1, fp);
        fwrite("\n",1,1,fp);
        p = p -> next;
    }
    printf("修改后的信息已保存到本地文件\"Output.csv\"中\n");
}


/*释放链表的空间*/
void ClearList(DuLinkList *L)
{
    DuLNode *p = NULL;
    while((*L) -> next)
    {
        p = (*L) -> next;
        (*L) -> next = p -> next;
        free(p);
    }
    free(*L);
}


int main()
{
    char datafile[] = { "Appendix_Recommended_Bibliography_ANSI.csv" };
    int flag = 1;
    int i;
    DuLinkList BookList = NULL;
    BookList = InitList(); //初始化链表
    create_list(BookList, datafile); //将CSV文件中的数据导入链表中
    while(flag)
    {
        printf("\nPress '1' Add a record\nPress '2' Delete a record");
        printf("\nPress '3' Rectify a record\nPress '4' The latest book");
        printf("\nPress '5' Insert a record\nPress '6' View the book list");
        printf("\nPress '7' Output the book list");
        printf("\nPress '0' for exit and any key to return\nInput:");
        switch (getchar())
        {
            //case '1': view_file(datafile); break;
            case '1': ListAdd(BookList); break;
            case '2': printf("The record you want to delete:"); scanf("%d", &i); ListDelete(BookList, i); break;
            case '3': printf("The record you want to rectify: "); scanf("%d", &i); RectifyList(BookList, i); break;
            case '4': LatestBook(BookList); break;
            case '5': printf("The place you want to insert: "); scanf("%d", &i); ListInsert(BookList, i); break;
            case '6': PrintList(BookList); break;
            case '7': Output(BookList); break;
            case '0': ClearList(&BookList); flag = 0; puts("\n");
        }
        getchar();//digest
    }
    return 0;
}
