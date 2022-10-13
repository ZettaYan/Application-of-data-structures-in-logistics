#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 9999  //���ֵ
#define MAX_VERTEX_NUM 6    //��󶥵����
# define OK 1
# define NOTFOUND -1
# define VertexType int
# define VRType int
# define Boolean int
# define TRUE 1
# define FALSE 0
# define APEX 1


//typedef enum{DG, DN, UDG, UDN} GraphKind;   //{����ͼ��������������ͼ��������}


typedef struct {
    VRType adj;    //�����ϵ����
    //struct ArcCell *Info;   //�������Ϣָ��
}ArcCell, AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];


typedef struct {
    //VertexType vex[MAX_VERTEX_NUM];  //��������
    AdjMatrix arcs; //�ڽӾ���
    int vexnum, arcnum; //ͼ�ĵ�ǰ�ڵ�ͻ���
    //GraphKind kind; //ͼ�������־
}MGraph;


typedef struct{
    VertexType adjvex;
    VRType lowcost;
}Closedge[MAX_VERTEX_NUM];


/*��������*/
int create_MGraph(MGraph *, char *);
int print_MGraph(MGraph);
void visit_demo(MGraph, int);
void DFSTraverse(MGraph, void (*Visit)(MGraph, int), int *);
void DFS(MGraph, void (*Visit)(MGraph, int), int, int *);
int FirstAdjVex(MGraph, int);
int NextAdjVex(MGraph, int, int);
void MiniSpanTree_PRIM(MGraph, VertexType);
int Locate(MGraph, VertexType);
VertexType DeLocate(MGraph, int);
int minium(Closedge);
void ShortestPath_DIJ(MGraph, VertexType, Boolean P[][6], VRType *);
void print_ShortestPath(MGraph, int, Boolean P[MAX_VERTEX_NUM][MAX_VERTEX_NUM], VRType *);


/*��ʼ��ͼ*/
int create_MGraph(MGraph *G, char *filename)
{
    FILE *fp;
    VRType i, j;
    char temp[50], *t = temp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open this file.\n");
    }
    (*G).vexnum = MAX_VERTEX_NUM;
    (*G).arcnum = 0;    //��ʼ����������
    for(i = 0; i < MAX_VERTEX_NUM; i++)
    {
        for(j = 0; j < MAX_VERTEX_NUM; j++)
        {
            while (!feof(fp))
            {
                int flag = 0;
                *t = fgetc(fp);
                if(*t == ',' || *t == '\n')
                {
                    *t = '\0';
                    (*G).arcs[i][j].adj = (VRType) atoi(temp);
                    t = temp;
                    flag = 1;
                    if((*G).arcs[i][j].adj >= 0 && (*G).arcs[i][j].adj != INFINITY)
                        (*G).arcnum += 1;
                }
                else
                    t++;
                if(flag == 1)
                    break;
            }
        }
    }
    fclose(fp);
    return OK;
}


