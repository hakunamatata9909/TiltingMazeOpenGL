#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include "glut.h"
#include <gl/GLU.h>
#include <gl/GL.h>

#define WIDTH 600
#define HEIGHT 600
#define PI 3.141592
#define MAX 25 // 홀수값만 지정 가능, 짝수 값 지정 시 안 이쁨
#define ACCELERATION_MIN 0.02f
#define ACCELERATION_MAX 0.8f
#define CAMERA_MIN 24.0f
#define CAMERA_MAX 36.0f
#define FRICTION_GROUND 0.99f
#define FRICTION_WALL 0.5f

GLfloat BGColor[] = { 0.870f, 0.721f, 0.529f, 1.0f };
GLfloat PathColor[] = { 0.678f, 0.847f, 0.902f, 1.0f };
GLfloat StartColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat FinishColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat WallAmbient[] = { 0.596f, 0.984f, 0.596f, 1.0f };
GLfloat WallDiffuse[] = { 0.596f, 0.984f, 0.596f, 1.0f };
GLfloat WallSpecular[] = { 0.296f, 0.484f, 0.296f, 1.0f };
GLfloat BallAmbient[] = { 0.75f, 0.75f, 0.75f, 1.0f };
GLfloat BallDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat BallSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat BallShininess = 128.0f;
GLfloat Light_Pos[] = { 0.0f, -20.0f, 80.0f, 0.0f };
GLfloat Light_Dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat Ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat Diffuse[] = { 0.5f, 0.9f, 0.5f, 1.0f };
GLfloat Specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };

float acceleration = 0.02f;
float cameraAngle = 32.0f;
float playerX, playerY, accelX, accelY, velocityX, velocityY;
int i, topBoundary, bottomBoundary, moveDistance, hAngle, vAngle, tempPosition;
int directionHorizontal = -1, directrionVertical = -1;
char isPressHor = 0, isPressVer = 0;
int angleAccel[] = { 2, -2 }, maxAngleAccel[] = { 30, -30 };
char txtBuffer[60];
bool gameCleared = false;

int Map[MAX][MAX];
bool Path[MAX][MAX];
int finishX, finishY;

enum Direction {
    DIRECTION_LEFT,
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN
};

enum MapFlag {
    MAP_FLAG_WALL,
    MAP_FLAG_EMPTY,
    MAP_FLAG_VISITED,
};

const int DIR[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

void shuffleArray(int array[], int size) {
    for (int i = 0; i < (size - 1); ++i) {
        int r = i + (rand() % (size - i));
        int temp = array[i];
        array[i] = array[r];
        array[r] = temp;
    }
}

bool inRange(int y, int x) {
    return (y < MAX - 1 && y > 0) && (x < MAX - 1 && x > 0);
}

void generateMap(int y, int x) {
    int directions[4] = {
        DIRECTION_UP,
        DIRECTION_RIGHT,
        DIRECTION_DOWN,
        DIRECTION_LEFT
    };

    Map[y][x] = MAP_FLAG_VISITED;

    shuffleArray(directions, 4);

    for (int i = 0; i < 4; i++) {
        int nx = x + DIR[directions[i]][0] * 2;
        int ny = y + DIR[directions[i]][1] * 2;

        if (inRange(ny, nx) && Map[ny][nx] == MAP_FLAG_WALL) {
            Map[(ny + y) / 2][x] = MAP_FLAG_EMPTY;
            Map[y][(nx + x) / 2] = MAP_FLAG_EMPTY;
            Map[ny][nx] = MAP_FLAG_EMPTY;

            generateMap(ny, nx);
        }
    }
}

std::pair<int, int> placeFinishPoint() {
    std::vector<std::pair<int, int>> corners = { {1, MAX - 2}, {MAX - 2, 1}, {MAX - 2, MAX - 2} };
    return corners[rand() % corners.size()];
}

void resetMap() {
    printf("\n");
    memset(Map, MAP_FLAG_WALL, sizeof(Map));
    memset(Path, false, sizeof(Path));
    generateMap(1, 1);

    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            if (Map[i][j] >= 1)
                Map[i][j] = 1;
            printf("%d ", Map[i][j]);
        }
        printf("\n");
    }

    playerX = -10.5f + 0; // Map[1][1]의 중심
    playerY = -10.5f + 0; // Map[1][1]의 중심
    velocityX = 0.0f;
    velocityY = 0.0f;
    hAngle = 0;
    vAngle = 0;
    accelX = 0.0f;
    accelY = 0.0f;
    gameCleared = false;
    isPressHor = 0;
    isPressVer = 0;
    directionHorizontal = -1;
    directrionVertical = -1;

    auto finish = placeFinishPoint();
    finishX = finish.second;
    finishY = finish.first;
    Map[finishY][finishX] = MAP_FLAG_EMPTY;

    printf("Finish point: (%d, %d)\n", finishY, finishX);

    glutPostRedisplay();
}

