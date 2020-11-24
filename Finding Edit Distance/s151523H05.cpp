#include <string>

/*int형 변수 3개 중 가장 작은 값을 찾아 반환하는 함수*/
int Minimum(int ins, int del, int subs) {
	int temp = ins;
	if (temp > del) temp = del;
	if (temp > subs) temp = subs;
	return temp;
}

/*int형 변수 3개 중 가장 작은 변수를 찾아 해당 flag를 반환하는 함수*/
//OP cost를 고려하여 해당하는 OP flag를 반환
//flag = 0 : insert
//flag = 1 : delete
//flag = 2 : substitution or bypass
int Minimum_Backtracing(int ins, int del, int subs, int ci, int cd, int cs) {
	int temp = ins;
	int flag = 0;

	if (temp > del) { temp = del; flag = 1; }
	else if (temp == del) {
		//ins_cost > del_cost인 경우 delete를 수행했다고 판단
		if (ci > cd) flag = 1;
	}

	if (temp > subs) { temp = subs; flag = 2; }
	return flag;
}

/*Edit Distance*/
void Edit_Distance(
	// inputs
	char *SS,		// Source string array
	char *TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
	// outputs
	int **Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char **SR,		// Source string with insertion to T marks '*'
	char **OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
	char **TR,       // Edited string with deletion form S mark '*'
				// NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int  *Mem_Allocated
) {
	/*----------------------------------초기 값 설정-------------------------------------*/
	int SN = strlen(SS), TN = strlen(TS);	//각 String의 length
	int alpha = 0;							//Substitution일 경우, 추가해야하는 cost를 담는 변수

	for (int i = 0; i <= SN; i++)
		Table[i][0] = i * del_cost;

	for (int i = 0; i <= TN; i++)
		Table[0][i] = i * ins_cost;

	/*---------------------Bottom-Up으로 Cost Table Calculated 작성----------------------*/
	for (int i = 1; i <= SN; i++) {
		for (int j = 1; j <= TN; j++) {
			if (SS[i-1] != TS[j-1]) alpha = sub_cost;	//해당 위치의 문자가 다를 경우(Subsitution)
			else alpha = 0;								//해당 위치의 문자가 같을 경우(Bypass)

			//최소 cost를 저장
			Table[i][j] = Minimum(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha);
		}
	}
	
	/*--------------------SR, TR, OP의 memory 할당을 위한 OP 횟수 계산---------------------*/

	//Backtracing을 하는 과정마다 OP_cnt 변수로 counting
	//Table[m][n]일 때, O(m + n - 2)의 시간복잡도를 가지며,
	//OP 횟수를 가장 직관적으로 구할 수 있는 방법
	//실질적인 tracing 및 기록은 실시되지 않는다.
	int i = SN, j = TN, flag = -1, OP_cnt = 0;

	while (1) {
		if (SS[i-1] != TS[j-1]) alpha = sub_cost;	//해당 위치의 문자가 다를 경우(Subsitution)
		else alpha = 0;								//해당 위치의 문자가 같을 경우(Bypass)

		flag = Minimum_Backtracing(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha, ins_cost, del_cost, sub_cost);

		switch (flag) {
		case 0: { j--; break; }						//insert를 수행했다고 판단
		case 1: { i--; break; }						//delete를 수행했다고 판단
		case 2: { i--; j--; break; }				//substitution or bypass를 수행했다고 판단
		default: break;}

		OP_cnt++;

		//insert or delete를 수행할 수 없는 상황에 다다른 경우 탈출
		if (i == 0 || j == 0) break;				
	}

	//남은 index j만큼 insert를 수행
	if (i == 0) { while (j > 0) { j--; OP_cnt++; } }

	//남은 index i만큼 delete를 수행
	else { while (i > 0) { i--; OP_cnt++; } }


	/*----------------------SR, TR, OP의 memory 할당 및 Mem_Allocated 저장-------------------*/

	//모두 OP 실행 횟수만큼 동일한 최소 크기를 가지며,
	//String의 특성 상 끝을 알리는 null을 저장하기 위해 sizeofchar*1을 추가
	*SR = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	*TR = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	*OP = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	(*OP)[OP_cnt] = '\0';
	(*SR)[OP_cnt] = '\0';
	(*TR)[OP_cnt] = '\0';

	//char = 1byte이므로 할당량만큼만 저장
	*Mem_Allocated = 3 * (OP_cnt + 1);


	/*-------------------------------------Backtracing-------------------------------------*/
	i = SN, j = TN, flag = -1;
	bool bSubs = false;

	while (1) {
		if (SS[i-1] != TS[j-1]) { alpha = sub_cost; bSubs = true; }	//해당 위치의 문자가 다를 경우(Subsitution)
		else { alpha = 0; bSubs = false; }							//해당 위치의 문자가 같을 경우(Bypass)

		flag = Minimum_Backtracing(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha, ins_cost, del_cost, sub_cost);
		OP_cnt--;

		switch (flag) {
		//insert
		case 0: {					
			(*OP)[OP_cnt] = 'i';
			(*SR)[OP_cnt] = '*';
			(*TR)[OP_cnt] = TS[j-1];
			j--; 
			break; }
		
		//delete
		case 1: { 
			(*OP)[OP_cnt] = 'd';
			(*SR)[OP_cnt] = SS[i-1];
			(*TR)[OP_cnt] = '*';
			i--; 
			break; }

		case 2: {
			//substitution
			if(bSubs) (*OP)[OP_cnt] = 's';
			//bypass
			else (*OP)[OP_cnt] = '.';

			(*SR)[OP_cnt] = SS[i-1];
			(*TR)[OP_cnt] = TS[j-1];
			i--; j--; 
			break; }

		default: break;}

		//insert or delete를 수행할 수 없는 상황에 다다른 경우 탈출
		if (i == 0 || j == 0) break;
	}

	//남은 index j만큼 insert를 수행
	if (i == 0) {
		while (j > 0) {
			(*OP)[--OP_cnt] = 'i';
			(*SR)[OP_cnt] = '*';
			(*TR)[OP_cnt] = TS[j-1];
			j--; 
		}
	}
	//남은 index i만큼 delete를 수행
	else {
		while (i > 0) { 
			(*OP)[--OP_cnt] = 'd';
			(*SR)[OP_cnt] = SS[i-1];
			(*TR)[OP_cnt] = '*';
			i--;  
		}
	}
	return;
}

