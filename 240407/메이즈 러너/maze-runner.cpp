// 메이즈 러너
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

struct person {
	int y;
	int x;
};

struct squrePoint {
	int y;
	int x;
	int squreLen;
};

bool compare(squrePoint a, squrePoint b) {
	if (a.y == b.y)
		return a.x < b.x;
	return a.y < b.y;
}

int MAP[11][11];
int dy[4] = { -1, 1, 0, 0 };
int dx[4] = { 0, 0, -1, 1 };
int N, M, K, exitY, exitX, cnt;
int isExit[11];
person personList[11];

void input(){
	cin >> N >> M >> K;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			cin >> MAP[i][j];
		}
	}
	int tempy, tempx;
	for (int i = 1; i <= M; i++){
		cin >> tempy >> tempx;
		personList[i] = { tempy - 1, tempx - 1 };	// 들어온 순서부터 1번 ~ M번째 사람
	}
	cin >> tempy >> tempx;
	exitY = tempy - 1;
	exitX = tempx - 1;
}

// 모두 탈출했는지 체크
bool allExit() {
	int flag = 1;
	for (int j = 1; j <= M; j++) {
		if (isExit[j] == 0) {
			flag = 0;
			return false;
		}
	}
	return true;
}

// 사람들 이동
void movePerson() {

	for (int i = 1; i <= M; i++){
		if (isExit[i] == 1)
			continue;

		int curDist = abs(exitY - personList[i].y) + abs(exitX - personList[i].x);
		for (int j = 0; j < 4; j++) {
			int ny = personList[i].y + dy[j];
			int nx = personList[i].x + dx[j];

			if (ny < 0 || nx < 0 || ny >= N || nx >= N)
				continue;
			if (MAP[ny][nx] > 0)
				continue;
			if (ny == exitY && nx == exitX) {	// 만약 그 칸이 출구라면
				cnt++;
				isExit[i] = 1;
				break;
			}

			int newDist = abs(exitY - ny) + abs(exitX - nx);
			if (newDist < curDist) {
				cnt++;
				personList[i].y = ny;
				personList[i].x = nx;
				break;
			}
		}
	}
}

// 각 사람 당 정사각형 시작 위치 찾는 함수
squrePoint findSqurePoint(int exitY, int exitX, int personY, int personX) {
	int gapY = abs(exitY - personY);
	int gapX = abs(exitX - personX);
	int squreLen, startY, startX;

	if (gapY == gapX) {
		return { min(exitY, personY), min(exitX, personX) };
	}
	else if (gapY > gapX) {		// y 차가 더 클때 -> startY, endY는 정해진다.
		startY = min(exitY, personY);
		squreLen = gapY + 1; 
		for (int i = 0; i <= min(exitX, personX); i++) {
			startX = i;
			if ((startX + squreLen) > max(exitX, personX))	// maxX가 해당 범위에 포함될 수 있다면 바로 return
				return { startY, startX, squreLen };
		}
	
	}
	else {								// x차가 더 클때 -> startX, endX는 정해진다.
		startX = min(exitX, personX);
		squreLen = gapX + 1;
		for (int i = 0; i <= min(exitY, personY); i++){
			startY = i; 
			if ((startY + squreLen) > max(exitY, personY))	// maxY가 해당 범위에 포함될 수 있다면 바로 return
				return {startY, startX, squreLen };
		}
	}
}

// 정사각형 찾고 돌리는 함수
void makeSqure(){

	// 사람이랑 출구 최소거리 찾기
	int minDist = 21e8;
	for (int i = 1; i <= M; i++){
		if (isExit[i] == 1)
			continue;
		int curDist = abs(personList[i].y - exitY) + abs(personList[i].x - exitX);
		minDist = min(curDist, minDist);
	}

	// 최소거리인 사람 curPeople벡터에 담기
	vector <person> curPeople;
	for (int i = 1; i <= M; i++) {
		if (isExit[i] == 1)
			continue;
		int curDist = abs(personList[i].y - exitY) + abs(personList[i].x - exitX);
		if (curDist == minDist)
			curPeople.push_back({ personList[i].y, personList[i].x });
	}
	
	// curPeople에 있는 사람들과 출구로 정사각형 경우 찾고, 정렬한다. 
	vector <squrePoint> squrePointList;
	for (int i = 0; i < curPeople.size(); i++){
		squrePoint ret = findSqurePoint(exitY, exitX, curPeople[i].y, curPeople[i].x);
		squrePointList.push_back(ret);
	}
	sort(squrePointList.begin(), squrePointList.end(), compare);
	
	// 정사각형 돌리기 준비
	int squreLen = squrePointList[0].squreLen;
	int startY = squrePointList[0].y;	
	int startX = squrePointList[0].x;
	int endY = squrePointList[0].y + squreLen - 1;
	int endX = squrePointList[0].x + squreLen - 1;
	int tempMAP[11][11] = { 0, };

	// 시계방향 90도 회전
	for (int i = 0; i <squreLen; i++){
		for (int j = 0; j < squreLen; j++){
			tempMAP[j + startY][squreLen-i-1 + startX] = MAP[i + startY][j + startX];
		}
	}

	// 회전한거 기존 맵에 다시 복사
	for (int i = startY; i < startY + squreLen; i++) {
		for (int j = startX; j < startX + squreLen; j++) {
			if (tempMAP[i][j] > 0)
				tempMAP[i][j]--;		// 내구도 감소
			MAP[i][j] = tempMAP[i][j];
		}
	}

	// 사람 돌리기
	for (int i = 1; i <= M; i++){
		if (isExit[i] == 1)
			continue;

		if (startY <= personList[i].y && personList[i].y <= endY && startX <= personList[i].x && personList[i].x <= endX) {
			int tempY = personList[i].y - startY;		// tempY: 시작 위치로부터 사람이 y방향 떨어진 거리
			int tempX = personList[i].x - startX;		// tempX: 시작 위치로부터 사람이 y방향 떨어진 거리

			personList[i].y = tempX + startY;
			personList[i].x = squreLen - tempY - 1 + startX;
		}
	}

	// 출구 돌리기
	int tempY = exitY - startY;
	int tempX = exitX - startX;
	exitY = tempX + startY;
	exitX = squreLen - tempY  -1 + startX;
}

// 사람 이동, 정사각형 찾기, 돌리기
void solve() {
	
	for (int i = 1; i <= K; i++){

		if (allExit())
			break;
		movePerson();
		makeSqure();
	}

	cout << cnt << "\n" << exitY + 1 << " " << exitX + 1;
}

int main() {
	//freopen("sample_input.txt", "r", stdin);
	input();
	solve();

	return 0;
}