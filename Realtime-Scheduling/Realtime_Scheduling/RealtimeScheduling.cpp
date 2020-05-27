#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>
#include <cstring>
#include <queue>
#include <cmath>
using namespace std;

//polling의 capacity과 period를 저장할 pair 변수
pair<int, int> poll;
//주기적 태스크의 period와 computation time을 저장할 벡터
vector<pair<int, int>> o_t;
//비주기적 태스크의 arrival time과 computation time을 저장할 벡터
vector<pair<int, int>> o_a;

//비주기적 태스크가 실행되었는지 확인하기 위한 변수
int ap_visit[3];
// 주기적 태스크가 computation time만큼 실행되었는지 확인하기 위한 변수
int cnt[5];
// 주기적 태스크가 한 주기를 돌았을 때를 확인하기 위한 변수
bool chk_period[5];
//태스크의 총 개수
int tSize = 5;
//스케쥴링된 task들을 담기 위한 변수
vector<int> scheduling;
//hyper period
int hyper;

void init() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}

//주기적, 비주기적, polling server를 입력받는 함수
void input() {
	cout << "\n **Realtime Scheduling **\n";
	cout << "First, Input 5 periodic tasks ( Computation time / Period ).\n";
	cout <<"cf. If P0 has Computation time is 1, Period 5 then you enter like 1 5.\n";
	for (int i = 0; i < 5; i++) {
		int tmp1, tmp2;
		cin >> tmp1 >> tmp2;
		o_t.push_back(make_pair(tmp2, tmp1));
	}
	cout << "\nSecond, Input 3 Aperiodic tasks (Computation time / Arrival Time).\n";
	cout <<  "cf. If AP0 has Computation time is 1, Arrival Time is 7 then you enter like 1 7.\n";
	for (int i = 0; i < 3; i++) {
		int tmp1, tmp2;
		cin >> tmp1 >> tmp2;
		o_a.push_back(make_pair(tmp2, tmp1));
	}

	cout << "\nLast, Input 1 Polling server information\n";
	cout << "cf. If the Polling server has Capacity is 1, Period 5 then you enter like 1 5.\n";
	int c, t;
	cin >> c >> t;
	poll = make_pair(c, t);

	sort(o_t.begin(), o_t.end());
	sort(o_a.begin(), o_a.end());
}

//hyperperiod를 구하기 위한 LCM 함수
int getLCM() {
	int lcm = 1;
	int max = 0;
	int cnt;
	for (int i = 0; i < tSize; i++) {
		if (o_t[i].first > max)
			max = o_t[i].first;
	}

	for (int i = 1; i < 1000; i++) {
		cnt = 0;
		for (int j = 0; j < tSize; j++) {
			if ((max * i) % o_t[j].first == 0) cnt++;
		}
		if (cnt == tSize) {
			lcm = max * i;
			break;
		}
	}
	return lcm;
}

// gantt chart 형식으로 프린트하기 위한 함수
void print() {
	for (int i = 0; i < hyper; i++) {
		cout << "+--";
	}cout << "+\n";

	cout << "|";
	for (int i = 0; i < scheduling.size(); i++) {
		if (scheduling[i] == 0) {
			cout << "P0|";
		}
		else if (scheduling[i] == 1) {
			cout << "P1|";
		}
		else if (scheduling[i] == 2) {
			cout << "P2|";
		}
		else if (scheduling[i] == 3) {
			cout << "P3|";
		}
		else if (scheduling[i] == 4) {
			cout << "P4|";
		}
		else if (scheduling[i] == -1) {
			cout << "A0|";
		}
		else if (scheduling[i] == -2) {
			cout << "A1|";
		}
		else if (scheduling[i] == -3) {
			cout << "A2|";
		}
		else {
			cout << "  |";
		}
	}cout << "\n";

	for (int i = 0; i < hyper; i++) {
		cout << "+--";
	}cout << "+\n";

	cout << "|";
	for (int i = 0; i < hyper; i++) {
		if (scheduling[i] == 0) {
			cout << "|||";
		}
		else if (scheduling[i] == 1) {
			cout << "//|";
		}
		else if (scheduling[i] == 2) {
			cout << "==|";
		}
		else if (scheduling[i] == 3) {
			cout << "::|";
		}
		else if (scheduling[i] == 4) {
			cout << "##|";
		}
		else {
			cout << "  |";
		}
	} cout << "\n";

	for (int i = 0; i < hyper; i++) {
		cout << "+--";
	}cout << "+\n";

	cout << "|";
	for (int i = 0; i < hyper; i++) {
		if (i > 9) {
			cout << i << "|";
		}
		else cout << i << " |";
	}cout << "\n";

	for (int i = 0; i < hyper; i++) {
		cout << "+--";
	}cout << "+\n";
}

