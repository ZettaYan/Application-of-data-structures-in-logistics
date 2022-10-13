#include <stdio.h>
#include <stdlib.h>
#include <string.h>


# define OK 1
# define ERROR 0
# define FINISH 0

typedef struct{
    int top_grade;
    int bottom_grade;
    unsigned int weight;    /*������Ÿ�������Ȩֵ*/
    unsigned int parent, lchild, rchild;    /*ָ��˫�ס����ӽ���ָ��*/
}HTNode, *HuffmanTree;


typedef struct GradeArea{
    int top_grade;
    int bottom_grade;
    int weight;
    struct GradeArea *next;
}GradeArea, *GradeList;


typedef char **HuffmanCode; /*��̬��������洢����������*/

/*��������*/
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
    int n = 0;  /*��������洢Ȩֵ����*/
    char datafile[] = { "Fractional_segments_and_weights.csv" };
    HuffmanTree ht = (HuffmanTree) NULL;
    HuffmanCode hcode = (HuffmanCode) NULL;
    GradeList grade_list = (GradeList) NULL;
    grade_list = InitList();    /*��ʼ������*/
    create_list(grade_list, datafile, &n); /*��CSV�ļ��е����ݵ���������*/
    CrtHuffmanTree(&ht, &hcode, grade_list, n); /*������������*/
    printf("\n-------------�������-------------\n");
    Preorder(ht, 2 * n - 1, visit); /*�������������������ӡ���*/
    /*�������ж�����*/
    printf("\n-------------�����ж�-------------\n");
    score_determine(ht, n);
    /*�ͷŹ��������ռ�*/
    //Preorder(ht, 2 * n - 1, free_HuffmanTree);
    free(ht);
    return 0;
}


/*���������μ�Ȩ�������ͷ�ڵ�*/
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


/*���ļ�ָ���ƶ�����һ��*/
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


/*����csv�ļ��еķ����μ�Ȩ����Ϣ�������������μ�Ȩ�ص�����*/
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
    printf("�½�\t�Ͻ�\tȨ��\n");
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


/*ѡ������parentΪ0��weight��С�Ľ��*/
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


/*������������*/
void CrtHuffmanTree(HuffmanTree *ht, HuffmanCode *hcode, GradeList w, int n)
{
    /*w���n��Ȩֵ, �����������ht, ���������������hc*/
    int m;  /*�����ʾ�ڵ������ı���*/
    int i;  /*����ѭ�����Ʊ���*/
    char *cd;   /*�����ַ���ָ�룬���ڴ��������ɵĹ���������*/
    int start;  /*��������ѭ�����Ʊ���*/
    unsigned int c = 0;
    unsigned int p;
    GradeArea *gp = w -> next;
    if (n <= 1)
        return;
    m = 2 * n - 1;
    *ht = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));    /*0�ŵ�Ԫδʹ��*/
    (*ht) -> bottom_grade = -1;
    (*ht) -> top_grade = -1;
    (*ht) -> weight = 0;
    (*ht) -> parent = 0;
    (*ht) -> lchild = 0;
    (*ht) -> rchild = 0;
    for(i = 1; i <= n; i++) /*Ҷ�ӽ���ʼ��*/
    {
        (*ht + i) -> bottom_grade = gp -> bottom_grade;
        (*ht + i) -> top_grade = gp -> top_grade;
        (*ht + i) -> weight = gp -> weight;
        (*ht + i) -> parent = 0;
        (*ht + i) -> lchild = 0;
        (*ht + i) -> rchild = 0;
        gp = gp -> next;
    }
    for(i = n + 1; i <= m; i++) /*��Ҷ�ӽ���ʼ��*/
    {
        (*ht + i) -> bottom_grade = -1;
        (*ht + i) -> top_grade = -1;
        (*ht + i) -> weight = 0;
        (*ht + i) -> parent = 0;
        (*ht + i) -> lchild = 0;
        (*ht + i) -> rchild = 0;
    }
    for(i = n + 1; i <= m; i++) /*������Ҷ�ӽ��, ����������*/
    {
        /*��ht[1]~ht[i-1]�ķ�Χ��ѡ������parentΪ0��weight��С�Ľ�㣬����ŷֱ�ֵ��s1��s2����*/
        int s1, s2;
        selectht(*ht, i-1, &s1, &s2);
        (*ht + s1) -> parent = i;
        (*ht + s2) -> parent = i;
        (*ht + i) -> lchild = s1;
        (*ht + i) -> rchild = s2;
        (*ht + i) -> weight = (*ht + s1) -> weight + (*ht + s2) -> weight;
    }/*���������������*/
    /*��Ҷ�ӽ�㵽����������ÿ��Ҷ�ӽ���Ӧ�Ĺ���������*/
    *hcode = (HuffmanCode)malloc((n + 1) * sizeof(char *)); /*����n�������ͷָ��*/
    cd = (char *)malloc(n * sizeof(char));  /*������ǰ����Ĺ����ռ�*/
    cd[n - 1]='\0'; /*����������λ��ű��룬���ȴ�ű��������*/
    for(i = 1; i <= n; i++)
    {
        start = n - 1;   /*��ʼ��������ʼָ��*/
        for(c = i, p = (*ht + i) -> parent; p != 0; c = p, p = (*ht + p) -> parent) /*��Ҷ�ӵ�����������*/
        {
            if((*ht + p) -> lchild == c) cd[--start] = '0'; /*���֧��0*/
            else cd[--start] = '1'; /*�ҷ�֧��1*/
        }
        (*hcode)[i] = (char *)malloc((n-start) * sizeof(char)); /*Ϊ��i���������ռ�*/
        strcpy((*hcode)[i], &cd[start]);
    }
    free(cd);
}


