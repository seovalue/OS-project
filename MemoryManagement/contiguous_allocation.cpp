#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

// variables
int memorySize = -1;
int requestId = -1;
int requestSize = -1;
int requestNum = 0;
int freeRequestNum = 0;
int freeBlock = 0;
double avgSize = 0;
int numOfBlock = 1;
vector<vector<int>> mm;
void Input();
void Allocation();

void PrintFree(int addr, int size) {
	cout << "\n\n";
	cout << "----------------------------------------------------------\n";
	cout << freeRequestNum << ". Free request P" << requestId << " (" << size << "K)\n";
	cout << "[Best-fit]\n";
	cout << "Freed at address " << addr << ".\n";
	cout << freeBlock << "K free, " << numOfBlock << " block(s), Average Size: " << avgSize << "K\n";
	cout << "----------------------------------------------------------\n";
}

void PrintAlloc(int addr) {
	cout << "\n\n";
	cout << "----------------------------------------------------------\n";
	cout << requestNum << ". Request P" << requestId << " : " << requestSize << "K\n";
	cout << "[Best-fit]\n";
	cout << "Allocated at address " << addr << ".\n";
	cout << freeBlock << "K free, " << numOfBlock << " block(s), Average Size: " << avgSize << "K\n";
	cout << "----------------------------------------------------------\n";
}

void Coalescing(int from, int to) {
	cout << "\nStart Coalescing from address at " << mm[from][1] << " to address at " << mm[to][1] << "........\n";
	
	mm[from][0] = -1;
	mm[from][2] = mm[to][2];
	//int CoalescingBlock = mm[from][2] - mm[from][1];

	if (to - from > 1) {
		mm.erase(mm.begin() + from + 1, mm.begin() + to);
	}
	else {
		mm.erase(mm.begin() + from + 1);
	}
	numOfBlock -= to - from;
}

void Free() {
	int size = 0;
	for (int i = 0; i < mm.size(); i++) {
		if (mm[i][0] == requestId) {
			bool flag = false;
			int from = -1;
			int to = -1;
			size = mm[i][2] - mm[i][1];
			if (i != 0) {
				if (mm[i - 1][0] == -1) { //���� ����� empty
					from = i - 1;
				}
			}

			if (i != mm.size() - 1) {
				if (mm[i + 1][0] == -1) { //���� ����� empty
					to = i + 1;
				}
			}

			if (to != -1 || from != -1) { // Free�� ���� �� ��ó�� empty�� ����
				if (to == -1) {
					to = i;
				}
				else if (from == -1) {
					from = i;
				}
				flag = true;
			}

			// Free
			mm[i][0] = -1;
			numOfBlock++;

			if (flag) {
				Coalescing(from, to);
			}
			freeBlock += size;
		//	else 
		//		freeBlock = freeBlock + (mm[i][2] - mm[i][1]);

			avgSize = double(freeBlock) / numOfBlock;

			PrintFree(mm[i][1], size);
			break;
		}
	}
}

