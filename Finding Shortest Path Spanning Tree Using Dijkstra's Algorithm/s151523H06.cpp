#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#define NONE -1
#define OUT_OF_RANGE 1000000
#define SWAP(x,y,t) ((t)=(x),(x)=(y),(y)=(t))

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;


int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	int next;
	/*--------------�ʱⰪ ����--------------*/
	for (int i = 0; i < Vnum; i++) {
		V[i].inS = false;
		V[i].distance = OUT_OF_RANGE;
		V[i].heap_idx = NONE;
	}
	V[src].inS = true;
	V[src].distance = 0;

	//src�� ������ vertex�鿡 ���� distance ����
	next = V[src].f_hd;
	while (1) {
		if (next == NONE)
			break;
		V[E[next].vr].distance = E[next].cost;
		next = E[next].fp;
	}

	next = V[src].r_hd;
	while (1) {
		if (next == NONE)
			break;
		V[E[next].vf].distance = E[next].cost;
		next = E[next].rp;
	}

	/*---------------Min Heap ����--------------*/
	int heap_idx;
	bool bIsOverThanSrc = false;		//�� �Ҵ��, �ݺ����� index�� minHeap�� index�� ��ġ�����ֱ� ���� flag
	for (int i = 0; i < Vnum; i++) {
		//src > i : heap_Idx = i + 1
		//src < i : heap_Idx = i
		if (i == src) {
			bIsOverThanSrc = true;
			continue;
		}
		if (bIsOverThanSrc)
			heap_idx = i;
		else
			heap_idx = i + 1;

		//minHeap�� �ϳ��� insert
		while (heap_idx != 1 && V[minHeap[heap_idx / 2]].distance > V[i].distance) {
			minHeap[heap_idx] = minHeap[heap_idx / 2];
			V[minHeap[heap_idx]].heap_idx = heap_idx;
			heap_idx = heap_idx / 2;
		}
		minHeap[heap_idx] = i;

		V[i].heap_idx = heap_idx;
	}

	/*-------------Dijkstra ����--------------*/
	int u, u_dist;
	int heap_size = Vnum-1;
	for (int i = 0; i < Vnum - 1; i++) {
		//���� ���� distance[u]�� ����
		//minHeap[1] : ���� distance�� ���� Vertex
		u = minHeap[1];
		V[u].inS = true;

		/*-------Heap ���� (Delete)-------*/
		//1. heap�� ���� ��, ���� ���� ��带 swap
		int parent, child, temp;
		SWAP(minHeap[1], minHeap[heap_size], temp);
		V[u].heap_idx = NONE;
		V[minHeap[1]].heap_idx = 1;
		heap_size--;

		//2. ���ʴ�� Heap�� ���ǿ� �°� node �̵�
		parent = 1;
		child = parent * 2;
		temp = minHeap[parent];
		while (child <= heap_size) {
			// �ΰ��� child �� �� ���� node ����
			if (child + 1 <= heap_size) {
				if (V[minHeap[child]].distance > V[minHeap[child + 1]].distance) {
					child++;
				}
			}
			if (child > heap_size) {
				break;
			}
			// ������ �ڸ��� �̵� �Ϸ� ��
			if (V[minHeap[child]].distance >= V[temp].distance) {
				break;
			}
			// ���� Heap ���� �Ҹ��� �� 
			else {
				minHeap[child / 2] = minHeap[child];
				V[minHeap[child / 2]].heap_idx = child / 2;
				child *= 2;
			}
		}
		// �ش� node ����
		if (heap_size == 0) 
			V[minHeap[child / 2]].heap_idx = NONE;
		else {
			minHeap[child / 2] = temp;
			V[minHeap[child / 2]].heap_idx = child / 2;
		}

		/*-----------distance update(front)-----------*/
		u_dist = V[u].distance;
		next = V[u].f_hd;
		while (1) {
			if (next == NONE)
				break;
			//u�� �߰��� S�� ���������Ͽ� �� ª�� distance�� update
			if (V[E[next].vr].inS == false && V[E[next].vr].distance > u_dist + E[next].cost) {
				V[E[next].vr].distance = u_dist + E[next].cost;

		/*-----Heap adjust ���� (Move-up, front)-------*/
				temp = E[next].vr;
				child = V[E[next].vr].heap_idx;
				while (child > 1) {
					// ������ �ڸ��� �̵� �Ϸ� ��
					if (V[minHeap[child]].distance > V[minHeap[child / 2]].distance)
						break;
					// ���� Heap ���� �Ҹ��� �� 
					else {
						SWAP(minHeap[child], minHeap[child / 2], temp);
						V[minHeap[child]].heap_idx = child;
						V[minHeap[child / 2]].heap_idx = child / 2;
						child = child / 2;
					}
				}
			}
			next = E[next].fp;
		}

		/*-----------distance update(rear)-----------*/
		next = V[u].r_hd;
		while (1) {
			if (next == NONE)
				break;
			//u�� �߰��� S�� ���������Ͽ� �� ª�� distance�� update
			if (V[E[next].vf].inS == false && V[E[next].vf].distance > u_dist + E[next].cost) {
				V[E[next].vf].distance = u_dist + E[next].cost;

		/*-----Heap adjust ���� (Move-up, rear)-------*/
				temp = E[next].vf;
				child = V[E[next].vf].heap_idx;
				while (child > 1) {
					// ������ �ڸ��� �̵� �Ϸ� ��
					if (V[minHeap[child]].distance >= V[minHeap[child / 2]].distance)
						break;
					// ���� Heap ���� �Ҹ��� �� 
					else {
						SWAP(minHeap[child], minHeap[child / 2], temp);
						V[minHeap[child]].heap_idx = child;
						V[minHeap[child / 2]].heap_idx = child / 2;
						child = child / 2;
					}
				}
			}
			next = E[next].rp;
		}
	}

	/*----------------Back-tracing---------------*/
	// �� Vertex���� V[f]distance - edge(f, r)_cost = V[r]distance�� �����ϸ鼭
	// cost�� �ּ��� edge�� �����ϴ� �˰���
	// ���� ��� ���� edge�� �˻�
	int ctemp;			// min cost�� �ӽ����� ����
	for (int i = 0; i < Vnum; i++) {
		ctemp = -1;
		if (i == src)
			continue;

		next = V[i].f_hd;
		while (1) {
			if (next == NONE)
				break;
			if (V[i].distance - E[next].cost == V[E[next].vr].distance){
				if (ctemp == -1 || E[ctemp].cost > E[next].cost) {
					ctemp = next;
				}
			}
			next = E[next].fp;
		}
		
		next = V[i].r_hd;
		while (1) {
			if (next == NONE)
				break;
			if (V[i].distance - E[next].cost == V[E[next].vf].distance) {
				if (ctemp == -1 || E[ctemp].cost > E[next].cost) {
					ctemp = next;
				}
			}
			next = E[next].rp;
		}
		
		//�ּ� cost edge -> true
		E[ctemp].flag = true;

		//treeCost ����
		treeCost += E[ctemp].cost;
		
	}
	return treeCost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph �ڷᱸ���� ����� �Լ�
	int fTemp, rTemp;

	//V�� E�� �ʱ� ����
	//flag�� main���� false �ʱ�ȭ ���ֱ⶧���� ����
	for (int i = 0; i < Vnum; i++) {
		V[i].f_hd = NONE;
		V[i].r_hd = NONE;
	}

	for (int i = 0; i < Enum; i++) {
		E[i].fp = NONE;
		E[i].rp = NONE;
		E[i].vf = NONE;
		E[i].vr = NONE;
		E[i].flag = false;
	}

	//�׷����� �����鼭 V�� E �迭�� �ش��ϴ� ������ �����͸� ����
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &fTemp, &rTemp, &E[i].cost);
		//���� ����Ǿ��ִ� V.?_hd�� edge index�� E.?p�� �̵�
		if (V[fTemp].f_hd != NONE)
			E[i].fp = V[fTemp].f_hd;
		V[fTemp].f_hd = i;

		if (V[rTemp].r_hd != NONE)
			E[i].rp = V[rTemp].r_hd;
		V[rTemp].r_hd = i;

		//E.v?�� front/rear vertex index�� ����
		E[i].vf = fTemp;
		E[i].vr = rTemp;
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main(void) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex[Vnum];
		E = new edge[Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if (V == NULL || E == NULL || minHeap == NULL) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// �Ʒ� Tree_Check�� SPT_test �Լ��� ÷���� SPT_test.obj ���Ͽ� �ִ�.
		// �� �׽�Ʈ���� ������ ���Ͽ� ���α׷��� �����ϸ� ���� �߸��� ���̴�(�����ص� 0��)
		if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if (t != 0)
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n",
			t + 1, Vnum, Enum, cmpt, Tree_cost);

		delete[] minHeap; delete[] V; delete[] E;
	}
	return 0;
}

void Error_Exit(const char *s) {
	printf("%s\n", s);
	exit(-1);
}
