#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
/*

	DATE : 2020-05-28
	AUTHOR : MINJEONG SEO 
	SUMMARY :
		최초의 snapshot을 바탕으로 프로세스를 할당했을 때,
		할당 가능한지 Resource-Request Algorithm과 Safety Algorithm을 이용하여 판단하는 프로그램.

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

	cout << "\n\n== 프로세스를 요청합니다 ==\n";

	if (size != 0) {
		cout << "\n** Warning **\n";
		cout << "Available한 요청량을 넘어서 대기 중인 프로세스가 " << size << " 개 존재합니다.\n";
		cout << "원활한 할당을 위해 적절한 값을 할당하세요.\n\n";
	}

	cout << "[입력 예시]\n";
	cout << "P1 requests (1,0,2)인 경우에는 \"1 1 0 2\"와 같이 입력해주세요!\n";
	cout << "==> ";
	
	cin >> req_process_num;
	for (int i = 0; i < resourcetype_num; i++) {
		int tmp;
		cin >> tmp;
		v_req.push_back(tmp);
	}

	for (int i = 0; i < resourcetype_num; i++) {
		if (v_req[i] > v_need[req_process_num][i]) {
			cout << "\nError! 요청량이 Need를 초과하여 할당 불가합니다.";
			cout << "\n프로그램을 종료합니다.";
			exit(0);
		}

		if (v_req[i] > v_avail[i]) {
			cout << "\n요청이 Available보다 크므로 해당 프로세스 "<< req_process_num+1 <<"는 해당 snapshot에서 할당할 수 없으며 대기해야합니다.";
			cout << "\n프로그램을 종료합니다.";
			exit(0);
		}

		cout << "\n요청한 프로세스를 할당할 수 있는지 확인합니다 ......\n\n";
		if (Safety()) {
			cout << "RESULT :: 요청한 프로세스는 할당 가능합니다!\n";
			cout << "Safety 알고리즘을 실행한 결과, 시퀀스";
			cout << "<";
			for (int i = 0; i < v_safe.size(); i++) {
				if (i == v_safe.size() - 1) {
					cout << "P" << v_safe[i];
					continue;
				}
				cout << "P" << v_safe[i] << ", ";
			} cout << ">";
			cout << "가 Safety Requirement를 충족하는 것을 확인할 수 있습니다.\n\n";
			return;
		}
		else {
			cout << "RESULT :: 요청한 프로세스를 할당하였을 때, 시스템이 불안전하므로 할당불가합니다.";
			cout << "\n해당 프로세스 " << req_process_num + 1 << "는 해당 snapshot에서 할당할 수 없으므로 대기해야합니다.\n";
			cout << "\n프로그램을 종료합니다.\n\n";
			return;
		}
	}

}

void InitFromSample() {
	cout << "\n샘플 데이터는 os-ch07 자료 p.26에 주어진 데이터입니다.";
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
	cout << "[프로세스 개수 입력 (2~10)]\n";
	cout << "==> ";
	cin >> process_num;
	if (process_num < 2 && process_num > 10) {
		cout << "Error! 프로세스의 개수는 2개 이상, 10개 이하만 가능합니다.\n";
		cout << "프로세스를 종료합니다.\n";
		exit(0);
	}
	cout << "\n[자원 타입 개수 입력 (1~5)]\n";
	cout << "==> ";
	cin >> resourcetype_num;
	if (resourcetype_num < 1 && resourcetype_num > 5) {
		cout << "Error! 자원 타입의 개수는 1개 이상, 5개 이하만 가능합니다.\n";
		cout << "프로세스를 종료합니다.\n";
		exit(0);
	}
	cout << "\n[자원 타입별 인스턴스 개수 입력 (1~10)]\n\n";
	for (int i = 0; i < resourcetype_num; i++) {
		cout << i + 1 << "번째 자원의 인스턴스 개수를 입력하세요 ==> ";
		int instance;
		cin >> instance;
		if (instance < 1 && instance > 10) {
			cout << "Error! 인스턴스 개수는 1개 이상, 10개 이하만 가능합니다.\n";
			cout << "프로세스를 종료합니다.\n";
			exit(0);
		}
		v_inst.push_back(instance);
	}
	cout << "\n[프로세스별 자원 할당]\n\n";
	cout << "(예시) P0의 자원 A,B,C를 각 0,1,0 만큼 할당하는 경우에는 \"0 1 0\" 과 같이 입력해주세요.\n";
	for (int i = 0; i < process_num; i++) {
		cout << i << "번째 프로세스를 할당하세요 ==> ";
		vector<int> inputAlloc;
		for (int j = 0; j < resourcetype_num; j++) {
			int i_alloc;
			cin >> i_alloc;
			if (i_alloc > v_inst[j]) {
				cout << "Error! 할당 가능한 인스턴스의 개수를 초과하였습니다.\n";
				cout << "프로세스를 종료합니다.\n";
				exit(0);
			}
			inputAlloc.push_back(i_alloc);
		}
		v_alloc.push_back(inputAlloc);
	}
	cout << "\n[프로세스별 MAX 할당]\n\n";
	cout << "(예시) P0의 자원 A,B,C의 MAX가 각 7,5,3인 경우에는 \"7 5 3\" 과 같이 입력해주세요.\n";
	for (int i = 0; i < process_num; i++) {
		cout << i << "번째 프로세스의 MAX 값을 할당하세요 ==> ";
		vector<int> inputMAX;
		for (int j = 0; j < resourcetype_num; j++) {
			int i_max;
			cin >> i_max;
			if (i_max > v_inst[j]) {
				cout << "Error! MAX 값이 인스턴스의 개수를 초과하였습니다.\n";
				cout << "프로세스를 종료합니다.\n";
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
	cout << "1. Sample로 자동 할당하기\n";
	cout << "2. 직접 할당하기\n";
	
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