//polling 구현
void polling() {
	//함수 내에서 직접적으로 값 변경이 있으므로 o_t와 o_c를 복사한다.
	vector<int> tt(5), tc(5);
	for (int i = 0; i < 5; i++) {
		tt[i] = o_t[i].first;
		tc[i] = o_t[i].second;
	}
	vector<int> at(3), ac(3);
	for (int i = 0; i < 3; i++) {
		at[i] = o_a[i].first;
		ac[i] = o_a[i].second;
	}

	//각 변수들을 초기화한다.
	memset(cnt, 0, sizeof(cnt));
	memset(chk_period, true, sizeof(chk_period));
	memset(ap_visit, 0, sizeof(ap_visit));

	hyper = getLCM();
	cout << "Hyperperiod: " << hyper << "\n";
	int polling_exec = 0;
	int polling_period = poll.second;
	vector<int>::iterator it;
	int t = -1;
	int ap_exec[3];
	memset(ap_exec, 0, sizeof(ap_exec));

	bool preem = false;
	int idx = 0;
	int saveIdx = 0;
	queue<int> que_preem;
	queue<int> que_polling;

	bool done = true; //정상종료 확인을 위한 변수
	while (t < hyper - 1) {
		t++;
		//deadline check
		for (int i = 0; i < 5; i++) {
			if (tt[i] <= t && chk_period[i] == true) {
				done = false;
				break;
			}
		}

		//polling 주기마다 polling을 구현함.
		if (t % polling_period == 0 && t != 0) {
			polling_exec = poll.first;
			for (int i = 0; i < 3; i++) {
				if (at[i] <= t && ap_visit[i] == 0) {
					int j = ac[i];
					while (j > 0) {
						que_polling.push(-(i + 1));
						j--;
					}
					ap_visit[i] = 1;
				}
			}
		}

		it = find(tt.begin(), tt.end(), t);
		if (it != tt.end()) { //주기 발견
			int dup_period = count(tt.begin(), tt.end(), t); //해당 주기에 겹치는 태스크의 개수를 셂.
			saveIdx = idx;
			idx = distance(tt.begin(), it); //인덱스 저장
			chk_period[idx] = true;
			tt[idx] = tt[idx] + o_t[idx].first; //주기를 더해줌 (다음번 주기에서 찾기 위해)
			preem = true;

			if (dup_period != 1) { //겹치는 주기가 존재하면 queue에 겹치는 태스크들을 저장함.
				while (true) {
					it = find(it + 1, tt.end(), t);
					if (it != tt.end()) {
						int next_idx = distance(tt.begin(), it);
						que_preem.push(distance(tt.begin(), it));
						chk_period[next_idx] = true;
						tt[next_idx] = tt[next_idx] + o_t[next_idx].first;
					}
					else break;
				}
			}
		}

		//실행 횟수만큼 모두 실행이 되었을 때, 다음 태스크로 넘기기
		if (idx < tSize && cnt[idx] == tc[idx]) {
			chk_period[idx] = false;
			cnt[idx] = 0;
			if (!que_preem.empty()) { //preem을 당했는데, 아직 preem된 task들이 존재하는 경우
				idx = que_preem.front();
				que_preem.pop();
			}
			else if (preem) { //preem을 당했었고, preem이 끝났을 때
				idx = saveIdx;
				if (idx < tSize) {
					if (cnt[idx] == tc[idx]) {
						chk_period[idx] = false;
						cnt[idx] = 0;
					}
				}
				preem = false;
			}
			else idx++;
		}

		if (idx >= tSize || !chk_period[idx]) {
			scheduling.push_back(-9999);
			continue;
		}
		
		if (!que_polling.empty() && polling_exec > 0) { //polling 태스크가 남아있을 때(polling capacity도 남아있고)
			int p = que_polling.front();
			ap_exec[abs(p) - 1] = t;
			scheduling.push_back(p);
			que_polling.pop();
			polling_exec--;
			continue;
		}

		scheduling.push_back(idx);
		cnt[idx] = cnt[idx] + 1;
	}

	if (done) {
		double avg_waiting_time = 0;
		for (int i = 0; i < 3; i++) {
			avg_waiting_time += (ap_exec[i] - at[i]);
		}
		avg_waiting_time /= (double)at.size();
		cout << "Average Waiting Time : " << avg_waiting_time << "\n";
		print();
	}
	else {
		cout << "The task does not end within the deadline.";
		return;
	}

	//벡터 메모리 할당 해제
	vector<int>().swap(tt);
	vector<int>().swap(tc);
	vector<int>().swap(at);
	vector<int>().swap(ac);
	vector<int>().swap(scheduling);

}

