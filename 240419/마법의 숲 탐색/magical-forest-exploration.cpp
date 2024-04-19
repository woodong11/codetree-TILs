#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>
using namespace std;

struct Node{
	int y;
	int x;
};

struct golem {
	int y;
	int x;
	int exitY;
	int exitX;
	int exitDir;
};

int R, C, K, isSpiritMove, answer;
int dy[4] = { -1, 0, 1, 0 };
int dx[4] = { 0, 1, 0, -1 };
int MAP[71][71];
int visited[71][71];
int cmdList[1001][2];
golem golemList[1001];

void input() {
	cin >> R >> C >> K;
	int tempC, tempDir;
	for (int i = 1; i <= K; i++){
		cin >> tempC >> tempDir;
		cmdList[i][0] = tempC-1;
		cmdList[i][1] = tempDir;
	}
}

// 골렘을 반시계 또는 시계로 움직이는 함수
void golemRotate(int num, int rotateDir) {

	int curDir = golemList[num].exitDir;
	if (rotateDir == 0) {
		curDir = (curDir + 3) % 4;			// 반시계 회전
	}
	else {
		curDir = (curDir + 1) % 4;			// 시계 회전
	}
	golemList[num].exitY = golemList[num].y + dy[curDir];
	golemList[num].exitX = golemList[num].x + dx[curDir];
	golemList[num].exitDir = curDir;
}

// 골렘을 한칸 밑으로 움직이는 함수
bool moveDown(int num) {

	golem curGolem = golemList[num];
	vector <Node> target;
	target.push_back({2, 0});
	target.push_back({ 1, -1 });
	target.push_back({ 1, 1 });

	for (int i = 0; i < target.size(); i++){
		int ny = curGolem.y + target[i].y;
		int nx = curGolem.x + target[i].x;

		if (ny >= R)	// 바닥을 뚫을 수 없다
			return false;

		if (ny >= 0 && nx >= 0 && ny < R && nx < C) {	// 만약 해당 칸이 다른 골렘이 있었다면 못간다
			if (MAP[ny][nx] > 0)
				return false;
		}
	}

	// 여기까지 왔다면 아래로 움직일 수 있다는 뜻.
	golemList[num] = { curGolem.y+1, curGolem.x, curGolem.exitY+1, curGolem.exitX, curGolem.exitDir };
	return true;
}

// 골렘을 한칸 좌하로 움직이는 함수
bool moveLeft(int num) {

	golem curGolem = golemList[num];
	vector <Node> target;
	target.push_back({ -1, -1 });
	target.push_back({ 0, -2 });
	target.push_back({ 1, -1 });
	target.push_back({ 1, -2 });
	target.push_back({ 2, -1 });

	for (int i = 0; i < target.size(); i++) {
		int ny = curGolem.y + target[i].y;
		int nx = curGolem.x + target[i].x;

		if (ny >= R)	// 바닥을 뚫을 수 없다
			return false;
		if (nx < 0 || nx >= C)	// 벽을 뚫을 수 없다
			return false;

		if (ny >= 0 && nx >= 0 && ny < R && nx < C) {	// 만약 해당 칸이 다른 골렘이 있었다면 못간다
			if (MAP[ny][nx] > 0)
				return false;
		}
	}

	// 여기까지 왔다면 아래로 움직일 수 있다는 뜻.
	golemList[num] = { curGolem.y+1, curGolem.x-1, curGolem.exitY+1, curGolem.exitX-1, curGolem.exitDir };
	golemRotate(num, 0);	// 반시계 회전
	return true;
}