/*��ӡ���ͼ*/
int print_MGraph(MGraph G)
{
    int i, j;
    printf("\n----------��ӡͼ���ڽӾ���----------\n");
    printf("\t");
    for(i = 0; i < MAX_VERTEX_NUM; i++)
        printf("V%d\t", DeLocate(G, i));
    printf("\n");
    for(i = 0; i < MAX_VERTEX_NUM; i++)
    {
        printf("V%d\t", DeLocate(G, i));
        for(j = 0; j < MAX_VERTEX_NUM; j++)
        {
            if(G.arcs[i][j].adj < INFINITY)
                printf("%d\t", G.arcs[i][j].adj);
            else
                printf("��\t");
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


/*�ڵ���ʺ���*/
void visit_demo(MGraph G, int v)
{
    printf("�ѷ��ʽڵ�V%d\n", DeLocate(G, v));
}


/*������ȱ������*/
void DFSTraverse(MGraph G, void (*Visit)(MGraph, int), int *visited)
{
    int v;
    int flag = 0; //���ڱ��ͼ�Ƿ�Ϊ��ͨͼ
    printf("----------����ͼ----------\n");
    for (v = 0; v < G.vexnum; v++)
        visited[v] = 0; //���ʱ�־�����ʼ��
    for (v = 0; v < G.vexnum; v++)
    {
        if (v > 0)
            if (!visited[v])
                flag += 1;
        if (!visited[v])
            DFS(G, Visit, v, visited);  //����δ���ʵĶ������DFS
    }
    /*�ж�ͼ�Ƿ���ͨ*/
    if (!flag)
        printf("��ͼΪ��ͨͼ��\n");
    else
        printf("��ͼ����ͨ������%d����ͨ������\n", flag + 1);
}


/*������ȱ���*/
void DFS(MGraph G, void (*Visit)(MGraph, int), int v, int *visited)
{
    int w;
    visited[v] = 1;
    Visit(G, v);
    for(w = FirstAdjVex(G, v); w >= 0; w = NextAdjVex(G, v, w))
    {
        if (!visited[w])
            DFS(G, Visit, w, visited);
    }
}


/*�����׸��ڽڵ㺯��*/
int FirstAdjVex(MGraph G, int v)
{
    int i;
    for(i = 0; i < G.vexnum; i++)
    {
        if(G.arcs[v][i].adj < INFINITY)
            return i;
    }
    return NOTFOUND;
}


/*���Һ�һ���ڽڵ㺯��*/
int NextAdjVex(MGraph G, int v, int w)
{
    int i;
    for(i = w + 1; i < G.vexnum; i++)
    {
        if(G.arcs[v][i].adj < INFINITY)
            return i;
    }
    return NOTFOUND;
}


/*��С֧����*/
void MiniSpanTree_PRIM(MGraph G, VertexType u)
{
    //������ķ�㷨�ӵ�u���������������G����С������T�����T�ĸ����ߡ�
    //��¼�Ӷ��㼯U��VһU�Ĵ�����С�ıߵĸ������鶨�壺
    Closedge closedge;
    int k, j;  //k��j���ڴ��涥���Ӧ������洢λ��
    int i;  //ѭ�����Ʊ���
    k = Locate(G, u);
    for(j = 0; j < G.vexnum; j++)   //��������ĳ�ʼ��
    {
        if(j != k)
        {
            closedge[j].adjvex = u;
            closedge[j].lowcost = G.arcs[k][j].adj;
        }
    }
    closedge[k].lowcost = 0; //��ʼ��U={u}
    printf("\n----------������С֧����----------\n");
    for(i = 1; i < G.vexnum; i++)   //ѡ������G.vexnum - 1������
    {
        k = minium(closedge);   //���T����һ����㣺��k����
        printf("��%d����Ϊ��V%d <-> V%d, ����Ϊ��%d\n", i, closedge[k].adjvex, DeLocate(G, k), closedge[k].lowcost);  //����������ı�
        closedge[k].lowcost = 0;    //��k���㲢��U��
        for(j = 0; j < G.vexnum ; j++)
        {
            if (G.arcs[k][j].adj < closedge[j].lowcost)
            {
                closedge[j].adjvex = DeLocate(G, k);
                closedge[j].lowcost = G.arcs[k][j].adj;
            }
        }
    }
}


/*���ݶ�������λ���������洢λ��*/
int Locate(MGraph G, VertexType u)
{
    return u - 1;
}


/*���ݶ���������洢λ��ȷ��������*/
VertexType DeLocate(MGraph G, int k)
{
    return k + 1;
}


/*����С����*/
int minium(Closedge closedge)
{
    int firstidex, minidex, i;
    for(firstidex = 0; firstidex < MAX_VERTEX_NUM; firstidex++)
    {
        if (closedge[firstidex].lowcost > 0 && closedge[firstidex].lowcost < INFINITY)
            break;
    }
    for(i = firstidex + 1, minidex = firstidex; i < MAX_VERTEX_NUM; i++)
    {
        if(closedge[i].lowcost < closedge[minidex].lowcost)
            minidex = i;
    }
    return minidex;
}


/*�����·*/
void ShortestPath_DIJ(MGraph G, VertexType u, Boolean P[MAX_VERTEX_NUM][MAX_VERTEX_NUM], VRType *D)
{
    int v0 = Locate(G, u);
    int v, w, i, j;
    int min;
    int final[MAX_VERTEX_NUM];
    // ��Dijkstra�㷨��������G��v0���㵽���ඥ��v�����·��P[v]�����Ȩ����D[v]��
    // ��P[v][w]ΪTRUE����w�Ǵ�v0��v��ǰ������·���ϵĶ��㡣
    // final[v]ΪTRUE���ҽ���v����S�����Ѿ���ô�v0��v�����·����
    for(v = 0; v < G.vexnum; v++)
    {
        final[v] = FALSE;
        D[v] = G.arcs[v0][v].adj;
        for (w = 0; w < G.vexnum; w++)
            P[v][w] = FALSE;   //���·��
        if (D[v] < INFINITY)
        {
            P[v][v0] = TRUE;
            P[v][v] = TRUE;
        }
    }
    D[v0] = 0;
    final[v0] = TRUE;    //��ʼ����v0��������S��
    //��ʼ��ѭ����ÿ�����v0��ĳ��v��������·��������v��S��
    for(i = 1; i < G.vexnum; i++)   //����G.vexnum-1������
    {
        min = INFINITY; //��ǰ��֪��v0������������
        for(w = 0; w < G.vexnum; w++)
        {
            if (!final[w])  //w������V-S��
            {
                if (D[w] < min) //w������v0�������
                {
                    v = w;
                    min = D[w];
                }
            }
        }
        final[v] = TRUE;    //��v0���������v����S��
        for(w = 0; w < G.vexnum; w++)   //���µ�ǰ���·��������
        {
            if(!final[w] && (min + G.arcs[v][w].adj < D[w]))  //�޸�D[w]��P[w]��w����V-S
            {
                D[w] = min + G.arcs[v][w].adj;
                for(j = 0; j < G.vexnum; j++)
                    P[w][j] = P[v][j];
                P[w][w] = TRUE;
            }
        }
    }
}


/*������·��*/
void print_ShortestPath(MGraph G, int u, Boolean P[][6], VRType *D)
{
    int v = Locate(G, u);
    int i, j;
    printf("\n----------�������·----------\n");
    for(i = 0; i < G.vexnum; i++)
    {
        if(i != v)
        {
            printf("����V%d������V%d�����·���ϰ����Ķ����У�", DeLocate(G, v), DeLocate(G, i));
            for(j = 0; j < G.vexnum; j++)
            {
                if(P[i][j] == TRUE)
                    printf("V%d", DeLocate(G, j));
            }
            printf("�����·����Ϊ��%d\n", D[i]);
        }
    }
}

int main() {
    MGraph G;
    char datafile[] = { "Nodes_and_weights.csv" };
    int visited[MAX_VERTEX_NUM];
    Boolean P[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    VRType D[MAX_VERTEX_NUM];
    create_MGraph(&G, datafile);    //��ȡCSV�ļ�������ͼ
    print_MGraph(G);    //��ӡͼ
    DFSTraverse(G, visit_demo, visited);    //����ͼ���ж��Ƿ�Ϊ��ͨͼ
    MiniSpanTree_PRIM(G, APEX);    //�Ӷ���APEX(V1)��ʼ������Сֵ֧����
    ShortestPath_DIJ(G, APEX, P, D);   //�󶥵�APEX(V1)����������������·��
    print_ShortestPath(G, APEX, P, D);   //��ӡ����APEX(V1)����������������·����·����
    return 0;
}
