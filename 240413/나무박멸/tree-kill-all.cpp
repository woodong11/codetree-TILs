#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
using namespace std;

struct Node{
	int y;
	int x;
};

struct preTree {	// 나무 번식할때 필요한 구조체
	int y;
	int x;
	int growNum;
};

bool compare(Node a, Node b) {
	if (a.y == b.y) {
		return a.x < b.x;
	}
	return a.y < b.y;
}

int N, M, K, C, answer;
int treeMAP[21][21];
int medMAP[21][21];
int dy[4] = { -1, 1, 0, 0 };  int dx[4] = { 0, 0, -1, 1 };		// 나무용
int ddy[4] = { -1, -1, 1, 1 }; int ddx[4] = { -1, 1, -1, 1 };	// 제초제용
queue <Node> treeList;


void input() {
	cin >> N >> M >> K >> C;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			cin >> treeMAP[i][j];
			if (treeMAP[i][j] > 0)
				treeList.push({ i, j });
		}
	}
}

// 나무 성장 함수
void growTree(int curYear){

	int curTreeNum = treeList.size();
	for (int  i = 0; i < curTreeNum; i++){
		Node curTree = treeList.front();
		treeList.pop();

		int nearNum = 0;
		for (int j = 0; j < 4; j++){
			int ny = curTree.y + dy[j];
			int nx = curTree.x + dx[j];

			if (ny < 0 || nx < 0 || ny >= N || nx >= N)
				continue;
			if (treeMAP[ny][nx] > 0)
				nearNum++;
		}
		treeMAP[curTree.y][curTree.x] += nearNum;
		treeList.push(curTree);
	}
}

// 나무 번식 함수
void makeTree(int curYear) {
	
	// 번식해서 태어날 예비 나무 리스트에 담는다.
	vector <preTree> preTrees;							// 번식해서 태어나는 예비 나무들
	int curTreeNum = treeList.size();
	for (int i = 0; i < curTreeNum; i++) {
		Node curTree = treeList.front();
		treeList.pop();

		vector <Node> posPosition;	// 한 나무당 가능한 위치 담는 함수
		for (int j = 0; j < 4; j++) {
			int ny = curTree.y + dy[j];
			int nx = curTree.x + dx[j];

			if (ny < 0 || nx < 0 || ny >= N || nx >= N)
				continue;
			if (treeMAP[ny][nx] == -1 || treeMAP[ny][nx] > 0)
				continue;
			if (medMAP[ny][nx] >= curYear)	// 제초제가 남아있는 위치였다면
				continue;
			posPosition.push_back({ ny, nx });
		}

		if (posPosition.size() == 0) {		
			continue;
		}	
		else {			// 예비 나무 리스트에 담는다.
			int makeNum = treeMAP[curTree.y][curTree.x] / posPosition.size();
			for (Node newPos : posPosition) {
				preTrees.push_back({newPos.y, newPos.x, makeNum});
			}
		}
	}

	// 예비 나무들 뿌리기
	for (preTree tree : preTrees){
		treeMAP[tree.y][tree.x] += tree.growNum;
	}
}

// 해당 위치에서 제초제 뿌릴때 최대 나무 kill 개수 구하기
int findKillNum(int startY, int startX) {

	int sum = treeMAP[startY][startX];
	for (int i = 0; i < 4; i++){
		for (int j = 1; j <= K; j++){
			int ny = startY + ddy[i] * j;
			int nx = startX + ddx[i] * j;

			if (ny < 0 || nx < 0 || ny >= N || nx >= N)
				break;
			if (treeMAP[ny][nx] <= 0)
				break;
			sum += treeMAP[ny][nx];
		}
	}
	return sum;
}

// 뿌릴 제초제 위치 구하고 뿌리고 살아있는 나무는 넣는 함수
void killTree(int curYear) {
	
	// max 값을 찾는다
	int maxMAP[21][21] = { 0, };
	int maxKill = 0;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (treeMAP[i][j] > 0)
				maxMAP[i][j] = findKillNum(i, j);
				maxKill = max(maxKill, maxMAP[i][j]);
		}
	}

	// max인 위치 넣는다.
	vector <Node> medList;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (maxMAP[i][j] == maxKill)
				medList.push_back({ i, j });
		}
	}

	// 위치 찾고 sort한다
	if (medList.size() == 0)
		return;
	sort(medList.begin(), medList.end(), compare);
	Node targetPos = medList[0];

	// 정답 업데이트하고 제초제 뿌리기 
	answer += maxKill;
	treeMAP[targetPos.y][targetPos.x] = 0;
	medMAP[targetPos.y][targetPos.x] = curYear + C;
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j <= K; j++) {
			int ny = targetPos.y + ddy[i] * j;
			int nx = targetPos.x + ddx[i] * j;

			if (ny < 0 || nx < 0 || ny >= N || nx >= N)
				break;
			if (treeMAP[ny][nx] <= 0) {
				medMAP[ny][nx] = curYear + C;
				break;
			}
			medMAP[ny][nx] = curYear +  C;
			treeMAP[ny][nx] = 0;	
		}
	}

	// 다음 년도를 위해 살아있는 나무들 리스트에 담기
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (treeMAP[i][j] > 0)
				treeList.push({i, j});
		}
	}
}

void solve() {
	for (int year = 1; year <= M; year++){

		growTree(year);
		makeTree(year);
		killTree(year);
		int de = -1;
	}
}

int main() {
	//freopen("sample_input.txt", "r", stdin);
	input();
	solve();

	cout << answer;

	return 0;
}