void Compaction() {
	/*
	1. ������ �ִ� ���μ����� ũ��� empty�� ũ�Ⱑ ������, �ش� ���μ����� �ֺ��� empty�̰�, �Ű��� �� �ջ� Freeblock >= req�� ��
	2. ������ �ִ� ���μ����� ���� ũ�⸦ ���� empty ���� => �� ������ �� ū ������ �о���δ�.
	*/

	cout << "\nStart Compaction......\n";

	bool isFit = false;
	int i;
	int size = mm.size();
	int maxEmptySize = 0;
	int maxEmptyIdx = 0;
	vector<vector<int>> empty;
	for (i = 0; i < size; i++) {
		if (isFit) break;
		if (mm[i][0] == -1) {
			empty.push_back({i,mm[i][1], mm[i][2]});
			if (maxEmptySize <= mm[i][2] - mm[i][1]) {
				maxEmptySize = mm[i][2] - mm[i][1];
				maxEmptyIdx = i;
			}
			
			int emptySize = mm[i][2] - mm[i][1];
			int ifMove = 0;
			for (int j = 0; j < size; j++) {
				if (isFit) break;
				if (j == i) continue;
				if (mm[j][2] - mm[j][1] == emptySize) {
					if (j == 0) {
						if (mm[j + 1][0] == -1) {
							ifMove = mm[j + 1][2] - mm[j + 1][1] + emptySize;
							if (ifMove >= requestSize) {
								int tmpStartAddr = mm[j][1];
								int tmpEndAddr = mm[i][2] - mm[i][1] + tmpStartAddr;
								mm[i] = { mm[j][0], tmpStartAddr, tmpEndAddr };
								// mm[i]�� isNear �����������.
								mm[j][0] = -1;
								Coalescing(0, j+1);
								isFit = true;
							}
						}
					}
					else if (j == size - 1) {
						if (mm[j - 1][0] == -1) {
							ifMove = mm[j - 1][2] - mm[j - 1][1] + emptySize;
							if (ifMove >= requestSize) {
								int tmpStartAddr = mm[j][1];
								int tmpEndAddr = mm[i][2] - mm[i][1] + tmpStartAddr;
								mm[i] = { mm[j][0], tmpStartAddr, tmpEndAddr };
								// mm[i]�� isNear �����������.
								mm[j][0] = -1;
								Coalescing(j-1, size-1);
								isFit = true;
							}
						}
					}
					else {
						int from = 0;
						int to = 0;
						if (mm[j + 1][0] == -1 && j+1 != i) {
							ifMove = mm[j + 1][2] - mm[j + 1][1] + emptySize;
							to = j + 1;
						}
						if (mm[j - 1][0] == -1 && j-1 != i) {
							ifMove = mm[j - 1][2] - mm[j - 1][1] + emptySize;
							from = j - 1;
						}

						if (ifMove >= requestSize) {
							int tmpStartAddr = mm[i][1];
							int tmpEndAddr = mm[i][2] - mm[i][1] + tmpStartAddr;
							mm[i] = { mm[j][0], tmpStartAddr, tmpEndAddr };
							mm[j][0] = -1;
							if (to == 0) {
								to = j;
							}
							if (from == 0) {
								from = j;
							}
							Coalescing(from, to);
							isFit = true;
						}
					}
				}
			}
		}
	}
	if (!isFit) 
	{ //��ġ �ű��
		int emptySize = empty.size();
		int startAddr = 0;
		int alloc = 0;
		for (int i = 0; i < emptySize; i++) {
			alloc += empty[i][2] - empty[i][1];
		}
		int endAddr = startAddr + alloc;

		if (maxEmptyIdx == 0) {
			mm[0] = { -1, startAddr, endAddr }; //0��° empty Update

		}
		else if (maxEmptyIdx == size - 1) {
			startAddr = memorySize - endAddr;
			endAddr = memorySize;
			mm[size - 1] = { -1, startAddr, endAddr };
		}
		else { //�߰��� empty�� ����
			int beforeSize = 0;
			int afterSize = 0;
			for (int i = 0; i < emptySize; i++) {
				if (i < maxEmptyIdx) {
					beforeSize += empty[i][2] - empty[i][1];
				}
				else if (i > maxEmptyIdx) {
					afterSize += empty[i][2] - empty[i][1];
				}
			}

			startAddr = mm[maxEmptyIdx][1] - beforeSize;
			endAddr = mm[maxEmptyIdx][2] + afterSize;

			mm[maxEmptyIdx] = { -1, startAddr, endAddr };
		}

		for (int i = 0; i < emptySize; i++) {
			if (i == maxEmptyIdx) continue;
			mm.erase(mm.begin() + empty[i][0]);
		}

		// �ּ� ���Ҵ�
		for (int i = 0; i < size - 2; i++) {
			if (mm[i][2] != mm[i + 1][1]) {
				int allocsize = mm[i + 1][2] - mm[i + 1][1];
				mm[i + 1][1] = mm[i][2];
				mm[i + 1][2] = mm[i + 1][1] + allocsize;
			}
		}
	}

	Allocation();
	return;
}

