#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
/*

	DATE : 2020-05-28
	AUTHOR : MINJEONG SEO 
	SUMMARY :
		������ snapshot�� �������� ���μ����� �Ҵ����� ��,
		�Ҵ� �������� Resource-Request Algorithm�� Safety Algorithm�� �̿��Ͽ� �Ǵ��ϴ� ���α׷�.

*/

using namespace std;
int process_num;
int resourcetype_num;
int req_process_num;
vector<int> v_inst;
vector<vector<int>> v_alloc;
vector<vector<int>> v_max;
vector<vector<int>> v_need;
vector<int> v_avail;
vector<int> v_req;
vector<int> v_safe;
queue<pair<int, vector<int>>> wait_queue;


void UpdateValue() {
	for (int i = 0; i < resourcetype_num; i++) {
		v_avail[i] = v_avail[i] - v_req[i];
		v_need[req_process_num][i] = v_need[req_process_num][i] - v_req[i];
		v_alloc[req_process_num][i] = v_alloc[req_process_num][i] + v_req[i];
	}
}

int Safety() {
	UpdateValue();
	vector<int> work(v_avail);
	vector<bool> finish(process_num, false);
	// finish[req_process_num] = true;
	int cnt = 0;
	while (true) {
		for (int i = 0; i < process_num; i++) {
			if (finish[i]) continue;
			bool find = true;
			for (int j = 0; j < resourcetype_num; j++) {
				if (!find) break;
				if (v_need[i][j] > work[j]) {
					find = false;
				}
			}
			if (find && !finish[i]) {
				for (int j = 0; j < resourcetype_num; j++) {
					work[j] += v_alloc[i][j];
				}
				finish[i] = true;
				v_safe.push_back(i);
				cnt++;
				i = -1;
			}
		}
		if (cnt == process_num) return 1;
		else return 0;
	}
}

void ResourceRequest() {
	int size = wait_queue.size();

	cout << "\n\n== ���μ����� ��û�մϴ� ==\n";

	if (size != 0) {
		cout << "\n** Warning **\n";
		cout << "Available�� ��û���� �Ѿ ��� ���� ���μ����� " << size << " �� �����մϴ�.\n";
		cout << "��Ȱ�� �Ҵ��� ���� ������ ���� �Ҵ��ϼ���.\n\n";
	}

	cout << "[�Է� ����]\n";
	cout << "P1 requests (1,0,2)�� ��쿡�� \"1 1 0 2\"�� ���� �Է����ּ���!\n";
	cout << "==> ";
	
	cin >> req_process_num;
	for (int i = 0; i < resourcetype_num; i++) {
		int tmp;
		cin >> tmp;
		v_req.push_back(tmp);
	}

	for (int i = 0; i < resourcetype_num; i++) {
		if (v_req[i] > v_need[req_process_num][i]) {
			cout << "\nError! ��û���� Need�� �ʰ��Ͽ� �Ҵ� �Ұ��մϴ�.";
			cout << "\n���α׷��� �����մϴ�.";
			exit(0);
		}

		if (v_req[i] > v_avail[i]) {
			cout << "\n��û�� Available���� ũ�Ƿ� �ش� ���μ��� "<< req_process_num+1 <<"�� �ش� snapshot���� �Ҵ��� �� ������ ����ؾ��մϴ�.";
			cout << "\n���α׷��� �����մϴ�.";
			exit(0);
		}

		cout << "\n��û�� ���μ����� �Ҵ��� �� �ִ��� Ȯ���մϴ� ......\n\n";
		if (Safety()) {
			cout << "RESULT :: ��û�� ���μ����� �Ҵ� �����մϴ�!\n";
			cout << "Safety �˰����� ������ ���, ������";
			cout << "<";
			for (int i = 0; i < v_safe.size(); i++) {
				if (i == v_safe.size() - 1) {
					cout << "P" << v_safe[i];
					continue;
				}
				cout << "P" << v_safe[i] << ", ";
			} cout << ">";
			cout << "�� Safety Requirement�� �����ϴ� ���� Ȯ���� �� �ֽ��ϴ�.\n\n";
			return;
		}
		else {
			cout << "RESULT :: ��û�� ���μ����� �Ҵ��Ͽ��� ��, �ý����� �Ҿ����ϹǷ� �Ҵ�Ұ��մϴ�.";
			cout << "\n�ش� ���μ��� " << req_process_num + 1 << "�� �ش� snapshot���� �Ҵ��� �� �����Ƿ� ����ؾ��մϴ�.\n";
			cout << "\n���α׷��� �����մϴ�.\n\n";
			return;
		}
	}

}

