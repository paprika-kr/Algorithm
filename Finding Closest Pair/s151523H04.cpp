#include <math.h>
#define SWAP(x, y, temp) ( (temp)=(x), (x)=(y), (y)=(temp) )
#define EUCLIDEAN(X, Y, i, j) (X[i] - X[j]) * (X[i] - X[j]) + (Y[i] - Y[j]) * (Y[i] - Y[j])

/*Merge & Merge Sort*/
void merge(unsigned low, unsigned mid, unsigned high, double* pt, unsigned* TMP, unsigned* id) {
	unsigned i, j, k;
	i = low; j = mid + 1; k = low;

	while (i <= mid && j <= high) {
		if (pt[TMP[i]] <= pt[TMP[j]]) { id[k] = TMP[i]; i++; }
		else { id[k] = TMP[j]; j++; }
		k++;
	}

	/*
	mid+1 ~ high ���̿� �˻����� ���� elements ���� ��
	���� �״�� �־��ֱ�
	*/
	if (i > mid) {
		for (int l = j; l <= high; l++) {
			id[k] = TMP[l];
			k++;
		}
	}

	/*
	low ~ mid ���̿� �˻����� ���� elements ���� ��
	���� �״�� �־��ֱ�
	*/
	else {
		for (int l = i; l <= mid; l++) {
			id[k] = TMP[l];
			k++;
		}
	}

	/*TMP�� �ٽ� ����*/
	for (int i = low; i <= high; i++)
		TMP[i] = id[i];
}

void mergesort(unsigned low, unsigned high, double* X, unsigned* TMP, unsigned* Xid) {
	unsigned mid;
	if (low < high) {
		mid = (low + high) / 2;
		mergesort(low, mid, X, TMP, Xid);
		mergesort(mid + 1, high, X, TMP, Xid);
		merge(low, mid, high, X, TMP, Xid);
	}
}

/*Selection Sort*/
void selectionsort(double* Y, unsigned* Yid, unsigned L, unsigned R) {
	/*
	Selection Sort : THS���� ���� ������ Yid�� �����ϱ� ���� ���
	min : �ּҰ��� ����Ű�� Index
	*/
	unsigned i, j, min, temp;
	int n = R - L + 1;

	for (i = L; i < R; i++) {						
		min = i;

		for (j = i + 1; j <= R; j++) {				// Y�� �� �ּڰ��� Ž��
			if (Y[Yid[j]] < Y[Yid[min]])
				min = j;
		}

		
		if (i != min) {							// �ּڰ��� ���簪�� �ٸ� �� Swap
			SWAP(Yid[i], Yid[min], temp);
		}
	}
}

/*Xid ���� �Լ�*/
void sortXid(double *X, unsigned *Xid, unsigned *TMP, unsigned N) {
	/*TMP�� [0...n-1] �ε����� �ְ�
	mergesort ȣ��*/
	for (int i = 0; i < N; i++) 
		TMP[i] = i;
	
	mergesort(0, N - 1, X, TMP, Xid);
}
/*closestPairDC�� combine �Լ�
(x��ǥ�� dl, dr�� �ϳ��� �����ϴ� pair���� ����)*/
void combine(
	unsigned* Yid, 
	unsigned L, unsigned M, unsigned R, double dlr, 
	double* X, double* Y, unsigned* Xid, unsigned* TMP, 
	unsigned *pt1, unsigned *pt2){

	double Xmid, dist = dlr;
	int count = 0;

	//Xmid�� �������� pt.x +- dlr��ŭ�� point pair index ����
	Xmid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;
	for (int i = L; i <= R; i++) {
		if (Xmid - dlr <= X[Yid[i]] && X[Yid[i]] <= Xmid + dlr){
			TMP[count] = Yid[i];
			count++;
		}
	}

	for (int i = 0; i < count - 1; i++) {
		for (int j = i + 1; j < count; j++) {
			if (Y[TMP[j]] - Y[TMP[i]] >= dlr) break;			//Y ��ǥ�� dlr�̻� ���̳��� pair���ʹ� break

			if (dist > sqrt(EUCLIDEAN(X, Y, TMP[i], TMP[j]))) {	//���ݱ��� dist���� ���� ���� ���� ��� dist, pt update
				dist = sqrt(EUCLIDEAN(X, Y, TMP[i], TMP[j]));
				*pt1 = TMP[i];
				*pt2 = TMP[j];
			}
		}
	}
}

double closestPairDC(
	unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned *pt1, unsigned *pt2, // closest pair points indices
	double *X, double *Y,         // (x,y) positions array(input)
	unsigned *Xid,  // point index array(sorted by x coordinates)
	unsigned *Yid,  // point index array(gradually sorted by y coordinates)
	unsigned *TMP,  // temporal index array
	unsigned THR	// threshold value
) {
	double dl, dr, dlr, dist;
	unsigned M;

	/*
	1) *pt1 == *pt2 : 
	���� pt1, pt2�� �ѹ��� ���������ʾ� ���� �ʱ�ȭ ���� �ʱⰪ�� ������ �ִ� ����̹Ƿ�
	0�� 1�� ���Ƿ� ������ �� dist ����

	2) *pt2 != *pt2 :
	���� ���࿡�� pt�� ������ �ٰ� �����ϴ� ����̹Ƿ�
	�̸� �������� dist ����
	*/
	if (*pt1 != *pt2) 
		dist = EUCLIDEAN(X, Y, *pt1, *pt2);
	else {
		*pt1 = 0; *pt2 = 1;
		dist = EUCLIDEAN(X, Y, 0, 1);
	}

	/*
	Threshold >= size :
	BruteForce Method�� ���� ���
	*/
	if (R - L + 1 <= THR) {

		//Yid ����
		for (int i = L; i <= R; i++)
			Yid[i] = Xid[i];
		selectionsort(Y, Yid, L, R);

		for (unsigned i = L; i < R; i++) {
			for (unsigned j = i + 1; j <= R; j++) {
				if (dist > EUCLIDEAN(X, Y, Xid[i], Xid[j])) {	//���ݱ��� dist���� ���� ���� ���� ��� dist, pt update
					dist = EUCLIDEAN(X, Y, Xid[i], Xid[j]);
					*pt1 = Xid[i];
					*pt2 = Xid[j];
				}
			}
		}
	}

	/*
	Threshold < size :
	D&C Method ���
	*/
	else {
		M = (L + R) / 2;
		dl = closestPairDC(L, M, pt1, pt2, X, Y, Xid, Yid, TMP, THR);		//���� ����
		dr = closestPairDC(M + 1, R, pt1, pt2, X, Y, Xid, Yid, TMP, THR);	//������ ����

		//�� divided space���� ���ĵ� Yid�� merge
		for (int i = L; i <= R; i++)
			TMP[i] = Yid[i];
		merge(L, M, R, Y, TMP, Yid);
		
		//min(dl, dr)
		dlr = dl;
		if (dl > dr)
			dlr = dr;

		combine(Yid, L, M, R, dlr, X, Y, Xid, TMP, pt1, pt2);


		//combine ���� �� �ּ� �� pair�� �����Ǿ��� ���
		//��ȯ�� ���� ���
		if (*pt1 != *pt2)
			dist = EUCLIDEAN(X, Y, *pt1, *pt2);
	}
	return sqrt(dist);
}
