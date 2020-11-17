#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	int vf, vr;

	// ���Ͽ��� �׷��� ������ �о� V�� E �迭�� �����Ѵ�.
	// V[ ].name�� E[ ].name�� ������ �ʿ䰡 ������ ���ǻ� �� �迭������ index�� �����Ѵ�.
	for (int i = 0; i < Vnum; i++) {
		if (!V[i].S.empty())
			errorExit("Vertex Setting Error");
		V[i].name = i;
		V[i].flag = false;	// V[].flag�� E[].flag�� �ݵ�� false�� �����ؾ� �Ѵ�.
	}

	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &vf, &vr, &E[i].cost);
		E[i].name = i;
		E[i].flag = false;
		E[i].vf = vf;
		E[i].vr = vr;

		SLL *p = pool.allocSLL();	// ���� edge (i,j)�� ������ V[i].S�� edge index k�� push�ϰ�
		SLL *q = pool.allocSLL();	//  ���������� V[j].S�� k�� push�Ѵ�.
		p->i = i;
		q->i = i;
		V[vf].S.push(p);
		V[vr].S.push(q);
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	for (int i = 0; i < Vnum; i++) {
		while (!(V[i].S.empty())) {
			SLL *p = V[i].S.pop();	// V �迭�� adjacency list�� ���Ҹ� pop()�� ����Ͽ�
			pool.freeSLL(p);		// ���  pool�� �ǵ��� �ش�
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
	std::queue<int> Q;	//BFS ������ ���� ť
	int vertex_cur;		//���� Ž�� ���� vertex
	int vertex_next;	//�� ���� Ž���� vertex
	int cost_sum = 0;	//�� cost
	V[src].flag = true;	//���� vertex�� true�� �ٷ� ����
	Q.push(src);		//Q�� ���� vertex �ְ� BFS ���� ����

	while (!Q.empty()) {
		vertex_cur = Q.front();
		Q.pop();

		SLL *p = V[vertex_cur].S.top();
		while (p != NULL) {				//�ش� vertex�� adjacent�� edge�� ��� Ž��
			vertex_next = E[p->i].vf;	//next vertex�� vf, vr�߿��� ���ϴ� ����
			if (E[p->i].vr != vertex_cur)
				vertex_next = E[p->i].vr;

			if (!V[vertex_next].flag) {	//next vertex�� ���� �湮�������� vertex�� ��
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
// �Ʒ� �Լ����� 02_04_CSE3081_CPrg&DataStructure_�߰��ڷ�.pdf��
//   �����Ͽ� �� �Լ����� ä�� ����.
// ���ø� �ʿ��ϹǷ� ť�� ��� �Լ��� �Ʒ� �Լ� ����Ʈ�� ����

// stack member fuctions
// �Ʒ��� �Լ��鿡 ���ؼ��� �����ð� �� �����Բ��� ���� ������ ���ּ̱⿡ ���� �ּ����� �������� �ʾҽ��ϴ�.
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