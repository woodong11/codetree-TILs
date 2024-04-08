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

struct tower {
	int y;
	int x;
	int power;
	int lattestAttack;
};

int N, M, K, curTurn, isEnd;
int dy[8] = { -1, 0, 1, 0, -1, -1, 1, 1 };		// 상좌하우 먼저 만들었다. (나중에 되돌아가면서 레이저 path 찾기 위해)
int dx[8] = { 0, -1, 0, 1, -1, 1, -1, 1 };
int MAP[11][11];
int visited[11][11];
int isRelated[11][11];			// 공격과 관련 있는지 저장
int lattestAttackList[11][11];  // 마지막 공격 시기
Node attacker, target;
vector <Node> path;				// 레이저용 패스 저장소

bool compare1(tower a, tower b) {				// 공격자용
	if (a.lattestAttack == b.lattestAttack) {
		if ((a.y + a.x) == (b.y + b.x))
			return a.x > b.x ;
		return (a.y + a.x) > (b.y + b.x);
	}
	return a.lattestAttack > b.lattestAttack;
};

bool compare2(tower a, tower b) {				// 대상자용
	if (a.lattestAttack == b.lattestAttack) {
		if ((a.y + a.x) == (b.y + b.x))
			return a.x < b.x;
		return (a.y + a.x) < (b.y + b.x);
	}
	return a.lattestAttack < b.lattestAttack;
};

void input() {
	cin >> N >> M >> K;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < M; j++){
			int tempPower;
			cin >> tempPower;
			MAP[i][j] = tempPower;		
		}
	}
}

// 공격자와 타겟 포탑 찾기
void findTarget() {
	
	// 현재 최대와 최소값 찾기
	int minVal = 21e8, maxVal = 0;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < M; j++){
			if (MAP[i][j] > 0) {
				minVal = min(minVal, MAP[i][j]);
				maxVal = max(maxVal, MAP[i][j]);
			}
		}
	}

	// 만약 부서지지 않은 포탑이 1개라면 즉시 종료
	if (minVal == maxVal){
		isEnd = 1;
		return;
	}
	
	// 다시 2중 for문 돌면서 해당하는 위치 찾고 sort 때려서 정하기
	vector <tower> attackerList;
	vector <tower> targetList;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < M; j++){
			if (MAP[i][j] == minVal)
				attackerList.push_back({i, j, MAP[i][j], lattestAttackList[i][j]});
			else if (MAP[i][j] == maxVal)
				targetList.push_back({ i, j, MAP[i][j], lattestAttackList[i][j] });
		}
	}
	sort(attackerList.begin(), attackerList.end(), compare1);
	sort(targetList.begin(), targetList.end(), compare2);
	attacker = { attackerList[0].y, attackerList[0].x };
	target = { targetList[0].y, targetList[0].x };
	lattestAttackList[attacker.y][attacker.x] = curTurn;	//  최근 공격 업데이트
	
	// 공격자 핸디캡 적용, 공격자와 대상자는 공격에 관련이 있다
	MAP[attacker.y][attacker.x] = MAP[attacker.y][attacker.x] + N + M;
	isRelated[attacker.y][attacker.x] = 1;
	isRelated[target.y][target.x] = 1;
}

// 레이저 체크
bool lazer() {
	memset(visited, 0, sizeof(visited));
	path.clear();
	int nny[8] = { N, 0, -N, 0, N, N, -N, -N };			// 범위 벗어났을때를 대비
	int nnx[8] = { 0, M, 0, -M, M, -M, M, -M };
	queue <Node> q;
	visited[attacker.y][attacker.x] = 1;
	q.push({ attacker.y, attacker.x });

	while (!q.empty()) {
		Node now = q.front();
		q.pop();

		for (int i = 0; i < 4; i++){
			int ny = now.y + dy[i];
			int nx = now.x + dx[i];

			// 방향 돌리기
			if (ny < 0)
				ny += N;
			if (nx < 0)
				nx += M;
			if (ny >= N)
				nx -= N;
			if (nx >= M)
				nx -= M;

			if (MAP[ny][nx] == 0)	// 부서진 포탑은 통과 못함
				continue;

			if ((visited[ny][nx] == 0) || (visited[now.y][now.x] + 1 < visited[ny][nx])) {	// 방문 안 했거나 갱신할 수 있을때
				q.push({ ny, nx });
				visited[ny][nx] = visited[now.y][now.x] + 1;
			}
		}
	}

	if (visited[target.y][target.x] == 0)	// 타겟까지 방문 못 할때는 레이저 못 쏴
		return false;

	// 타겟에서 돌아오면서 최단거리 정하기
	// visited가 -1인 곳을 헨젤이 과자 줍듯이 따라가면 무조건 출발지점이 나온다!!
	int curDist = visited[target.y][target.x];
	int curY = target.y, curX = target.x;
	while (curDist > 2){
		for (int i = 0; i < 4; i++){
			int ny = curY + dy[i];
			int nx = curX + dx[i];

			if (ny < 0 || nx < 0 || ny >= N || nx >= M) {
				ny = ny + nny[i];
				nx = nx + nnx[i];
			}

			if (visited[ny][nx] == curDist - 1) {
				curDist--;
				path.push_back({ny, nx});
				curY = ny, curX = nx;
				isRelated[ny][nx] = 1;
				break;
			}
		}
	}

	// 레이저 지지기
	MAP[target.y][target.x] -= MAP[attacker.y][attacker.x];
	if (MAP[target.y][target.x] < 0)
		MAP[target.y][target.x] = 0;
	int attackPower = MAP[attacker.y][attacker.x] / 2;
	for (auto Point : path){
		MAP[Point.y][Point.x] -= attackPower;
		if (MAP[Point.y][Point.x] < 0)
			MAP[Point.y][Point.x] = 0;
	}

	return true;
}

void bomb() {
	int attackPower = MAP[attacker.y][attacker.x] / 2;

	// 피해받되, 디버깅을 위해 -라면 0으로 만들어보자
	MAP[target.y][target.x] -= MAP[attacker.y][attacker.x];
	if (MAP[target.y][target.x] < 0)
		MAP[target.y][target.x] = 0;
	for (int i = 0; i < 8; i++){
		int ny = target.y + dy[i];
		int nx = target.x + dx[i];

		// 방향 돌리기
		if (ny < 0)
			ny += N;
		if (nx < 0)
			nx += M;
		if (ny >= N)
			nx -= N;
		if (nx >= M)
			nx -= M;

		if ((ny == attacker.y && nx == attacker.x) || (MAP[ny][nx] == 0))	// 공격자거나 부서진 포탑은 무시 
			continue;

		MAP[ny][nx] -= attackPower;
		isRelated[ny][nx] = 1;
		if (MAP[ny][nx] < 0)
			MAP[ny][nx] = 0;
	}
}

void restore() {
	for (int i = 0; i < N; i++){
		for (int j = 0; j < M; j++){
			if (isRelated[i][j] == 1 || MAP[i][j] == 0)
				continue;
			MAP[i][j] += 1;
		}
	}
}

void solve() {
	for (int i = 1; i <= K; i++){
		curTurn = i;
		memset(isRelated, 0, sizeof(isRelated));

		findTarget();
		if (isEnd) {
			break;
		}
		if (!lazer()) {
			bomb();
		}

		restore();
		int de = -1;

	}

	// 종료되면, 가장 강한 포탑 공격력 출력
	int answer = 0;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < M; j++){
			answer = max(answer, MAP[i][j]);
		}
	}
	cout << answer;
	
}

int main() {
	//freopen("sample_input.txt", "r", stdin);
	input();
	solve();

	return 0;
}