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
	cout << "\n\nDelay Write�� �����ϰ� ���� ��Ϲ�ȣ�� �Է��ϼ���.\n";
	cout << "�Է� ==> ";
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
	cout << "\n\n�Ҵ��� ��� ��ȣ�� �Է��ϼ���.\n";
	cout << "�Է� ==> ";
	int blkno;
	cin >> blkno;

	bool isInHash = false;
	bool isBufBusy = false;

	// ����� �ؽ�����࿭�� �ִ��� Ȯ��
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

	if (isInHash) { // ����� �ؽ�����࿭�� ����
		// ���۰� ��������� Ȯ��
		bool wakeup = false;
		if (isBufBusy) { //Case 5.
			if (sleep.empty()) {
				sleep.push_back(make_pair(blkno, 1));
				cout << "\n��Ϲ�ȣ " << blkno << " ���۰� ������̹Ƿ� 1�� �� Sleep �մϴ�.\n";
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
							cout << "\n��Ϲ�ȣ " << blkno << " ���۰� ������̹Ƿ� 1�� �� Sleep �մϴ�.\n";
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
					cout << "\n��Ϲ�ȣ " << blkno << " ���۸� unlock�մϴ�.\n";
					print();

					if (!waitlist.empty()) {
						int waitblkno = waitlist[0].data;
						cout << "\nNOTICE **\n";
						cout << "��� ���̾��� ��Ϲ�ȣ " << waitblkno << " ���۸� �Ҵ��մϴ�.\n";
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
	else { //����� �ؽ�����࿭�� ����.
		if (freelist.empty()) { //��������Ʈ�� ���۰� ���� // Case 4
			cout << "\nfreelist�� ���۰� �����Ƿ� 1�� �� Sleep �մϴ�.\n";
			cout << "freelist�� ���۰� �߰��Ǹ�, �� �� �ش� ����� �Ҵ��մϴ�.\n";

			//�ߺ� ���۴� waitlist�� �������� ����
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
					cout << "\nDelay Write�� ��Ϲ�ȣ " << freelist[i].data << " ���۸� ��ũ�� �񵿱�� ����մϴ�.\n";
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
					//��������Ʈ���� ���۸� �����.
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
		cout << "�Է� ==> ";
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
			cout << "���α׷��� �����մϴ�.\n";
			exit(0);
			break;
		default:
			break;
		}
	}
}



void input() {
	cout << "\n�Ʒ��� �Է� �� hash queue header size, ����� ������ ��� 3 �̻�, 9 ������ ���� �Է� �����մϴ�.\n";
	cout << "Hash queue�� header size�� �Է����ּ��� ==> ";
	cin >> hashHeaderSize;
	if (hashHeaderSize < 3 || hashHeaderSize > 10) {
		cout << "\n3 �̻� 9 ������ ���� �����մϴ�.\n";
		cout << "���α׷��� �����մϴ�.\n";
		exit(0);
	}
	
	for (int i = 0; i < hashHeaderSize; i++) {
		int n;
		cout << "\nhash queue�� "<< i << "��°�� �Ҵ�Ǿ��ִ� block�� ������ �Է��ϼ���.\n";
		cout << "�Է� ==> ";
		cin >> n;

		if (n < 3 || n > 10) {
			cout << "\n3 �̻� 9 ������ ���� �����մϴ�.\n";
			cout << "���α׷��� �����մϴ�.\n";
			exit(0);
		}

		cout << "\n" << i << "��°�� �Ҵ�Ǿ��ִ� ��ϵ��� ��� ��ȣ�� �Է��ϼ���.\n";
		cout << "����) 28 4 64\n";
		cout << "�Է� ==> ";
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
	cout << "\nfree list�� �ִ� block�� ���� m�� �Է��ϼ���.\n";
	cout << "�Է� ==> ";
	cin >> freeListSize;

	if (freeListSize < 3 || freeListSize > 10) {
		cout << "\n3 �̻� 9 ������ ���� �����մϴ�.\n";
		cout << "���α׷��� �����մϴ�.\n";
		exit(0);
	}

	cout << "\nfree list�� �ִ� ��� ��ȣ�� �Է��ϼ���.\n";
	cout << "����) 6 3 5 4 28 97 10\n";
	cout << "�Է� ==> ";
	for (int i = 0; i < freeListSize; i++) {
		int tmp;
		cin >> tmp;
		Buffer buf;
		buf.data = tmp;
		buf.isBusy = false;
		buf.isDelayWrite = false;
		freelist.push_back(buf);
	}
	cout << "\n�Է��� ��� �Ϸ�Ǿ����ϴ�.\n";
}

int main(void) {
	cout << "== Buffer Cache ==\n";
	input();
	program();
	return 0;
}