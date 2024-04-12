#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <cstring>
#include <algorithm>
using namespace std;

int N, M, K;
long long answer;
int MAP[16][16];
int teamMAP[16][16];    // 팀 번호 나타낸 맵 
int visited[16][16];
int dy[4] = { 0, -1, 0, 1 };
int dx[4] = { 1, 0, -1, 0 };

struct Node {
    int y;
    int x;
};

Node teamList[6][2];    // 각 팀당 0번 인덱스는 머리, 1번 인덱스는 꼬리



void input() {
    int teamNum = 1;
    cin >> N >> M >> K;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> MAP[i][j];
            if (MAP[i][j] == 1) {
                teamList[teamNum][0] = { i, j };
                teamNum++;
            }
        }
    }
}

// bfs로 팀 번호 나타낸 맵 그리기, 사람 꼬리 저장하기
void drawTeamMAP() {
    for (int i = 1; i <= M; i++) {

        Node startPerson = teamList[i][0];
        queue <Node> q;
        q.push({ startPerson.y, startPerson.x });
        teamMAP[startPerson.y][startPerson.x] = i;

        while (!q.empty()) {
            Node now = q.front();
            q.pop();
            for (int j = 0; j < 4; j++) {
                int ny = now.y + dy[j];
                int nx = now.x + dx[j];
                if (ny < 0 || nx < 0 || ny >= N || nx >= N)        // 범위 체크
                    continue;
                if (teamMAP[ny][nx] > 0)
                    continue;
                if (MAP[ny][nx] == 0)
                    continue;

                if (MAP[ny][nx] == 3) {                    // 꼬리사람 체크
                    teamList[i][1] = { ny, nx };
                }
                q.push({ ny, nx });
                teamMAP[ny][nx] = i;
            }
        }
    }
}

// 머리사람 따라 한칸 이동하는 함수 -> 머리랑 꼬리 위치만 바꾸면 된다!
void movePeople(int num) {

    // 꼬리사람 이동 
    Node tailPerson = teamList[num][1];
    Node newTail = { 0, 0 };
    for (int i = 0; i < 4; i++) {
        int ny = tailPerson.y + dy[i];
        int nx = tailPerson.x + dx[i];
        if (ny < 0 || nx < 0 || ny >= N || nx >= N)        // 범위 체크
            continue;
        if (MAP[ny][nx] == 2) {
            newTail = { ny, nx };
            break;
        }
    }
    MAP[newTail.y][newTail.x] = 3;
    teamList[num][1] = newTail;
    MAP[tailPerson.y][tailPerson.x] = 4;

    // 머리사람 이동
    Node headPerson = teamList[num][0];
    Node newHead = { 0, 0 };
    for (int i = 0; i < 4; i++) {
        int ny = headPerson.y + dy[i];
        int nx = headPerson.x + dx[i];
        if (ny < 0 || nx < 0 || ny >= N || nx >= N)        // 범위 체크
            continue;
        if (MAP[ny][nx] == 4) {
            newHead = { ny, nx };
            break;
        }
    }
    MAP[newHead.y][newHead.x] = 1;
    teamList[num][0] = newHead;
    MAP[headPerson.y][headPerson.x] = 2;

}

// 공 시작 위치 정하고 던지는 함수
Node throwBall(int round) {

    // 공의 시작 위치, 방향 정하기 
    int startY, startX, ballDir;
    round = round % (4 * N);
    if (round >= 1 && round <= N)
        startY = round - 1, startX = -1, ballDir = 0;
    else if (round <= 2 * N)
        startY = N, startX = round - N - 1, ballDir = 1;
    else if (round <= 3 * N)
        startY = (N - 1) - (round - 2 * N - 1), startX = N, ballDir = 2;
    else {
        if (round == 0)        // 0도 여기 포함되므로 편하게 변형했다.
            round = 4 * N;
        startY = -1, startX = (N - 1) - (round - 3 * N - 1), ballDir = 3;
    }

    // 공 던지기
    int curY, curX;
    curY = startY, curX = startX;
    while (1){
        int ny = curY + dy[ballDir];
        int nx = curX + dx[ballDir];
        if (ny < 0 || nx < 0 || ny >= N || nx >= N)        // 범위 체크
            break;

        if (MAP[ny][nx] >= 1 && MAP[ny][nx] <= 3)
            return { ny, nx };
        
        curY = ny, curX = nx;
    }
    return { -1, -1 };  // 사람 못 쳤을때
}

// 공을 맞은 사람 몇번짼지 파악하는 함수
int findOrder(int hitY, int hitX) {
    int hitTeam = teamMAP[hitY][hitX];
    Node headPerson = teamList[hitTeam][0];
    memset(visited, 0, sizeof(visited));

    if (headPerson.y == hitY && headPerson.x == hitX)
        return 1;

    Node cur, next;
    int orderNum = 2;
    cur.y = headPerson.y, cur.x = headPerson.x;
    visited[cur.y][cur.x] = 1;

    while (1) {

        // 다음 사람 찾기
        for (int i = 0; i < 4; i++) {
            next.y = cur.y + dy[i];
            next.x = cur.x + dx[i];
            if (next.y < 0 || next.x < 0 || next.y >= N || next.x >= N)        // 범위 체크
                continue;
            if (visited[next.y][next.x] == 1)
                continue;
            if (MAP[next.y][next.x] > 1 && MAP[next.y][next.x] < 4) {      // 다음칸에 사람이 있다면
                break;
            }
        }

        if (next.y == hitY && next.x == hitX) {
            return orderNum;
        }
        cur = {next.y, next.x};
        visited[next.y][next.x] = 1;
        orderNum++;
    }
}

// 점수 획득, 방향 바꾸기
void hitBall(int hitY, int hitX) {
    
    // 점수 획득
    int orderNum = findOrder(hitY, hitX);
    answer += (orderNum * orderNum);

    //방향 바꾸기
    int heatTeam = teamMAP[hitY][hitX];
    Node headPerson = teamList[heatTeam][0];
    Node tailPerson = teamList[heatTeam][1];
    teamList[heatTeam][1] = { headPerson.y, headPerson.x };
    teamList[heatTeam][0] = { tailPerson.y, tailPerson.x };
    MAP[teamList[heatTeam][0].y][teamList[heatTeam][0].x] = 1;
    MAP[teamList[heatTeam][1].y][teamList[heatTeam][1].x] = 3;
}

void solve() {

    drawTeamMAP();
    for (int i = 1; i <= K; i++) {

        // 각 팀들 머리사람 따라 한칸 이동
        for (int j = 1; j <= M; j++){
            movePeople(j);
        }
       
        Node ballDest = throwBall(i);
        if (ballDest.y == -1)
            continue;
        else {
            hitBall(ballDest.y, ballDest.x);
        }

        int de = -1;
    }

    cout << answer;
}

int main() {
    //freopen("sample_input.txt", "r", stdin);

    input();
    solve();

    return 0;
}