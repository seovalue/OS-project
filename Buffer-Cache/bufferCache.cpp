#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <windows.h>
using namespace std;
void program();

struct Buffer {
	int data;
	bool isDelayWrite;
	bool isBusy;
};

//variables
int hashHeaderSize;
vector<vector<Buffer>> hashQueue;
vector<Buffer> freelist;
vector<pair<int,int>> sleep;
vector<Buffer> waitlist;

void print() {
	cout << "\n\n";
	for (int i = 0; i < hashQueue.size(); i++) {
		cout << "blk no " << i << " mod " << hashHeaderSize << " [";
		for (int j = 0; j < hashQueue[i].size(); j++) {
			cout << " " << hashQueue[i][j].data;
			if (hashQueue[i][j].isDelayWrite) {
				cout << "(D)";
			}
		}cout << " ]\n";
	}
	cout << "free list [";
	for (int i = 0; i < freelist.size(); i++) {
		cout << " " << freelist[i].data;
		if (freelist[i].isDelayWrite) {
			cout << "(D)";
		}
	}cout << " ]\n";
}

int hashfunc(int index) {
	return index % hashHeaderSize;
}

void changeIsDelayWrite() {
	cout << "\n\nDelay Write로 변경하고 싶은 블록번호를 입력하세요.\n";
	cout << "입력 ==> ";
	int blkno;
	cin >> blkno;

	int idx = hashfunc(blkno);
	for (int i = 0; i < hashQueue[idx].size(); i++) {
		if (hashQueue[idx][i].data == blkno) {
			hashQueue[idx][i].isDelayWrite = true;
			break;
		}
	}

	for (int i = 0; i < freelist.size(); i++) {
		if (freelist[i].data == blkno) {
			freelist[i].isDelayWrite = true;
			break;
		}
	}
}

int getblk() {
	cout << "\n\n할당할 블록 번호를 입력하세요.\n";
	cout << "입력 ==> ";
	int blkno;
	cin >> blkno;

	bool isInHash = false;
	bool isBufBusy = false;

	// 블록이 해쉬대기행열에 있는지 확인
	for (int i = 0; i < hashQueue.size(); i++) {
		if (isInHash) break;
		for (int j = 0; j < hashQueue[i].size(); j++) {
			if (hashQueue[i][j].data == blkno) {
				isInHash = true;
				isBufBusy = hashQueue[i][j].isBusy;
				break;
			}
		}
	}

	if (isInHash) { // 블록이 해쉬대기행열에 존재
		// 버퍼가 사용중인지 확인
		bool wakeup = false;
		if (isBufBusy) { //Case 5.
			if (sleep.empty()) {
				sleep.push_back(make_pair(blkno, 1));
				cout << "\n블록번호 " << blkno << " 버퍼가 사용중이므로 1초 간 Sleep 합니다.\n";
				Sleep(1000);
			}
			else {
				for (int k = 0; k < sleep.size(); k++) {
					if (sleep[k].first == blkno) {
						if (sleep[k].second == 2) {
							sleep.erase(sleep.begin() + k);
							wakeup = true;
						}
						else {
							sleep[k].second++;
							cout << "\n블록번호 " << blkno << " 버퍼가 사용중이므로 1초 간 Sleep 합니다.\n";
							Sleep(1000);
						}
					}
				}

				if (wakeup) {
					Buffer buf;
					buf.data = blkno;
					buf.isBusy = false;
					buf.isDelayWrite = false;
					freelist.push_back(buf);

					bool flag = false;
					for (int i = 0; i < hashQueue.size(); i++) {
						if (flag) break;
						for (int j = 0; j < hashQueue[i].size(); j++) {
							if (hashQueue[i][j].data == blkno) {
								hashQueue[i][j].isBusy = false;
								flag = true;
								break;
							}
						}
					}
					cout << "\n블록번호 " << blkno << " 버퍼를 unlock합니다.\n";
					print();

					if (!waitlist.empty()) {
						int waitblkno = waitlist[0].data;
						cout << "\nNOTICE **\n";
						cout << "대기 중이었던 블록번호 " << waitblkno << " 버퍼를 할당합니다.\n";
						buf.data = waitblkno;
						buf.isBusy = true;
						freelist.erase(freelist.begin());
						int idx = hashfunc(waitblkno);
						hashQueue[idx].push_back(buf);

						return waitblkno;
					}
					return -1;
				}
			}
		} else { //case 1
			int idx = hashfunc(blkno);
			for (int i = 0; i < hashQueue[idx].size(); i++) {
				if (hashQueue[idx][i].data == blkno) {
					hashQueue[idx][i].isBusy = true;
					break;
				}
			}

			for (int i = 0; i < freelist.size(); i++) {
				if (freelist[i].data == blkno) {
					freelist.erase(freelist.begin() + i);
					break;
				}
			}

			return blkno;
		}
	}
	else { //블록이 해쉬대기행열에 없다.
		if (freelist.empty()) { //자유리스트상에 버퍼가 없다 // Case 4
			cout << "\nfreelist에 버퍼가 없으므로 1초 간 Sleep 합니다.\n";
			cout << "freelist에 버퍼가 추가되면, 그 때 해당 블록을 할당합니다.\n";

			//중복 버퍼는 waitlist에 저장하지 않음
			if (waitlist.empty())
			{
				Buffer buf;
				buf.data = blkno;
				buf.isBusy = false;
				buf.isDelayWrite = false;
				waitlist.push_back(buf);
			}
			else {
				bool isSame = false;
				for (int i = 0; i < waitlist.size(); i++) {
					if (waitlist[i].data == blkno) {
						isSame = true;
						break;
					}
				}

				if (!isSame) {
					Buffer buf;
					buf.data = blkno;
					buf.isBusy = false;
					buf.isDelayWrite = false;
					waitlist.push_back(buf);
				}
			}

			Sleep(1000);
		}
		else {
			for (int i = 0; i < freelist.size(); i++) {
				if (freelist[i].isDelayWrite) { //Case 3
					cout << "\nDelay Write인 블록번호 " << freelist[i].data << " 버퍼를 디스크에 비동기로 출력합니다.\n";
					for (int j = 0; j < hashQueue.size(); j++) {
						for (int k = 0; k < hashQueue[j].size(); k++) {
							if (hashQueue[j][k].data == freelist[i].data) {
								hashQueue[j][k].isDelayWrite = false;
							}
						}
					}

					freelist.erase(freelist.begin() + i);
					i = i - 1;
				}
				else { //Case 2
					//자유리스트에서 버퍼를 지운다.
					int rmblkno = freelist[i].data;
					freelist.erase(freelist.begin() + i);
					bool isRemoved = false;
					for (int i = 0; i < hashQueue.size(); i++) {
						if (isRemoved) break;
						for (int j = 0; j < hashQueue[i].size(); j++) {
							if (hashQueue[i][j].data == rmblkno) {
								hashQueue[i].erase(hashQueue[i].begin() + j);
								isRemoved = true;
								break;
							}
						}
					}

					int idx = hashfunc(blkno);
					Buffer buf;
					buf.data = blkno;
					buf.isBusy = true;
					buf.isDelayWrite = false;
					hashQueue[idx].push_back(buf);
					return blkno;
				}
			}
		}
	}

	return -1;
}