void Timer(int value);

void resetMapCallback(int value) {
    resetMap();
    glutTimerFunc(1, Timer, 0);
}

void Text_Bitmap(float x, float y, char* s) {
    glRasterPos2f(x, y);
    while (*s != '\0')
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(s++));
}

void Reload_TableView() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 50.0);
    glTranslatef(0.0f, 0.0f, -cameraAngle);
    glRotatef(-0.6f * hAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(0.6f * vAngle - 30, 1.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    accelX = -acceleration * sin(hAngle * PI / 180);
    accelY = -acceleration * sin(vAngle * PI / 180);
}

void Reload_TextView() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void drawBackground() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            glLoadIdentity();
            glTranslatef(-11.5f + j, -11.5f + i, -0.4f);
            if (Path[i][j])
                glColor3fv(PathColor);
            else
                glColor3fv(BGColor);
            glBegin(GL_QUADS);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, 0.5f, 0.0f);
            glVertex3f(-0.5f, 0.5f, 0.0f);
            glEnd();
        }
    }
}

void drawStartPoint() {
    glLoadIdentity();
    glTranslatef(-10.5f, -10.5f, -0.4f);
    glColor3fv(StartColor);
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();
}

void drawFinishPoint() {
    glLoadIdentity();
    glTranslatef(finishX - 11.5f, finishY - 11.5f, -0.4f);
    glColor3fv(FinishColor);
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();
}

void drawMaze() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (Map[i][j] == MAP_FLAG_WALL) {
                glLoadIdentity();
                glTranslatef(-11.5f + j, -11.5f + i, 0.0f);
                glColor3fv(WallDiffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, WallAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, WallDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, WallSpecular);
                glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
                glutSolidCube(1.0);
            }
        }
    }
}

void drawBall() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, BallAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, BallDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, BallSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, BallShininess);

    glLoadIdentity();
    glTranslatef(playerX, playerY, 0.5f);
    glutSolidSphere(0.5, 50, 50);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (gameCleared) {
        Reload_TextView();
        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(txtBuffer, "clear!");
        Text_Bitmap(-0.1f, 0.0f, txtBuffer);
        glutSwapBuffers();
        return;
    }

    Reload_TableView();
    drawStartPoint();
    drawFinishPoint();
    drawBackground();
    glEnable(GL_LIGHTING);
    drawMaze();
    drawBall();
    glDisable(GL_LIGHTING);
    Reload_TextView();
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(txtBuffer, "Camera angle: % .1f  Q to decrease, E to increase", cameraAngle);
    Text_Bitmap(-0.95f, 0.9f, txtBuffer);
    sprintf(txtBuffer, "Acceleration: % .2f  R to decrease, T to increase", acceleration);
    Text_Bitmap(-0.95f, 0.8f, txtBuffer);
    glutSwapBuffers();
}

void Init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.000f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Dif);
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Pos);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 90.0f);
    resetMap();
}

