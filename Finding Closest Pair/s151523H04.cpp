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
	mid+1 ~ high 사이에 검사하지 않은 elements 존재 시
	순서 그대로 넣어주기
	*/
	if (i > mid) {
		for (int l = j; l <= high; l++) {
			id[k] = TMP[l];
			k++;
		}
	}

	/*
	low ~ mid 사이에 검사하지 않은 elements 존재 시
	순서 그대로 넣어주기
	*/
	else {
		for (int l = i; l <= mid; l++) {
			id[k] = TMP[l];
			k++;
		}
	}

	/*TMP로 다시 복사*/
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
	Selection Sort : THS보다 적은 갯수의 Yid를 정렬하기 위해 사용
	min : 최소값을 가리키는 Index
	*/
	unsigned i, j, min, temp;
	int n = R - L + 1;

	for (i = L; i < R; i++) {						
		min = i;

		for (j = i + 1; j <= R; j++) {				// Y값 중 최솟값을 탐색
			if (Y[Yid[j]] < Y[Yid[min]])
				min = j;
		}

		
		if (i != min) {							// 최솟값이 현재값과 다를 때 Swap
			SWAP(Yid[i], Yid[min], temp);
		}
	}
}

/*Xid 정렬 함수*/
void sortXid(double *X, unsigned *Xid, unsigned *TMP, unsigned N) {
	/*TMP에 [0...n-1] 인덱스를 넣고
	mergesort 호출*/
	for (int i = 0; i < N; i++) 
		TMP[i] = i;
	
	mergesort(0, N - 1, X, TMP, Xid);
}
/*closestPairDC의 combine 함수
(x좌표가 dl, dr에 하나씩 존재하는 pair에서 조사)*/
void combine(
	unsigned* Yid, 
	unsigned L, unsigned M, unsigned R, double dlr, 
	double* X, double* Y, unsigned* Xid, unsigned* TMP, 
	unsigned *pt1, unsigned *pt2){

	double Xmid, dist = dlr;
	int count = 0;

	//Xmid를 기준으로 pt.x +- dlr만큼의 point pair index 추출
	Xmid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;
	for (int i = L; i <= R; i++) {
		if (Xmid - dlr <= X[Yid[i]] && X[Yid[i]] <= Xmid + dlr){
			TMP[count] = Yid[i];
			count++;
		}
	}

	for (int i = 0; i < count - 1; i++) {
		for (int j = i + 1; j < count; j++) {
			if (Y[TMP[j]] - Y[TMP[i]] >= dlr) break;			//Y 좌표가 dlr이상 차이나는 pair부터는 break

			if (dist > sqrt(EUCLIDEAN(X, Y, TMP[i], TMP[j]))) {	//지금까지 dist보다 작은 값이 나올 경우 dist, pt update
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
	아직 pt1, pt2가 한번도 결정되지않아 변수 초기화 시의 초기값을 가지고 있는 경우이므로
	0과 1을 임의로 배정한 뒤 dist 결정

	2) *pt2 != *pt2 :
	이전 시행에서 pt가 결정된 바가 존재하는 경우이므로
	이를 바탕으로 dist 결정
	*/
	if (*pt1 != *pt2) 
		dist = EUCLIDEAN(X, Y, *pt1, *pt2);
	else {
		*pt1 = 0; *pt2 = 1;
		dist = EUCLIDEAN(X, Y, 0, 1);
	}

	/*
	Threshold >= size :
	BruteForce Method로 직접 계산
	*/
	if (R - L + 1 <= THR) {

		//Yid 정렬
		for (int i = L; i <= R; i++)
			Yid[i] = Xid[i];
		selectionsort(Y, Yid, L, R);

		for (unsigned i = L; i < R; i++) {
			for (unsigned j = i + 1; j <= R; j++) {
				if (dist > EUCLIDEAN(X, Y, Xid[i], Xid[j])) {	//지금까지 dist보다 작은 값이 나올 경우 dist, pt update
					dist = EUCLIDEAN(X, Y, Xid[i], Xid[j]);
					*pt1 = Xid[i];
					*pt2 = Xid[j];
				}
			}
		}
	}

	/*
	Threshold < size :
	D&C Method 사용
	*/
	else {
		M = (L + R) / 2;
		dl = closestPairDC(L, M, pt1, pt2, X, Y, Xid, Yid, TMP, THR);		//왼쪽 영역
		dr = closestPairDC(M + 1, R, pt1, pt2, X, Y, Xid, Yid, TMP, THR);	//오른쪽 영역

		//각 divided space에서 정렬된 Yid를 merge
		for (int i = L; i <= R; i++)
			TMP[i] = Yid[i];
		merge(L, M, R, Y, TMP, Yid);
		
		//min(dl, dr)
		dlr = dl;
		if (dl > dr)
			dlr = dr;

		combine(Yid, L, M, R, dlr, X, Y, Xid, TMP, pt1, pt2);


		//combine 시행 후 최소 점 pair가 결정되었을 경우
		//반환을 위해 계산
		if (*pt1 != *pt2)
			dist = EUCLIDEAN(X, Y, *pt1, *pt2);
	}
	return sqrt(dist);
}
