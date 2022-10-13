#include <stdio.h>
#include <stdlib.h>
#include <string.h>


# define OK 1
# define ERROR 0
# define FINISH 0

typedef struct{
    int top_grade;
    int bottom_grade;
    unsigned int weight;    /*用来存放各个结点的权值*/
    unsigned int parent, lchild, rchild;    /*指向双亲、孩子结点的指针*/
}HTNode, *HuffmanTree;


typedef struct GradeArea{
    int top_grade;
    int bottom_grade;
    int weight;
    struct GradeArea *next;
}GradeArea, *GradeList;


typedef char **HuffmanCode; /*动态分配数组存储哈夫曼编码*/

/*函数申明*/
GradeList InitList();
int moveToNextLine(FILE *);
int create_list(GradeList, char *, int *);
int selectht(HuffmanTree, int, int *, int *);
void CrtHuffmanTree(HuffmanTree *, HuffmanCode *, GradeList, int);
int visit (HuffmanTree, unsigned int);
int Preorder (HuffmanTree, unsigned int, int (*visit) (HuffmanTree, unsigned int));
int locate(HuffmanTree, unsigned int, int);
void score_determine(HuffmanTree, unsigned int);


int main() {
    int n = 0;  /*定义变量存储权值个数*/
    char datafile[] = { "Fractional_segments_and_weights.csv" };
    HuffmanTree ht = (HuffmanTree) NULL;
    HuffmanCode hcode = (HuffmanCode) NULL;
    GradeList grade_list = (GradeList) NULL;
    grade_list = InitList();    /*初始化链表*/
    create_list(grade_list, datafile, &n); /*将CSV文件中的数据导入链表中*/
    CrtHuffmanTree(&ht, &hcode, grade_list, n); /*创建哈夫曼树*/
    printf("\n-------------先序遍历-------------\n");
    Preorder(ht, 2 * n - 1, visit); /*先序遍历哈夫曼树并打印输出*/
    /*分数段判定程序*/
    printf("\n-------------分数判定-------------\n");
    score_determine(ht, n);
    /*释放哈夫曼树空间*/
    //Preorder(ht, 2 * n - 1, free_HuffmanTree);
    free(ht);
    return 0;
}


/*创建分数段及权重链表的头节点*/
GradeList InitList()
{
    GradeList H;
    H = (GradeArea *)malloc(sizeof(GradeArea));
    if (!H)
    {
        printf("Grade list initialization false!\n");
        return ERROR;
    }
    H -> next = NULL;
    printf("Grade list initialization success!\n");
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


/*导入csv文件中的分数段及权重信息，并创建分数段及权重的链表*/
int create_list(GradeList grade_list, char *filename, int *n)
{
    FILE *fp;
    GradeArea *p = NULL, *q = NULL;
    char temp[50], *t = temp;
    int i = 0;
    p = grade_list;
    *n = 0;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open this file.\n");
    }
    moveToNextLine(fp);
    printf("下界\t上界\t权重\n");
    while (!feof(fp))
    {
        q = (GradeArea *)malloc(sizeof(GradeArea));
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
                    case 0: q -> bottom_grade = atoi(temp); i++; break;
                    case 1: q -> top_grade = atoi(temp); i++; break;
                    case 2: q -> weight = (int) (atof(temp) * 100);
                        i = 0;
                        flag = 1;
                        *n += 1;
                        printf("%3d\t%3d\t%3d\t\n",q -> bottom_grade, q -> top_grade, q -> weight);
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


/*选择两个parent为0且weight最小的结点*/
int selectht(HuffmanTree ht, int n, int *s1, int *s2)
{
    int i;
    for(i = 1; i <= n; i++)
    {
        if ((ht + i) -> parent != 0)
            continue;
        else
        {
            *s1 = i++;
            break;
        }
    }
    for(; i <= n; i++)
    {
        if ((ht + i) -> parent != 0)
            continue;
        else
        {
            *s2 = i++;
            break;
        }
    }
    if ((ht + *s1) -> weight > (ht + *s2) -> weight)
    {
        *s1 += *s2;
        *s2 = *s1 - *s2;
        *s1 -= *s2;
    }
    for(; i <= n; i++)
    {
        if((ht + i) -> parent != 0)
            continue;
        if((ht + i) -> weight >= (ht + *s2) -> weight)
            continue;
        else if((ht + i) -> weight >= (ht + *s1) -> weight && (ht + i) -> weight < (ht + *s2) -> weight)
            *s2 = i;
        else if((ht + i) -> weight < (ht + *s1) -> weight)
        {
            *s2 = *s1;
            *s1 = i;
        }
        else
        {
            printf("ERROR");
            return ERROR;
        }
    }
    return OK;
}


/*创建哈夫曼树*/
void CrtHuffmanTree(HuffmanTree *ht, HuffmanCode *hcode, GradeList w, int n)
{
    /*w存放n个权值, 构造哈夫曼树ht, 并求出哈夫曼编码hc*/
    int m;  /*定义表示节点总数的变量*/
    int i;  /*定义循环控制变量*/
    char *cd;   /*定义字符串指针，用于储存新生成的哈夫曼编码*/
    int start;  /*定义逆向循环控制变量*/
    unsigned int c = 0;
    unsigned int p;
    GradeArea *gp = w -> next;
    if (n <= 1)
        return;
    m = 2 * n - 1;
    *ht = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));    /*0号单元未使用*/
    (*ht) -> bottom_grade = -1;
    (*ht) -> top_grade = -1;
    (*ht) -> weight = 0;
    (*ht) -> parent = 0;
    (*ht) -> lchild = 0;
    (*ht) -> rchild = 0;
    for(i = 1; i <= n; i++) /*叶子结点初始化*/
    {
        (*ht + i) -> bottom_grade = gp -> bottom_grade;
        (*ht + i) -> top_grade = gp -> top_grade;
        (*ht + i) -> weight = gp -> weight;
        (*ht + i) -> parent = 0;
        (*ht + i) -> lchild = 0;
        (*ht + i) -> rchild = 0;
        gp = gp -> next;
    }
    for(i = n + 1; i <= m; i++) /*非叶子结点初始化*/
    {
        (*ht + i) -> bottom_grade = -1;
        (*ht + i) -> top_grade = -1;
        (*ht + i) -> weight = 0;
        (*ht + i) -> parent = 0;
        (*ht + i) -> lchild = 0;
        (*ht + i) -> rchild = 0;
    }
    for(i = n + 1; i <= m; i++) /*创建非叶子结点, 建哈夫曼树*/
    {
        /*在ht[1]~ht[i-1]的范围内选择两个parent为0且weight最小的结点，其序号分别赋值给s1、s2返回*/
        int s1, s2;
        selectht(*ht, i-1, &s1, &s2);
        (*ht + s1) -> parent = i;
        (*ht + s2) -> parent = i;
        (*ht + i) -> lchild = s1;
        (*ht + i) -> rchild = s2;
        (*ht + i) -> weight = (*ht + s1) -> weight + (*ht + s2) -> weight;
    }/*哈夫曼树建立完毕*/
    /*从叶子结点到根，逆向求每个叶子结点对应的哈夫曼编码*/
    *hcode = (HuffmanCode)malloc((n + 1) * sizeof(char *)); /*分配n个编码的头指针*/
    cd = (char *)malloc(n * sizeof(char));  /*分配求当前编码的工作空间*/
    cd[n - 1]='\0'; /*从右向左逐位存放编码，首先存放编码结束符*/
    for(i = 1; i <= n; i++)
    {
        start = n - 1;   /*初始化编码起始指针*/
        for(c = i, p = (*ht + i) -> parent; p != 0; c = p, p = (*ht + p) -> parent) /*从叶子到根结点求编码*/
        {
            if((*ht + p) -> lchild == c) cd[--start] = '0'; /*左分支标0*/
            else cd[--start] = '1'; /*右分支标1*/
        }
        (*hcode)[i] = (char *)malloc((n-start) * sizeof(char)); /*为第i个编码分配空间*/
        strcpy((*hcode)[i], &cd[start]);
    }
    free(cd);
}