void Timer(int value) {
    if (gameCleared) return; // 게임이 클리어되면 타이머 중지

    // 가속도를 속도에 더해줌
    velocityX += accelX;
    velocityY += accelY;

    // 지면 마찰력을 속도에 곱해 속도를 줄임
    velocityX *= FRICTION_GROUND;
    velocityY *= FRICTION_GROUND;

    // 공의 X 방향 이동 처리
    if (velocityX > 0) {  // 오른쪽으로 이동
        topBoundary = (int)(playerY + 11.6f);  // 공의 Y 좌표(위쪽 경계)
        bottomBoundary = (int)(playerY + 12.4f);  // 공의 Y 좌표(아래쪽 경계)
        moveDistance = (int)velocityX;  // 이동할 거리
        tempPosition = (int)(playerX + 43.5f) - 30; // 새로운 위치 계산
        // 이동할 거리만큼 반복
        for (i = 0; i < moveDistance; i++) {
            // 만약 이동할 위치가 벽이면 이동 중단
            if (!Map[topBoundary][tempPosition] || !Map[bottomBoundary][tempPosition])
                break;
            tempPosition++;
        }
        if (i == moveDistance) {
            // 만약 이동할 수 있는 거리만큼 이동할 수 있으면 위치 업데이트
            tempPosition = (int)(playerX + velocityX + 42.5f) - 30;
            if (Map[topBoundary][tempPosition] && Map[bottomBoundary][tempPosition])
                i++;
        }
        if (i > moveDistance) {
            playerX += velocityX;
        }
        else {
            // 벽에 충돌하면 위치를 업데이트하고 속도를 반사시킴
            playerX = (int)(playerX + i + 25) - 24.5f;
            velocityX *= -FRICTION_WALL;
        }
    }
    else {  // 왼쪽으로 이동
        topBoundary = (int)(playerY + 11.6f);  // 공의 Y 좌표(위쪽 경계)
        bottomBoundary = (int)(playerY + 12.4f);  // 공의 Y 좌표(아래쪽 경계)
        moveDistance = -(int)velocityX;  // 이동할 거리 (음수값을 양수로 변경)
        tempPosition = (int)(playerX + 40.5f) - 30;  // 새로운 위치 계산
        // 이동할 거리만큼 반복
        for (i = 0; i < moveDistance; i++) {
            // 만약 이동할 위치가 벽이면 이동 중단
            if (!Map[topBoundary][tempPosition] || !Map[bottomBoundary][tempPosition])
                break;
            tempPosition--;
        }
        if (i == moveDistance) {
            // 만약 이동할 수 있는 거리만큼 이동할 수 있으면 위치 업데이트
            tempPosition = (int)(playerX + velocityX + 41.5f) - 30;
            if (Map[topBoundary][tempPosition] && Map[bottomBoundary][tempPosition])
                i++;
        }
        if (i > moveDistance) {
            playerX += velocityX;
        }
        else {
            // 벽에 충돌하면 위치를 업데이트하고 속도를 반사시킴
            playerX = (int)(playerX - i + 25) - 24.5f;
            velocityX *= -FRICTION_WALL;
        }
    }

    // 공의 Y 방향 이동 처리
    if (velocityY > 0) {  // 위쪽으로 이동
        topBoundary = (int)(playerX + 11.6f);  // 공의 X 좌표(왼쪽 경계)
        bottomBoundary = (int)(playerX + 12.4f);  // 공의 X 좌표(오른쪽 경계)
        moveDistance = (int)velocityY;  // 이동할 거리
        tempPosition = (int)(playerY + 43.5f) - 30;  // 새로운 위치 계산
        // 이동할 거리만큼 반복
        for (i = 0; i < moveDistance; i++) {
            // 만약 이동할 위치가 벽이면 이동 중단
            if (!Map[tempPosition][topBoundary] || !Map[tempPosition][bottomBoundary])
                break;
            tempPosition++;
        }
        if (i == moveDistance) {
            // 만약 이동할 수 있는 거리만큼 이동할 수 있으면 위치 업데이트
            tempPosition = (int)(playerY + velocityY + 42.5f) - 30;
            if (Map[tempPosition][topBoundary] && Map[tempPosition][bottomBoundary])
                i++;
        }
        if (i > moveDistance)
            playerY += velocityY;
        else {
            // 벽에 충돌하면 위치를 업데이트하고 속도를 반사시킴
            playerY = (int)(playerY + i + 25) - 24.5f;
            velocityY *= -FRICTION_WALL;
        }
    }
    else {  // 아래쪽으로 이동
        topBoundary = (int)(playerX + 11.6f);  // 공의 X 좌표(왼쪽 경계)
        bottomBoundary = (int)(playerX + 12.4f);  // 공의 X 좌표(오른쪽 경계)
        moveDistance = -(int)velocityY;  // 이동할 거리 (음수값을 양수로 변경)
        tempPosition = (int)(playerY + 40.5f) - 30;  // 새로운 위치 계산
        // 이동할 거리만큼 반복
        for (i = 0; i < moveDistance; i++) {
            // 만약 이동할 위치가 벽이면 이동 중단
            if (!Map[tempPosition][topBoundary] || !Map[tempPosition][bottomBoundary])
                break;
            tempPosition--;
        }
        if (i == moveDistance) {
            // 만약 이동할 수 있는 거리만큼 이동할 수 있으면 위치 업데이트
            tempPosition = (int)(playerY + velocityY + 41.5f) - 30;
            if (Map[tempPosition][topBoundary] && Map[tempPosition][bottomBoundary])
                i++;
        }
        if (i > moveDistance)
            playerY += velocityY;
        else {
            // 벽에 충돌하면 위치를 업데이트하고 속도를 반사시킴
            playerY = (int)(playerY - i + 25) - 24.5f;
            velocityY *= -FRICTION_WALL;
        }
    }

    // 도착 지점 도달 체크 (작은 오차 허용)
    if (fabs(playerX - (finishX - 11.5f)) < 0.5f && fabs(playerY - (finishY - 11.5f)) < 0.5f) {
        gameCleared = true;
        glutPostRedisplay();
        glutTimerFunc(2000, resetMapCallback, 0); // 2초 후에 맵 리셋
        return;
    }

    // 수평 방향 회전 처리
    if (directionHorizontal > -1) {
        if (hAngle == 0) {
            if (!isPressHor)
                directionHorizontal = -1;
            else
                hAngle += angleAccel[directionHorizontal];
        }
        else if (hAngle != maxAngleAccel[directionHorizontal])
            hAngle += angleAccel[directionHorizontal];
    }

    // 수직 방향 회전 처리
    if (directrionVertical > -1) {
        if (vAngle == 0) {
            if (!isPressVer)
                directrionVertical = -1;
            else
                vAngle += angleAccel[directrionVertical];
        }
        else if (vAngle != maxAngleAccel[directrionVertical])
            vAngle += angleAccel[directrionVertical];
    }

    // 디스플레이 함수 호출하여 화면 갱신
    glutPostRedisplay();
    // 25ms 후에 다시 Timer 함수를 호출
    glutTimerFunc(1, Timer, 0);
}

