#include <string>

/*int�� ���� 3�� �� ���� ���� ���� ã�� ��ȯ�ϴ� �Լ�*/
int Minimum(int ins, int del, int subs) {
	int temp = ins;
	if (temp > del) temp = del;
	if (temp > subs) temp = subs;
	return temp;
}

/*int�� ���� 3�� �� ���� ���� ������ ã�� �ش� flag�� ��ȯ�ϴ� �Լ�*/
//OP cost�� ����Ͽ� �ش��ϴ� OP flag�� ��ȯ
//flag = 0 : insert
//flag = 1 : delete
//flag = 2 : substitution or bypass
int Minimum_Backtracing(int ins, int del, int subs, int ci, int cd, int cs) {
	int temp = ins;
	int flag = 0;

	if (temp > del) { temp = del; flag = 1; }
	else if (temp == del) {
		//ins_cost > del_cost�� ��� delete�� �����ߴٰ� �Ǵ�
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
	/*----------------------------------�ʱ� �� ����-------------------------------------*/
	int SN = strlen(SS), TN = strlen(TS);	//�� String�� length
	int alpha = 0;							//Substitution�� ���, �߰��ؾ��ϴ� cost�� ��� ����

	for (int i = 0; i <= SN; i++)
		Table[i][0] = i * del_cost;

	for (int i = 0; i <= TN; i++)
		Table[0][i] = i * ins_cost;

	/*---------------------Bottom-Up���� Cost Table Calculated �ۼ�----------------------*/
	for (int i = 1; i <= SN; i++) {
		for (int j = 1; j <= TN; j++) {
			if (SS[i-1] != TS[j-1]) alpha = sub_cost;	//�ش� ��ġ�� ���ڰ� �ٸ� ���(Subsitution)
			else alpha = 0;								//�ش� ��ġ�� ���ڰ� ���� ���(Bypass)

			//�ּ� cost�� ����
			Table[i][j] = Minimum(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha);
		}
	}
	
	/*--------------------SR, TR, OP�� memory �Ҵ��� ���� OP Ƚ�� ���---------------------*/

	//Backtracing�� �ϴ� �������� OP_cnt ������ counting
	//Table[m][n]�� ��, O(m + n - 2)�� �ð����⵵�� ������,
	//OP Ƚ���� ���� ���������� ���� �� �ִ� ���
	//�������� tracing �� ����� �ǽõ��� �ʴ´�.
	int i = SN, j = TN, flag = -1, OP_cnt = 0;

	while (1) {
		if (SS[i-1] != TS[j-1]) alpha = sub_cost;	//�ش� ��ġ�� ���ڰ� �ٸ� ���(Subsitution)
		else alpha = 0;								//�ش� ��ġ�� ���ڰ� ���� ���(Bypass)

		flag = Minimum_Backtracing(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha, ins_cost, del_cost, sub_cost);

		switch (flag) {
		case 0: { j--; break; }						//insert�� �����ߴٰ� �Ǵ�
		case 1: { i--; break; }						//delete�� �����ߴٰ� �Ǵ�
		case 2: { i--; j--; break; }				//substitution or bypass�� �����ߴٰ� �Ǵ�
		default: break;}

		OP_cnt++;

		//insert or delete�� ������ �� ���� ��Ȳ�� �ٴٸ� ��� Ż��
		if (i == 0 || j == 0) break;				
	}

	//���� index j��ŭ insert�� ����
	if (i == 0) { while (j > 0) { j--; OP_cnt++; } }

	//���� index i��ŭ delete�� ����
	else { while (i > 0) { i--; OP_cnt++; } }


	/*----------------------SR, TR, OP�� memory �Ҵ� �� Mem_Allocated ����-------------------*/

	//��� OP ���� Ƚ����ŭ ������ �ּ� ũ�⸦ ������,
	//String�� Ư�� �� ���� �˸��� null�� �����ϱ� ���� sizeofchar*1�� �߰�
	*SR = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	*TR = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	*OP = (char*)malloc(sizeof(char) * (OP_cnt + 1));
	(*OP)[OP_cnt] = '\0';
	(*SR)[OP_cnt] = '\0';
	(*TR)[OP_cnt] = '\0';

	//char = 1byte�̹Ƿ� �Ҵ緮��ŭ�� ����
	*Mem_Allocated = 3 * (OP_cnt + 1);


	/*-------------------------------------Backtracing-------------------------------------*/
	i = SN, j = TN, flag = -1;
	bool bSubs = false;

	while (1) {
		if (SS[i-1] != TS[j-1]) { alpha = sub_cost; bSubs = true; }	//�ش� ��ġ�� ���ڰ� �ٸ� ���(Subsitution)
		else { alpha = 0; bSubs = false; }							//�ش� ��ġ�� ���ڰ� ���� ���(Bypass)

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

		//insert or delete�� ������ �� ���� ��Ȳ�� �ٴٸ� ��� Ż��
		if (i == 0 || j == 0) break;
	}

	//���� index j��ŭ insert�� ����
	if (i == 0) {
		while (j > 0) {
			(*OP)[--OP_cnt] = 'i';
			(*SR)[OP_cnt] = '*';
			(*TR)[OP_cnt] = TS[j-1];
			j--; 
		}
	}
	//���� index i��ŭ delete�� ����
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

