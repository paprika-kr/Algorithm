#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;
	int eulerianCheck = 0;		//flag�� �����ϴ� ����
	int startingVertex = 0;		//���� Vertex
	int currentVertex;			//���� Ž�� Vertex
	int e, v1, v2;				//���� Ž�� Edge�� �� Edge�� Vertex pair
	DBL* p;
	path = (int*)malloc(sizeof(int) * (EN + 1));
	*pathN = 0;
	*flag = 0;
	
	/*
	flag ����
	degree�� Ȧ���� vertex�� 0�� = flag:0(Euler Cycle)
	degree�� Ȧ���� vertex�� 2�� = flag:1(Euler path)
	degree�� Ȧ���� vertex�� 4�� �̻� = flag:2(not Eulerian)
	*/
	for (int i = 0; i < VN; i++) {
		if (V[i].degree % 2 == 1)
			eulerianCheck++;

		if (eulerianCheck > 2) {
			*flag = 2;
			return path;
		}
	}
	if (eulerianCheck == 2) 
		*flag = 1;
	
	/*
	startingVertex ����
	degree�� Ȧ���� Vertex�� �����Ѵٸ� �� Vertex���� ����
	Euler path Ž���� ����
	*/
	for (int i = 0; i < VN; i++) {
		if (V[i].degree % 2 == 1) {
			startingVertex = i;
			break;
		}
	}

	ST.push(startingVertex);	//���ÿ� ���� vertex�� push

	while (!ST.empty()) {
		currentVertex = ST.top();
		/*
		�� �̻� Ž���� adjacent edge�� ���� vertex�� ���
		stack���� ���� �� path�� ����
		*/
		if (V[currentVertex].degree == 0) {
			(*pathN)++;
			path[*pathN - 1] = currentVertex;
			ST.pop();
		}

		/*
		���� Ž���� adjacent edge�� �����ִ� vertex�� ���
		*/
		else {
			//Edge�� ���� ����(Readability ������ ����)
			e = V[currentVertex].S.top()->d;
			v1 = E[e].v1;
			v2 = E[e].v2;

			/*
			Ž���� edge�� V list���� ���� 
			1)�ܿ� degree ����
			2)����� twin pointer�� ���� 2���� �ߺ� ��带 ����
			3)������ ��带 Storage pool�� �̵�(free)
			4)���ÿ� nextVertex�� push
			*/
			(V[v1].degree)--;
			(V[v2].degree)--;

			if (currentVertex == v1) {
				p = V[v1].S.top()->twin;
				V[v2].S.remove(V[v1].S.top()->twin);
				pool.freeDBL(p);

				p = V[v1].S.top();
				V[v1].S.remove(V[v1].S.top());
				pool.freeDBL(p);

				ST.push(v2);
			}
				
			else {
				p = V[v2].S.top()->twin;
				V[v1].S.remove(V[v2].S.top()->twin);
				pool.freeDBL(p);

				p = V[v2].S.top();
				V[v2].S.remove(V[v2].S.top());
				pool.freeDBL(p);

				ST.push(v1);
			}
		}
	}
	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;

	/*
	���� �������� ���� �޸𸮰� ������ ���� ã�Ƽ� 
	Storage pool�� �̵���Ű��
	�����Ҵ��ߴ� V�� E�� ����
	*/
	for (int i = 0; i < VN; i++) {
		while (!V[i].S.empty()) {
			p = V[i].S.pop();
			pool.freeDBL(p);
		}
	}
	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;

	for (int i = 0; i < EN; i++) {
		//DBL Storage pool���� new node �Ҵ�
		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();

		//�ش� Edge�� ���� ����
		v1 = E[i].v1;
		v2 = E[i].v2;

		//DBL ��忡 �ε���, twin ����
		adjE1->d = i;
		adjE2->d = i;
		adjE1->twin = adjE2;
		adjE2->twin = adjE1;

		//V array stack�� push
		V[v1].S.push(adjE1);
		V[v2].S.push(adjE2);
	}

}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

// DBL Storage Pool ���� �Լ���
// DBL Storage pool -> application program
DBL *DBList::allocDBL(void) {		// allocate a DBL element
	DBL *p;
	if (DBL_pool == NULL) {
		p = new DBL;
		if (p == NULL) {
			Error_Exit("Memory alloc error(Alloc_DBL)");
		}
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}
	p->rp = p->lp = p->twin = NULL;
	++DBL_cnt;

	return(p);
}

//application program -> DBL Storage pool
void DBList::freeDBL(DBL *p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;
	--DBL_cnt;		// reduce # of active DBL elements by one 
}

//pool�� �޸� ���� : ���α׷� ���� ��
void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

//DBL Stack Node ���� �Լ���
void dblStack::push(DBL *p) {
	if (ST != NULL) {
		ST->lp = p;
	}
	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

DBL *dblStack::pop(void) {	// remove and return p in front of Q
	DBL *p = ST;
	if (ST->rp == NULL)
		ST = NULL;
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}
	return p;
}

void dblStack::remove(DBL *p) {	// delete p from ST
	if (ST == p) {
		ST = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = NULL;
		}
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = p->lp;
		}
	}

}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