void InitFromSample() {
	cout << "\n���� �����ʹ� os-ch07 �ڷ� p.26�� �־��� �������Դϴ�.";
	process_num = 5;
	resourcetype_num = 3;
	v_inst = { 10, 5, 7 };
	v_alloc = {
		{0, 1, 0}, {2, 0, 0},{3, 0 ,2},{2, 1, 1},{0, 0, 2}
	};
	v_max = {
		{7, 5, 3}, {3, 2, 2},{9, 0 ,2},{2, 2, 2},{4, 3, 3}
	};

	//Init Available
	for (int i = 0; i < resourcetype_num; i++) {
		v_avail.push_back(v_inst[i]);
	}

	for (int i = 0; i < process_num; i++) {
		vector<int> inputNeed;
		for (int j = 0; j < resourcetype_num; j++) {
			int tmp;
			tmp = v_max[i][j] - v_alloc[i][j];
			inputNeed.push_back(tmp);
		}
		v_need.push_back(inputNeed);
	}

	for (int i = 0; i < resourcetype_num; i++) {
		for (int j = 0; j < process_num; j++) {
			v_avail[i] = v_avail[i] - v_alloc[j][i];
		}
	}

	ResourceRequest();
}

void Input() {
	cout << "[���μ��� ���� �Է� (2~10)]\n";
	cout << "==> ";
	cin >> process_num;
	if (process_num < 2 && process_num > 10) {
		cout << "Error! ���μ����� ������ 2�� �̻�, 10�� ���ϸ� �����մϴ�.\n";
		cout << "���μ����� �����մϴ�.\n";
		exit(0);
	}
	cout << "\n[�ڿ� Ÿ�� ���� �Է� (1~5)]\n";
	cout << "==> ";
	cin >> resourcetype_num;
	if (resourcetype_num < 1 && resourcetype_num > 5) {
		cout << "Error! �ڿ� Ÿ���� ������ 1�� �̻�, 5�� ���ϸ� �����մϴ�.\n";
		cout << "���μ����� �����մϴ�.\n";
		exit(0);
	}
	cout << "\n[�ڿ� Ÿ�Ժ� �ν��Ͻ� ���� �Է� (1~10)]\n\n";
	for (int i = 0; i < resourcetype_num; i++) {
		cout << i + 1 << "��° �ڿ��� �ν��Ͻ� ������ �Է��ϼ��� ==> ";
		int instance;
		cin >> instance;
		if (instance < 1 && instance > 10) {
			cout << "Error! �ν��Ͻ� ������ 1�� �̻�, 10�� ���ϸ� �����մϴ�.\n";
			cout << "���μ����� �����մϴ�.\n";
			exit(0);
		}
		v_inst.push_back(instance);
	}
	cout << "\n[���μ����� �ڿ� �Ҵ�]\n\n";
	cout << "(����) P0�� �ڿ� A,B,C�� �� 0,1,0 ��ŭ �Ҵ��ϴ� ��쿡�� \"0 1 0\" �� ���� �Է����ּ���.\n";
	for (int i = 0; i < process_num; i++) {
		cout << i << "��° ���μ����� �Ҵ��ϼ��� ==> ";
		vector<int> inputAlloc;
		for (int j = 0; j < resourcetype_num; j++) {
			int i_alloc;
			cin >> i_alloc;
			if (i_alloc > v_inst[j]) {
				cout << "Error! �Ҵ� ������ �ν��Ͻ��� ������ �ʰ��Ͽ����ϴ�.\n";
				cout << "���μ����� �����մϴ�.\n";
				exit(0);
			}
			inputAlloc.push_back(i_alloc);
		}
		v_alloc.push_back(inputAlloc);
	}
	cout << "\n[���μ����� MAX �Ҵ�]\n\n";
	cout << "(����) P0�� �ڿ� A,B,C�� MAX�� �� 7,5,3�� ��쿡�� \"7 5 3\" �� ���� �Է����ּ���.\n";
	for (int i = 0; i < process_num; i++) {
		cout << i << "��° ���μ����� MAX ���� �Ҵ��ϼ��� ==> ";
		vector<int> inputMAX;
		for (int j = 0; j < resourcetype_num; j++) {
			int i_max;
			cin >> i_max;
			if (i_max > v_inst[j]) {
				cout << "Error! MAX ���� �ν��Ͻ��� ������ �ʰ��Ͽ����ϴ�.\n";
				cout << "���μ����� �����մϴ�.\n";
				exit(0);
			}
			inputMAX.push_back(i_max);
		}
		v_max.push_back(inputMAX);
	}

	//Init Available
	for (int i = 0; i < resourcetype_num; i++) {
		v_avail.push_back(v_inst[i]);
	}

	for (int i = 0; i < process_num; i++) {
		vector<int> inputNeed;
		for (int j = 0; j < resourcetype_num; j++) {
			int tmp;
			tmp = v_max[i][j] - v_alloc[i][j];
			inputNeed.push_back(tmp);
		}
		v_need.push_back(inputNeed);
	}

	for (int i = 0; i < resourcetype_num; i++) {
		for (int j = 0; j < process_num; j++) {
			v_avail[i] = v_avail[i] - v_alloc[j][i];
		}
	}

	ResourceRequest();
}
int main(void) {
	cout << "[Banker's Algorithm (Deadlock)]\n\n";
	cout << "1. Sample�� �ڵ� �Ҵ��ϱ�\n";
	cout << "2. ���� �Ҵ��ϱ�\n";
	
	int cmd;
	cin >> cmd;

	switch (cmd) {
	case 1:
		InitFromSample();
		break;
	case 2:
		Input();
		break;
	}


	return 0;
}