/*访问并输出节点信息*/
int visit (HuffmanTree ht, unsigned int i)
{
    if (i == 0)
        return OK;
    if ((ht + i) -> bottom_grade >= 0 && (ht + i) -> top_grade <= 100)
    {
        if ((ht + i) -> top_grade == 100)
        {
            printf("节点%d的权重为:%d，对应分数段[%d,%d]\n", i, (ht + i) -> weight, (ht + i) -> bottom_grade, (ht + i) -> top_grade);
            return OK;
        }
        else
        {
            printf("节点%d的权重为:%d，对应分数段[%d,%d)\n", i, (ht + i) -> weight, (ht + i) -> bottom_grade, (ht + i) -> top_grade);
            return OK;
        }
    }
    else if ((ht + i) -> bottom_grade == -1 && (ht + i) -> top_grade == -1)
    {
        printf("节点%d的权重为:%d\n", i, (ht + i) -> weight);
        return OK;
    }
    else
        return ERROR;
}


/*先序遍历哈夫曼树*/
int Preorder (HuffmanTree ht, unsigned int i, int (*visit) (HuffmanTree, unsigned int))
{
    /*先序遍历以ht为根指针的二叉树*/
    if (i)
    {
        if (visit(ht, i)) // 通过函数指针 *visit 访问根结点
            if (Preorder(ht, (ht + i) -> lchild, visit)) // 先序遍历左子树
                if (Preorder(ht, (ht + i) -> rchild, visit)) // 先序遍历右子树
                    return OK;
        return ERROR;
    }
    else
        return OK;
}


/*分数判定*/
void score_determine(HuffmanTree ht, unsigned int n)
{
    int flag = 1;
    int score;
    while (flag)
    {
        printf("请输入一个成绩(输入-1结束程序):");
        scanf("%d", &score);
        if (score == -1)
        {
            flag = 0;
            break;
        }
        else if (score > 100 || score < 0)
            printf("输入错误！输入的成绩需要在区间[0, 100]中！\n");
        else
            locate(ht, 2 * n - 1, score);
    }
}


/*分数定位*/
int locate(HuffmanTree ht, unsigned int i, int score)
{
    if (i)
    {
        if ((ht + i) -> lchild == 0 && (ht + i) -> rchild == 0)
        {
            if ((ht + i) -> top_grade == 100)
            {
                if (score >= (ht + i) -> bottom_grade && score <= (ht + i) -> top_grade)
                {
                    printf("位于分数段[%d,%d]\n", (ht + i) -> bottom_grade, (ht + i) -> top_grade);
                    return OK;
                }
                else
                    return FINISH;  /*如果没有匹配到，则这一分支遍历完毕*/
            }
            else
            if (score >= (ht + i) -> bottom_grade && score < (ht + i) -> top_grade)
            {
                printf("位于分数段[%d,%d)\n", (ht + i) -> bottom_grade, (ht + i) -> top_grade);
                return OK;
            }
            else
                return FINISH;  /*如果没有匹配到，则这一分支遍历完毕*/
        }
        else
        {
            if(locate(ht, (ht + i) -> lchild, score))
                return OK;
            if(locate(ht, (ht + i) -> rchild, score))
                return OK;
        }
        return FINISH;  /*如果没有匹配到，则这一分支遍历完毕*/
    }
    return ERROR;
}