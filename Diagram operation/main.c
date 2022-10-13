#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 9999  //最大值
#define MAX_VERTEX_NUM 6    //最大顶点个数
# define OK 1
# define NOTFOUND -1
# define VertexType int
# define VRType int
# define Boolean int
# define TRUE 1
# define FALSE 0
# define APEX 1


//typedef enum{DG, DN, UDG, UDN} GraphKind;   //{有向图，有向网，无向图，无向网}


typedef struct {
    VRType adj;    //顶点关系类型
    //struct ArcCell *Info;   //弧相关信息指针
}ArcCell, AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];


typedef struct {
    //VertexType vex[MAX_VERTEX_NUM];  //顶点向量
    AdjMatrix arcs; //邻接矩阵
    int vexnum, arcnum; //图的当前节点和弧数
    //GraphKind kind; //图的种类标志
}MGraph;


typedef struct{
    VertexType adjvex;
    VRType lowcost;
}Closedge[MAX_VERTEX_NUM];


/*函数申明*/
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


/*初始化图*/
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
    (*G).arcnum = 0;    //初始化弧的数量
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


/*打印输出图*/
int print_MGraph(MGraph G)
{
    int i, j;
    printf("\n----------打印图的邻接矩阵----------\n");
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
                printf("∞\t");
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


/*节点访问函数*/
void visit_demo(MGraph G, int v)
{
    printf("已访问节点V%d\n", DeLocate(G, v));
}


/*深度优先遍历框架*/
void DFSTraverse(MGraph G, void (*Visit)(MGraph, int), int *visited)
{
    int v;
    int flag = 0; //用于标记图是否为连通图
    printf("----------遍历图----------\n");
    for (v = 0; v < G.vexnum; v++)
        visited[v] = 0; //访问标志数组初始化
    for (v = 0; v < G.vexnum; v++)
    {
        if (v > 0)
            if (!visited[v])
                flag += 1;
        if (!visited[v])
            DFS(G, Visit, v, visited);  //对尚未访问的顶点调用DFS
    }
    /*判断图是否连通*/
    if (!flag)
        printf("该图为连通图！\n");
    else
        printf("该图不连通！共有%d个连通分量！\n", flag + 1);
}


/*深度优先遍历*/
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


/*查找首个邻节点函数*/
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


/*查找后一个邻节点函数*/
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


/*最小支撑树*/
void MiniSpanTree_PRIM(MGraph G, VertexType u)
{
    //用普里姆算法从第u个顶点出发构造网G的最小生成树T，输出T的各条边。
    //记录从顶点集U到V一U的代价最小的边的辅助数组定义：
    Closedge closedge;
    int k, j;  //k和j用于储存顶点对应的物理存储位置
    int i;  //循环控制变量
    k = Locate(G, u);
    for(j = 0; j < G.vexnum; j++)   //辅助数组的初始化
    {
        if(j != k)
        {
            closedge[j].adjvex = u;
            closedge[j].lowcost = G.arcs[k][j].adj;
        }
    }
    closedge[k].lowcost = 0; //初始，U={u}
    printf("\n----------生成最小支撑树----------\n");
    for(i = 1; i < G.vexnum; i++)   //选择其余G.vexnum - 1个顶点
    {
        k = minium(closedge);   //求出T的下一个结点：第k顶点
        printf("第%d条边为：V%d <-> V%d, 距离为：%d\n", i, closedge[k].adjvex, DeLocate(G, k), closedge[k].lowcost);  //输出生成树的边
        closedge[k].lowcost = 0;    //第k顶点并入U集
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


/*根据顶点名定位顶点的物理存储位置*/
int Locate(MGraph G, VertexType u)
{
    return u - 1;
}


/*根据顶点的无力存储位置确定顶点名*/
VertexType DeLocate(MGraph G, int k)
{
    return k + 1;
}


/*求最小代价*/
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


/*求最短路*/
void ShortestPath_DIJ(MGraph G, VertexType u, Boolean P[MAX_VERTEX_NUM][MAX_VERTEX_NUM], VRType *D)
{
    int v0 = Locate(G, u);
    int v, w, i, j;
    int min;
    int final[MAX_VERTEX_NUM];
    // 用Dijkstra算法求有向网G的v0顶点到其余顶点v的最短路径P[v]及其带权长度D[v]。
    // 若P[v][w]为TRUE，则w是从v0到v当前求得最短路径上的顶点。
    // final[v]为TRUE当且仅当v属于S，即已经求得从v0到v的最短路径。
    for(v = 0; v < G.vexnum; v++)
    {
        final[v] = FALSE;
        D[v] = G.arcs[v0][v].adj;
        for (w = 0; w < G.vexnum; w++)
            P[v][w] = FALSE;   //设空路径
        if (D[v] < INFINITY)
        {
            P[v][v0] = TRUE;
            P[v][v] = TRUE;
        }
    }
    D[v0] = 0;
    final[v0] = TRUE;    //初始化，v0顶点属于S集
    //开始主循环，每次求得v0到某个v顶点的最短路径，并加v到S集
    for(i = 1; i < G.vexnum; i++)   //其余G.vexnum-1个顶点
    {
        min = INFINITY; //当前所知离v0顶点的最近距离
        for(w = 0; w < G.vexnum; w++)
        {
            if (!final[w])  //w顶点在V-S中
            {
                if (D[w] < min) //w顶点离v0顶点更近
                {
                    v = w;
                    min = D[w];
                }
            }
        }
        final[v] = TRUE;    //离v0顶点最近的v加入S集
        for(w = 0; w < G.vexnum; w++)   //更新当前最短路径及距离
        {
            if(!final[w] && (min + G.arcs[v][w].adj < D[w]))  //修改D[w]和P[w]，w属于V-S
            {
                D[w] = min + G.arcs[v][w].adj;
                for(j = 0; j < G.vexnum; j++)
                    P[w][j] = P[v][j];
                P[w][w] = TRUE;
            }
        }
    }
}


/*输出最短路径*/
void print_ShortestPath(MGraph G, int u, Boolean P[][6], VRType *D)
{
    int v = Locate(G, u);
    int i, j;
    printf("\n----------生成最短路----------\n");
    for(i = 0; i < G.vexnum; i++)
    {
        if(i != v)
        {
            printf("顶点V%d到顶点V%d的最短路径上包含的顶点有：", DeLocate(G, v), DeLocate(G, i));
            for(j = 0; j < G.vexnum; j++)
            {
                if(P[i][j] == TRUE)
                    printf("V%d", DeLocate(G, j));
            }
            printf("。最短路径长为：%d\n", D[i]);
        }
    }
}

int main() {
    MGraph G;
    char datafile[] = { "Nodes_and_weights.csv" };
    int visited[MAX_VERTEX_NUM];
    Boolean P[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    VRType D[MAX_VERTEX_NUM];
    create_MGraph(&G, datafile);    //读取CSV文件，生成图
    print_MGraph(G);    //打印图
    DFSTraverse(G, visit_demo, visited);    //遍历图并判断是否为连通图
    MiniSpanTree_PRIM(G, APEX);    //从顶点APEX(V1)开始生成最小值支撑树
    ShortestPath_DIJ(G, APEX, P, D);   //求顶点APEX(V1)到其他各顶点的最短路径
    print_ShortestPath(G, APEX, P, D);   //打印顶点APEX(V1)到其他各顶点的最短路径及路径长
    return 0;
}