void Allocation() {
	int size = mm.size();
	if (numOfBlock == 1) {
		if (size == 1) { //���� �ƹ��͵� �Ҵ���� ���� ����
			mm[0] = { requestId, 0, requestSize};
			mm.push_back({ -1, requestSize, memorySize});
			avgSize = memorySize - requestSize;
			freeBlock = avgSize;
			PrintAlloc(0);
		}
		else { //numOfBlock�� �Ѱ�, ���μ����� �Ѱ� �̻� �Ҵ�� ����
			for (int i = 0; i < size; i++) {
				if (mm[i][0] == -1) {
					if (i + 1 >= size) { //empty�� ���� �������� ����
						mm[i] = { requestId, mm[i - 1][2], mm[i - 1][2] + requestSize };
						avgSize = memorySize - (mm[i - 1][2] + requestSize);

						if (mm[i - 1][2] + requestSize <= memorySize)
							mm.push_back({ -1, mm[i - 1][2] + requestSize, memorySize });
						
						freeBlock = avgSize;
						PrintAlloc(mm[i - 1][2]);
						break;
					}

					if (mm[i][2] - mm[i][1] > requestSize) {
						mm[i] = { requestId, mm[i - 1][2], mm[i - 1][2] + requestSize };
						mm.resize(mm.size() + 1);
						for (int j = size - 2; j >= i + 1; j--) {
							mm[j + 1] = mm[j];
						}
						mm[i + 1] = { -1, mm[i][2], mm[i + 2][1] };
						avgSize = mm[i + 2][1] - mm[i][2];
						freeBlock = avgSize;
						PrintAlloc(mm[i - 1][2]);
						break;
					}
					else if (mm[i][2] - mm[i][1] == requestSize) {
						mm[i] = { requestId, mm[i - 1][2], mm[i - 1][2] + requestSize };
						if (freeBlock == requestSize) {
							avgSize = 0;
							freeBlock = avgSize;
							numOfBlock = 0;
						}
						else {
							mm.push_back({ -1, mm[i - 1][2] + requestSize, memorySize });
							avgSize = memorySize - (mm[i - 1][2] + requestSize);
							freeBlock = avgSize;
						}
						PrintAlloc(mm[i - 1][2]);
						break;
					}
					
				}
			}
		}
	}
	else { //������ �Ҵ��, �߰��� ������ empty ����

		bool canAlloc = false;
		int idx = 0;
		int canAllocSize = memorySize;
		int canSumSize = 0;
		for (int i = 0; i < size; i++) {
			if (mm[i][0] == -1) {
				if (mm[i][2] - mm[i][1] >= requestSize) {
					canAlloc = true;
					if (canAllocSize >= mm[i][2] - mm[i][1]) {
						canAllocSize = mm[i][2] - mm[i][1];
						idx = i;
					}
				}
				
				canSumSize += mm[i][2] - mm[i][1];
			}
		}

		int startAddr = 0;
		int endAddr = 0;
		int isNear = 0;

		//empty �߿� �Ҵ� �����Ѱ� ������ ��
		if (canAlloc) {
			if (requestSize < canAllocSize) isNear = 1;
			if (idx != 0) {
				startAddr = mm[idx - 1][2];
			}
			endAddr = startAddr + requestSize;
			mm[idx] = { requestId, startAddr, endAddr};

			if (isNear) {
				mm.resize(mm.size() + 1);
				for (int i = size - 2; i >= idx + 1; i--) {
					mm[i + 1] = mm[i];
				}
				mm[idx + 1] = { -1, endAddr, mm[idx + 2][1]};
			}

			numOfBlock--;
		}

		//empty�� ������ �� �Ҵ� �����Ѱ� ������ �� => Compaction
		if (!canAlloc && canSumSize >= requestSize) {
			Compaction();
		}
		else {
			freeBlock = freeBlock - requestSize;
			avgSize = (double)freeBlock / numOfBlock;
			PrintAlloc(startAddr);
		}
	}
}

void Input() {
	while (true) {
		cout << "\nEnter the request id and request size";
		cout << "\nEXAMPLE) if you request that P0, 64K then you enter like 0 64";
		cout << "\n** If you want to exit, enter -1 -1 **";
		cout << "\nInput ==> ";
		cin >> requestId >> requestSize;

		if (requestId == -1 && requestSize == -1) {
			cout << "Exit the program.\n";
			exit(0);
		}

		if (requestSize == 0) {
			freeRequestNum++;
			Free();
		}
		else if (requestSize > memorySize || requestSize > freeBlock) {
			cout << "Error! Request Size is more than memory size.\n";
			Input();
		}
		else {
			requestNum++;
			Allocation();
		}
	}
}

int main(void) {
	while (true) {
		cout << "==== Contiguous allocation ====\n";
		cout << "Enter the memory size ==> ";
		cin >> memorySize;
		mm.push_back({ -1,0,memorySize}); //processID, addrStart,addrEnd,isNear
		avgSize = memorySize;
		freeBlock = memorySize;

		cout << "\n";
		if (memorySize > 1000 || memorySize <= 0) {
			cout << "\nMemory size must be more than 0, less than 1000K\n";
		}
		else {
			Input();
		}
	}
	return 0;
}