// 골렘을 한칸 우하로 움직이는 함수
bool moveRight(int num) {

	golem curGolem = golemList[num];
	vector <Node> target;
	target.push_back({ -1, 1 });
	target.push_back({ 0, 2 });
	target.push_back({ 1, 1 });
	target.push_back({ 1, 2 });
	target.push_back({ 2, 1 });

	for (int i = 0; i < target.size(); i++) {
		int ny = curGolem.y + target[i].y;
		int nx = curGolem.x + target[i].x;

		if (ny >= R)	// 바닥을 뚫을 수 없다
			return false;
		if (nx < 0 || nx >= C)	// 벽을 뚫을 수 없다
			return false;

		if (ny >= 0 && nx >= 0 && ny < R && nx < C) {	// 만약 해당 칸이 다른 골렘이 있었다면 못간다
			if (MAP[ny][nx] > 0)
				return false;
		}
	}

	// 여기까지 왔다면 아래로 움직일 수 있다는 뜻.
	golemList[num] = { curGolem.y+1, curGolem.x+1, curGolem.exitY+1, curGolem.exitX+1, curGolem.exitDir };
	golemRotate(num, 1);	// 시계 회전
	return true;
}

// 골렘을 움직일 수 있을때까지 움직이고 맵에 표시하는 함수
void moveGolem(int num) {

	// 골렘 진입 준비 -> -2행에서 시작한다
	isSpiritMove = 1;
	golemList[num] = { -2, cmdList[num][0], -2, cmdList[num][0], cmdList[num][1] };
	golemList[num].exitY = golemList[num].y + dy[golemList[num].exitDir];
	golemList[num].exitX = golemList[num].x + dx[golemList[num].exitDir];

	// 골렘 진입하기 
	while (1){
		if (moveDown(num)) {
			continue;
		}
		else {
			if (moveLeft(num))
				continue;
			else {
				if (moveRight(num))
					continue;
				else
					break;
			}
		}
	}

	// 골렘이 맵 벗어났는지 체크하기
	golem curGolem = golemList[num];
	if (curGolem.y <= 0) {
		memset(MAP, 0, sizeof(MAP));
		isSpiritMove = 0;
		return;
	}

	// 맵에다 골렘 표시하기
	MAP[curGolem.y][curGolem.x] = num;
	for (int i = 0; i < 4; i++){
		int ny = curGolem.y + dy[i];
		int nx = curGolem.x + dx[i];
		MAP[ny][nx] = num;
	}
}

// 정령을 이동하고 최대 이동할 수 있는 행 더하는 함수
void moveSpirit(int num) {

	if (isSpiritMove == 0)
		return;				// 해당 골렘이 초과해서 골렘의 정령이 못 움직이는 경우


	// bfs 시작
	golem curGolem = golemList[num];
	memset(visited, 0, sizeof(visited));
	queue <Node> q;
	q.push({ curGolem.y, curGolem.x });
	visited[curGolem.y][curGolem.x] = 1;
	int maxRow = 0;

	while (!q.empty()){
		Node now = q.front();
		q.pop();

		for (int i = 0; i < 4; i++){
			int ny = now.y + dy[i];
			int nx = now.x + dx[i];

			if (ny < 0 || nx < 0 || ny >= R || nx >= C)	// 맵 벗어나면 이동 못해
				continue;
			if (visited[ny][nx] == 1)	// 이미 벗어난 곳이면 이동 못해
				continue;
			if (MAP[ny][nx] == 0)		// 맵이 빈칸이면 이동 못해
				continue;

			// 골렘이 움직일 수 있는 경우
			int curGolemNum = MAP[now.y][now.x];
			int newGolemNum = MAP[ny][nx];
			
			if (curGolemNum == newGolemNum) {
				q.push({ ny, nx });
				visited[ny][nx] = 1;
				maxRow = max(maxRow, ny);
			}
				
			else {
				if (now.y == golemList[curGolemNum].exitY && now.x == golemList[curGolemNum].exitX) {
					q.push({ ny, nx });
					visited[ny][nx] = 1;
					maxRow = max(maxRow, ny);
				}
			}
		}
	}

	answer += (maxRow + 1);		// 정답 업데이트
}


void solve() {

	for (int num = 1; num <= K; num++)
	{
		moveGolem(num);
		moveSpirit(num);
	}

	cout << answer;
}

int main() {
	//freopen("sample_input.txt", "r", stdin);
	input();
	solve();
	return 0;
}