void background() {
	vector<int> tt(5), tc(5);
	for (int i = 0; i < 5; i++) {
		tt[i] = o_t[i].first;
		tc[i] = o_t[i].second;
	}

	vector<int> at(3), ac(3);
	for (int i = 0; i < 3; i++) {
		at[i] = o_a[i].first;
		ac[i] = o_a[i].second;
	}

	memset(cnt, 0, sizeof(cnt));
	memset(chk_period, true, sizeof(chk_period));
	memset(ap_visit, 0, sizeof(ap_visit));
	hyper = getLCM();
	cout << "Hyperperiod: " << hyper << "\n";

	vector<int>::iterator it;
	int t = -1;
	int ap_exec[3];
	memset(ap_exec, 0, sizeof(ap_exec));

	bool preem = false;
	int idx = 0;
	int saveIdx = 0;
	queue<int> que_preem;
	queue<int> que_ap;

	bool done = true;

	while (t < hyper-1) {
		t++;

		//deadline check
		for (int i = 0; i < 5; i++) {
			if (tt[i] <= t && chk_period[i] == true) {
				done = false;
				break;
			}
		}

		it = find(tt.begin(), tt.end(), t);
		if (it != tt.end()) { //주기 발견
			int dup_period = count(tt.begin(), tt.end(), t); //해당 주기에 겹치는 태스크의 개수를 셂.
			saveIdx = idx;
			idx = distance(tt.begin(), it); //인덱스 저장
			chk_period[idx] = true;
			tt[idx] = tt[idx] + o_t[idx].first; //주기를 더해줌 (다음번 주기에서 찾기 위해)
			preem = true;

			if (dup_period != 1) { //겹치는 주기가 존재하면 queue에 겹치는 태스크들을 저장함.
				while (true) {
					it = find(it + 1, tt.end(), t);
					if (it != tt.end()) {
						int next_idx = distance(tt.begin(), it);
						que_preem.push(distance(tt.begin(), it));
						chk_period[next_idx] = true;
						tt[next_idx] = tt[next_idx] + o_t[next_idx].first;
					}
					else break;
				}
			}
		}

		//computation time 만큼 실행된 경우
		if (idx < tSize && cnt[idx] == tc[idx]) {
			chk_period[idx] = false;
			cnt[idx] = 0;
			if (!que_preem.empty()) {
				idx = que_preem.front();
				que_preem.pop();
			}
			else if (preem) {
				idx = saveIdx;
				preem = false;
			}
			else idx++;
		}
		
		//idx가 태스크의 개수를 넘어선 경우
		if (idx >= tSize || !chk_period[idx]) {
			for (int i = 0; i < 3; i++) {
				if (at[i] <= t && ap_visit[i] == 0)  {
					int j = ac[i];
					while (j > 0) {
						que_ap.push(-(i+1));
						j--;
					}
					ap_visit[i] = 1;
				}
			}

			// 실행되지 않은 aperiodic task가 존재하는 경우
			if (!que_ap.empty()) {
				int ap = que_ap.front();
				ap_exec[abs(ap) - 1] = t;
				scheduling.push_back(ap);
				que_ap.pop();
			}
			else {
				//empty임을 나타내기 위해 -9999 삽입
				scheduling.push_back(-9999);
			}
			continue;
		}

		scheduling.push_back(idx);
		cnt[idx] = cnt[idx] + 1;
	}

	if (done) {
		double avg_waiting_time = 0;
		for (int i = 0; i < 3; i++) {
			avg_waiting_time += (ap_exec[i] - at[i]);
		}
		avg_waiting_time /= (double)at.size();
		cout << "Average Waiting Time : " << avg_waiting_time << "\n";
		print();
	}
	else {
		cout << "The task does not end within the deadline.";
		return;
	}

	//벡터 메모리 할당 해제
	vector<int>().swap(tt);
	vector<int>().swap(tc);
	vector<int>().swap(at);
	vector<int>().swap(ac);
	vector<int>().swap(scheduling);
}

void Solution() {
	cout << "\n*** Gantt Chart는 15인치 화면 가로 길이 기준 hyperperiod가 60인 경우까지 가로로 길게 보이도록 되어있습니다.\n" <<
		"더 작은 화면의 경우에는 가로 길이를 늘려도 chart가 잘려 보일 수 있습니다. *** \n";

	cout << "\n\n#1. Background\n";
	background();

	cout << "\n\n#2. Polling\n";
	polling();
}


int main(void) {
	init();
	input();
	Solution();
	return 0;
}