void program() {
	while (true) {
		print();

		cout << "\nCHOOSE COMMAND\n";
		cout << "1. change block status to delayed write\n";
		cout << "2. getblk\n";
		cout << "3. exit\n";

		int cmd;
		cout << "입력 ==> ";
		cin >> cmd;

		int buffno;

		switch (cmd)
		{
		case 1:
			changeIsDelayWrite();
			break;
		case 2:
			buffno = getblk();
			if (buffno != -1) {
				cout << "\nRETURN => " << buffno;
			}
			break;
		case 3:
			cout << "프로그램을 종료합니다.\n";
			exit(0);
			break;
		default:
			break;
		}
	}
}



void input() {
	cout << "\n아래의 입력 중 hash queue header size, 블록의 개수는 모두 3 이상, 9 이하의 수만 입력 가능합니다.\n";
	cout << "Hash queue의 header size를 입력해주세요 ==> ";
	cin >> hashHeaderSize;
	if (hashHeaderSize < 3 || hashHeaderSize > 10) {
		cout << "\n3 이상 9 이하의 수만 가능합니다.\n";
		cout << "프로그램을 종료합니다.\n";
		exit(0);
	}
	
	for (int i = 0; i < hashHeaderSize; i++) {
		int n;
		cout << "\nhash queue의 "<< i << "번째에 할당되어있는 block의 개수를 입력하세요.\n";
		cout << "입력 ==> ";
		cin >> n;

		if (n < 3 || n > 10) {
			cout << "\n3 이상 9 이하의 수만 가능합니다.\n";
			cout << "프로그램을 종료합니다.\n";
			exit(0);
		}

		cout << "\n" << i << "번째에 할당되어있는 블록들의 블록 번호를 입력하세요.\n";
		cout << "예시) 28 4 64\n";
		cout << "입력 ==> ";
		vector<Buffer> inputHash;
		for (int j = 0; j < n; j++) {
			int tmp;
			cin >> tmp;
			Buffer buf;
			buf.data = tmp;
			buf.isBusy = false;
			buf.isDelayWrite = false;
			inputHash.push_back(buf);
		}
		hashQueue.push_back(inputHash);
	}

	int freeListSize;
	cout << "\nfree list에 있는 block의 개수 m을 입력하세요.\n";
	cout << "입력 ==> ";
	cin >> freeListSize;

	if (freeListSize < 3 || freeListSize > 10) {
		cout << "\n3 이상 9 이하의 수만 가능합니다.\n";
		cout << "프로그램을 종료합니다.\n";
		exit(0);
	}

	cout << "\nfree list에 있는 블록 번호를 입력하세요.\n";
	cout << "예시) 6 3 5 4 28 97 10\n";
	cout << "입력 ==> ";
	for (int i = 0; i < freeListSize; i++) {
		int tmp;
		cin >> tmp;
		Buffer buf;
		buf.data = tmp;
		buf.isBusy = false;
		buf.isDelayWrite = false;
		freelist.push_back(buf);
	}
	cout << "\n입력이 모두 완료되었습니다.\n";
}

int main(void) {
	cout << "== Buffer Cache ==\n";
	input();
	program();
	return 0;
}