void Special(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        directionHorizontal = 0;
        isPressHor = 1;
        break;
    case GLUT_KEY_RIGHT:
        directionHorizontal = 1;
        isPressHor = 1;
        break;
    case GLUT_KEY_DOWN:
        directrionVertical = 0;
        isPressVer = 1;
        break;
    case GLUT_KEY_UP:
        directrionVertical = 1;
        isPressVer = 1;
        break;
    }
}

void Special_Up(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        if (directionHorizontal == 0) {
            directionHorizontal = 1;
            isPressHor = 0;
        }
        break;
    case GLUT_KEY_RIGHT:
        if (directionHorizontal == 1) {
            directionHorizontal = 0;
            isPressHor = 0;
        }
        break;
    case GLUT_KEY_DOWN:
        if (directrionVertical == 0) {
            directrionVertical = 1;
            isPressVer = 0;
        }
        break;
    case GLUT_KEY_UP:
        if (directrionVertical == 1) {
            directrionVertical = 0;
            isPressVer = 0;
        }
        break;
    }
}

void Keyboard(GLubyte key, int x, int y) {
    switch (key) {
    case 'q':
        if (cameraAngle > CAMERA_MIN)
            cameraAngle -= 1.0f;
        break;
    case 'e':
        if (cameraAngle < CAMERA_MAX)
            cameraAngle += 1.0f;
        break;
    case 'r':
        if (acceleration > ACCELERATION_MIN)
            acceleration -= 0.01f;
        break;
    case 't':
        if (acceleration < ACCELERATION_MAX)
            acceleration += 0.01f;
        break;
    case 'a': {
        int newX = (int)(playerX - 1.0f + 11.5f);
        if (newX > 0 && Map[(int)(playerY + 11.5f)][newX] != MAP_FLAG_WALL) {
            playerX = newX - 11.5f;
            velocityX = 0.0f;
            velocityY = 0.0f;
            accelX = 0.0f;
            accelY = 0.0f;
        }
        break;
    }
    case 'd': {
        int newX = (int)(playerX + 1.0f + 11.5f);
        if (newX < MAX - 1 && Map[(int)(playerY + 11.5f)][newX] != MAP_FLAG_WALL) {
            playerX = newX - 11.5f;
            velocityX = 0.0f;
            velocityY = 0.0f;
            accelX = 0.0f;
            accelY = 0.0f;
        }
        break;
    }
    case 's': {
        int newY = (int)(playerY - 1.0f + 11.5f);
        if (newY > 0 && Map[newY][(int)(playerX + 11.5f)] != MAP_FLAG_WALL) {
            playerY = newY - 11.5f;
            velocityX = 0.0f;
            velocityY = 0.0f;
            accelX = 0.0f;
            accelY = 0.0f;
        }
        break;
    }
    case 'w': {
        int newY = (int)(playerY + 1.0f + 11.5f);
        if (newY < MAX - 1 && Map[newY][(int)(playerX + 11.5f)] != MAP_FLAG_WALL) {
            playerY = newY - 11.5f;
            velocityX = 0.0f;
            velocityY = 0.0f;
            accelX = 0.0f;
            accelY = 0.0f;
        }
        break;
    }
    }
}

