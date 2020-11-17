#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	int vf, vr;

	// 파일에서 그래프 정보를 읽어 V와 E 배열을 구성한다.
	// V[ ].name과 E[ ].name은 설정할 필요가 없으나 편의상 각 배열에서의 index를 저장한다.
	for (int i = 0; i < Vnum; i++) {
		if (!V[i].S.empty())
			errorExit("Vertex Setting Error");
		V[i].name = i;
		V[i].flag = false;	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	}

	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &vf, &vr, &E[i].cost);
		E[i].name = i;
		E[i].flag = false;
		E[i].vf = vf;
		E[i].vr = vr;

		SLL *p = pool.allocSLL();	// 따라서 edge (i,j)를 읽으면 V[i].S에 edge index k를 push하고
		SLL *q = pool.allocSLL();	//  마찬가지로 V[j].S에 k를 push한다.
		p->i = i;
		q->i = i;
		V[vf].S.push(p);
		V[vr].S.push(q);
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	for (int i = 0; i < Vnum; i++) {
		while (!(V[i].S.empty())) {
			SLL *p = V[i].S.pop();	// V 배열의 adjacency list의 원소를 pop()을 사용하여
			pool.freeSLL(p);		// 모두  pool로 되돌려 준다
		}
	}
}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0)
) {
	std::queue<int> Q;	//BFS 연산을 위한 큐
	int vertex_cur;		//현재 탐색 중인 vertex
	int vertex_next;	//그 다음 탐색할 vertex
	int cost_sum = 0;	//총 cost
	V[src].flag = true;	//시작 vertex는 true로 바로 설정
	Q.push(src);		//Q에 시작 vertex 넣고 BFS 연산 시작

	while (!Q.empty()) {
		vertex_cur = Q.front();
		Q.pop();

		SLL *p = V[vertex_cur].S.top();
		while (p != NULL) {				//해당 vertex에 adjacent한 edge를 모두 탐색
			vertex_next = E[p->i].vf;	//next vertex를 vf, vr중에서 정하는 과정
			if (E[p->i].vr != vertex_cur)
				vertex_next = E[p->i].vr;

			if (!V[vertex_next].flag) {	//next vertex가 아직 방문하지않은 vertex일 때
				V[vertex_next].flag = true;
				E[p->i].flag = true;
				cost_sum += E[p->i].cost;
				Q.push(vertex_next);
			}
			p = p->p;
		}
	}
	return cost_sum;
}

// SLL management functions
// 아래 함수들은 02_04_CSE3081_CPrg&DataStructure_추가자료.pdf를
//   참조하여 각 함수들을 채워 넣자.
// 스택만 필요하므로 큐의 멤버 함수는 아래 함수 리스트에 없다

// stack member fuctions
// 아래의 함수들에 대해서는 수업시간 때 교수님께서 상세히 설명을 해주셨기에 굳이 주석으로 설명하지 않았습니다.
void sllStack2::push(SLL *p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL *sllStack2::pop(void) {
	// remove and return p at the top of ST
	SLL *p = ST;
	if (ST->p == NULL)
		ST = NULL;
	else
		ST = ST->p;
	return p;
}

SLL *sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions
SLL *SLList2::allocSLL(void) {		// allocate an SLL element
	SLL *p;

	if (SLL_pool == NULL) {
		p = (SLL*)malloc(sizeof(SLL));
		if (p == NULL)
			errorExit("Memory allocation error(allocSLL)");
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	else {
		p = SLL_pool;
		SLL_pool = p->p;
	}
	p->p = NULL;
	++SLL_cnt;

	return(p);
}

void SLList2::freeSLL(SLL *p) {		// free *p (move it to SLL_pool
	if (p->i == NONE) {
		errorExit("This is already freed(freeSLL).");
	}

	p->i = NONE;
	p->p = SLL_pool;
	SLL_pool = p;
	--SLL_cnt;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
	SLL *p = SLL_pool;

	while (p != NULL) {
		SLL_pool = p->p;
		free(p);
		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}

	if (SLL_cnt != 0) {
		errorExit("Non-zero SLL_cnt after cleanup.");
	}
}