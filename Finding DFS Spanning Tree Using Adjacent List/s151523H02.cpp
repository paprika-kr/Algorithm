#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	int fTemp, rTemp;
	
	//V와 E의 초기 설정
	//V와 E의 name field는 각각 자신의 인덱스를 저장
	//flag는 main에서 false 초기화 해주기때문에 생략
	for (int i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].f_hd = NONE;
		V[i].r_hd = NONE;
	}

	for (int i = 0; i < Enum; i++) {
		E[i].name = i;
		E[i].fp = NONE;
		E[i].rp = NONE;
		E[i].vf = NONE;
		E[i].vr = NONE;
	}

	//그래프를 읽으면서 V와 E 배열에 해당하는 영역에 데이터를 저장
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &fTemp , &rTemp, &E[i].cost);
		//원래 저장되어있던 V.?_hd의 edge index를 E.?p로 이동
		if(V[fTemp].f_hd != NONE)
			E[i].fp = V[fTemp].f_hd;
		V[fTemp].f_hd = i;

		if(V[rTemp].r_hd != NONE)
			E[i].rp = V[rTemp].r_hd;
		V[rTemp].r_hd = i;

		//E.v?에 front/rear vertex index를 저장
		E[i].vf = fTemp;
		E[i].vr = rTemp;
	}

}

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	int costSum = 0;	//cost의 총합
	edge* nextEdge;		//nextEdge라는 edge 포인터를 사용하여 
						//DFS탐색 시 다음 노드를 알아낼 수 있도록 작성하였음

	V[src].flag = true; // 시작 vertex flag : true

	//Undirected graph represented with array의 경우
	//front와 rear 두 방향으로 DFS 검색을 해야한다.

	//nextEdge의 초기 설정
	nextEdge = &E[V[src].f_hd];	
	
	//case 1: 해당 vertex가 front인 edge가 존재하는 경우
	if (V[src].f_hd != NONE) {
		while (1){
			//방문하지 않은 vertex 발견
			if (!V[nextEdge->vr].flag) {	
				nextEdge->flag = true;
				costSum += nextEdge->cost;
				//발견한 vertex를 시작 vertex로 하는 DFS를 recursive 호출한다.
				costSum += DFS_Tree_adj_array(nextEdge->vr, Vnum, V, Enum, E);
			}

			//방문한 vertex를 발견
			else {			
				//다음 탐색할 edge가 없는 경우 front 방향 탐색 중단
				if (nextEdge->fp == NONE)
					break;

				//다음 탐색할 edge로 포인터 초기화
				nextEdge = &E[nextEdge->fp];
			}
		}
	}

	nextEdge = &E[V[src].r_hd];
	//case 2: 해당 vertex가 rear인 edge가 존재하는 경우
	//case 1과 동일한 알고리즘
	if (V[src].r_hd != NONE){
		while (1){
			if (!V[nextEdge->vf].flag) {
				nextEdge->flag = true;
				costSum += nextEdge->cost;
				costSum += DFS_Tree_adj_array(nextEdge->vf, Vnum, V, Enum, E);
			}
			else {
				if (nextEdge->rp == NONE)
					break;
				nextEdge = &E[nextEdge->rp];
			}
		} 
	}

	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	return costSum;
}