void Keyboard_Up(GLubyte key, int x, int y) {
    /*switch (key) {
    case 'a':
    case 'd':
        velocityX = 0.0f;
        break;
    case 's':
    case 'w':
        velocityY = 0.0f;
        break;
    }*/
}

bool BFS(int startX, int startY) {
    if (Map[startY][startX] == MAP_FLAG_WALL || Map[finishY][finishX] == MAP_FLAG_WALL) {
        return false;
    }

    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(MAX, std::vector<bool>(MAX, false));
    std::vector<std::vector<std::pair<int, int>>> parent(MAX, std::vector<std::pair<int, int>>(MAX, { -1, -1 }));

    q.push({ startY, startX });
    visited[startY][startX] = true;

    while (!q.empty()) {
        auto [y, x] = q.front();
        q.pop();

        if (y == finishY && x == finishX) {
            std::pair<int, int> p = { finishY, finishX };
            while (p != std::make_pair(startY, startX)) {
                Path[p.first][p.second] = true;
                p = parent[p.first][p.second];
            }
            Path[startY][startX] = true;
            return true;
        }

        for (const auto& dir : DIR) {
            int ny = y + dir[0];
            int nx = x + dir[1];

            if (inRange(ny, nx) && !visited[ny][nx] && Map[ny][nx] != MAP_FLAG_WALL) {
                q.push({ ny, nx });
                visited[ny][nx] = true;
                parent[ny][nx] = { y, x };
            }
        }
    }
    return false;
}

void menu(int num) {
    if (num == 0) {
        resetMap();
    }
    else if (num == 1) {
        if (BFS(1, 1)) {
            std::cout << "Path found!" << std::endl;
            for (int i = 0; i < MAX; ++i) {
                for (int j = 0; j < MAX; ++j) {
                    if (Path[i][j]) {
                        printf("1 ");
                    }
                    else {
                        printf("0 ");
                    }
                }
                printf("\n");
            }
        }
        else {
            std::cout << "No path found!" << std::endl;
        }
    }
}

void createMenu() {
    glutCreateMenu(menu);
    glutAddMenuEntry("Reset Map", 0);
    glutAddMenuEntry("Find Path", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Tilting Maze Game");
    Init();
    glutSpecialFunc(Special);
    glutSpecialUpFunc(Special_Up);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(Keyboard_Up);
    glutTimerFunc(0, Timer, 0);
    glutDisplayFunc(Display);
    createMenu();
    glutMainLoop();
}