/*���ʲ�����ڵ���Ϣ*/
int visit (HuffmanTree ht, unsigned int i)
{
    if (i == 0)
        return OK;
    if ((ht + i) -> bottom_grade >= 0 && (ht + i) -> top_grade <= 100)
    {
        if ((ht + i) -> top_grade == 100)
        {
            printf("�ڵ�%d��Ȩ��Ϊ:%d����Ӧ������[%d,%d]\n", i, (ht + i) -> weight, (ht + i) -> bottom_grade, (ht + i) -> top_grade);
            return OK;
        }
        else
        {
            printf("�ڵ�%d��Ȩ��Ϊ:%d����Ӧ������[%d,%d)\n", i, (ht + i) -> weight, (ht + i) -> bottom_grade, (ht + i) -> top_grade);
            return OK;
        }
    }
    else if ((ht + i) -> bottom_grade == -1 && (ht + i) -> top_grade == -1)
    {
        printf("�ڵ�%d��Ȩ��Ϊ:%d\n", i, (ht + i) -> weight);
        return OK;
    }
    else
        return ERROR;
}


/*���������������*/
int Preorder (HuffmanTree ht, unsigned int i, int (*visit) (HuffmanTree, unsigned int))
{
    /*���������htΪ��ָ��Ķ�����*/
    if (i)
    {
        if (visit(ht, i)) // ͨ������ָ�� *visit ���ʸ����
            if (Preorder(ht, (ht + i) -> lchild, visit)) // �������������
                if (Preorder(ht, (ht + i) -> rchild, visit)) // �������������
                    return OK;
        return ERROR;
    }
    else
        return OK;
}


/*�����ж�*/
void score_determine(HuffmanTree ht, unsigned int n)
{
    int flag = 1;
    int score;
    while (flag)
    {
        printf("������һ���ɼ�(����-1��������):");
        scanf("%d", &score);
        if (score == -1)
        {
            flag = 0;
            break;
        }
        else if (score > 100 || score < 0)
            printf("�����������ĳɼ���Ҫ������[0, 100]�У�\n");
        else
            locate(ht, 2 * n - 1, score);
    }
}


/*������λ*/
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
                    printf("λ�ڷ�����[%d,%d]\n", (ht + i) -> bottom_grade, (ht + i) -> top_grade);
                    return OK;
                }
                else
                    return FINISH;  /*���û��ƥ�䵽������һ��֧�������*/
            }
            else
            if (score >= (ht + i) -> bottom_grade && score < (ht + i) -> top_grade)
            {
                printf("λ�ڷ�����[%d,%d)\n", (ht + i) -> bottom_grade, (ht + i) -> top_grade);
                return OK;
            }
            else
                return FINISH;  /*���û��ƥ�䵽������һ��֧�������*/
        }
        else
        {
            if(locate(ht, (ht + i) -> lchild, score))
                return OK;
            if(locate(ht, (ht + i) -> rchild, score))
                return OK;
        }
        return FINISH;  /*���û��ƥ�䵽������һ��֧�������*/
    }
    return ERROR;
}