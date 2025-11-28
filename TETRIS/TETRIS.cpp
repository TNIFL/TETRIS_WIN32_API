// TETRIS.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "TETRIS.h"

#include <stdlib.h>
#include <time.h>
#include <array>
#define MAX_LOADSTRING 100
#define TETROMINO_COUNT 7       /// 테트로미노 갯수
#define BOARD_W 12              /// 게임 보드 너비
#define BOARD_H 27              /// 게임 보드 높이

/// 열거형으로 테트로미노 선언

enum TetrominiType {
    I = 0, O, T, S, Z, J, L
    ///I = 0, L = 6
};

/// 열거형으로 회전 각도 선언

enum Rotation {
    ROT_0 = 0, ROT_90,
    ROT_180, ROT_270
};

/// 한 칸의 자료형을 bool 로 할지 BYTE 로 할지 모르겠음
/// 둘 다 같은 사이즈를 가지긴 했는데
/// 명확성을 따지면 bool 자료형이 좀 더 좋을거같음
/// 근데 바꾸기 귀찮음
/// 일단 이대로 하다가 안될거같으면 bool로 바꿀거임

/// 4 x 4 크기의 배열 필요 - 테트로미노 담아야함
using Shape = std::array<std::array<BYTE, 4>, 4>;

/// 게임 보드 배열 필요 - 게임 플레이 영역
using GameBoard = std::array<std::array<BYTE, BOARD_W>, BOARD_H>;


/// 각 테트로미노가 어떤 좌표를 가지는지 초기 설정
/// 7, 4 -> 테트로미노가 총 7개, 로테이션(회전) 4개
/// 내 테트로미노 구조는 [0~6:블럭 타입][0~3:ROT_0~270][(0~3)y:비트맵][(0~3)x:비트맵]
static const Shape TETROMINO[7][4] = {
    /// I
    {
        /// ROT_0
        {{{0,0,0,0}, 
          {1,1,1,1}, 
          {0,0,0,0}, 
          {0,0,0,0}}},

        ///ROT_90
        {{{0,0,1,0},
          {0,0,1,0},
          {0,0,1,0},
          {0,0,1,0}}},

        ///ROT_180
        {{{0,0,0,0},
          {1,1,1,1},
          {0,0,0,0},
          {0,0,0,0}}},

        ///ROT_270
        {{{0,0,1,0},
          {0,0,1,0},
          {0,0,1,0},
          {0,0,1,0}}},

    },
    /// O
    {   
        /// ROT_0
        {{{0,0,0,0},
          {0,1,1,0},
          {0,1,1,0},
          {0,0,0,0}}},

        /// ROT_90
        {{{0,0,0,0},
          {0,1,1,0},
          {0,1,1,0},
          {0,0,0,0}}},

        /// ROT_180
        {{{0,0,0,0},
          {0,1,1,0},
          {0,1,1,0},
          {0,0,0,0}}},

        /// ROT_270
        {{{0,0,0,0},
          {0,1,1,0},
          {0,1,1,0},
          {0,0,0,0}}},
    },
    /// T
    {
        /// ROT_0
        {{{0,0,0,0},
          {0,1,1,1},
          {0,0,1,0},
          {0,0,0,0}}},

        /// ROT_90
        {{{0,0,1,0},
          {0,1,1,0},
          {0,0,1,0},
          {0,0,0,0}}},

        /// ROT_180
        {{{0,0,1,0},
          {0,1,1,1},
          {0,0,0,0},
          {0,0,0,0}}},

        /// ROT_270
        {{{0,0,1,0},
          {0,0,1,1},
          {0,0,1,0},
          {0,0,0,0}}},
    },
    /// S
    {
        /// ROT_0
        {{{0,0,0,0},
          {0,0,1,1},
          {0,1,1,0},
          {0,0,0,0}}},

        /// ROT_90
        {{{0,1,0,0},
          {0,1,1,0},
          {0,0,1,0},
          {0,0,0,0}}},
    
        /// ROT_180
        {{{0,0,0,0},
          {0,0,1,1},
          {0,1,1,0},
          {0,0,0,0}}},

        /// ROT_270
        {{{0,1,0,0},
          {0,1,1,0},
          {0,0,1,0},
          {0,0,0,0}}},
    },
    /// Z
    {
        /// ROT_0
        {{{0,0,0,0},
          {0,1,1,0},
          {0,0,1,1},
          {0,0,0,0}}},
        
        /// ROT_90
        {{{0,0,1,0},
          {0,1,1,0},
          {0,1,0,0},
          {0,0,0,0}}},
        
        /// ROT_180
        {{{0,0,0,0},
          {0,1,1,0},
          {0,0,1,1},
          {0,0,0,0}}},
        
        /// ROT_270
        {{{0,0,1,0},
          {0,1,1,0},
          {0,1,0,0},
          {0,0,0,0}}},
    },
    /// J
    {
        ///ROT_0
        {{{0,0,0,0},
          {0,1,1,1},
          {0,0,0,1},
          {0,0,0,0}}},

        ///ROT_90
        {{{0,0,1,0},
          {0,0,1,0},
          {0,1,1,0},
          {0,0,0,0}}},
          
        ///ROT_180
        {{{0,1,0,0},
          {0,1,1,1},
          {0,0,0,0},
          {0,0,0,0}}},
          
        ///ROT_270
        {{{0,0,1,1},
          {0,0,1,0},
          {0,0,1,0},
          {0,0,0,0}}},
    },
    /// L
    {
        /// ROT_0
        {{{0,0,0,0},
          {0,1,1,1},
          {0,1,0,0},
          {0,0,0,0}}},

        /// ROT_90
        {{{0,1,1,0},
          {0,0,1,0},
          {0,0,1,0},
          {0,0,0,0}}},
          
        /// ROT_180
        {{{0,0,0,1},
          {0,1,1,1},
          {0,0,0,0},
          {0,0,0,0}}},

        /// ROT_270
        {{{0,0,1,0},
          {0,0,1,0},
          {0,0,1,1},
          {0,0,0,0}}},
    },
};


static const GameBoard GB = {
    {{1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,0,0,0,1},
     {1,1,1,1,1,1,1,1,1,1,1,1}}
};

/// 테트로미노 구조체
struct Piece {
    TetrominiType type;   /// TETROMINO (0 ~ 6)
    Rotation rot;    /// ROT_ 0 ~ 270 (0, 90, 180, 270)
    int blockX, blockY;   /// 보드에서의 좌표(왼쪽 위 기준)
    int r, g, b;/// 테트로미노의 고유한 색상
};

/// 현재 떨어지고 있는 블럭의 정보를 담은 구조체
Piece currentPiece;

/// 현재 Piece 구조체 사용 안하고있음
/// 색상도 넣고 사용하려면 구조체 안에 여러 정보들이 있어야함
/// 

/// !!! 게임 보드 영역의 좌표는 무조건 왼쪽 위 부터 오른쪽 아래로 갈 수록 커짐
/// g_board[BOARD_H][BOARD_W] 로 정의 한 이유
/// 행 -> 열 순으로 위에서부터 아래로 행을 쌓아간다
/// 
struct Cell {
    bool isWall;        /// 벽이면 true
    bool fix;           /// 고정상태면 true
    int r, g, b;        /// 고정된 블럭의 색
};

Cell g_board[BOARD_H][BOARD_W];

/// g_board 에도 색상을 넣기 위해서는 색상 정보를 저장할 변수들이 필요함
/// 그렇다면 차라리 g_board도 구조체로 만들어버림
/// cell 구조체를 만들고
/// cell 구조체형식의 g_board 로 생성

/// 게임 플레이 영역의 벽 초기화
void InitBoard() {
    /// 위 빼고 왼쪽, 오른쪽, 아래만 그리는 로직을 다시 구성
    /// 위의 로직은 이상하게 그려짐
    /// 그려지는 순서는 위에서 아래로
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 왼쪽 벽 ------- 오른쪽 벽
    /// 아래쪽아래쪽아래쪽아래쪽
    /// 이렇게 그려져야함
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            /// 모든 색상을 rgb 0 0 0 으로 초기화
            g_board[y][x].r = g_board[y][x].g = g_board[y][x].b = 0;
            /// x 가 0 이면 왼쪽 벽
            if (x == 0) {
                g_board[y][x].isWall = TRUE;  /// y행의 x열을 1로(왼쪽 벽)
            }
            /// x 가 BOARD_W - 1 이면(인덱스 기준이라 -1 해야함) 오른쪽 벽
            else if (x == BOARD_W - 1) {
                g_board[y][x].isWall = TRUE;  /// y행의 x열을 1로(오른쪽 벽)
            }
            /// y 가 BOARD_H - 1 이면(인덱스 기준이라 -1 해야함) 아래 벽
            else if (y == BOARD_H - 1) {
                g_board[y][x].isWall = TRUE;  /// y행의 모든 x 를 1로(아래 벽)
            }
        }
    }
}
/// 위 처럼 InitBoard() 가 있어야하고
/// 그리고 그려야 함
/// 그리기 함수는
/// 선 그리기
///  - MoveToEx(hdc, g_x, g_y, NULL);
///  - LineTo(hdc, x, y);
/// 사각형 그리기
///  - Rectangle(hdc, g_x, g_y, x, y);
/// 원 그리기
///  - Ellipse(hdc, x,y,x1,y1);
/// 만약 WM_PAINT 밖에서 그리면
/// GetDC 로 hdc 를 얻고 그려야함
///  - ReleaseDC 로 항상 해제 해줘야함
/// 핸들을 얻었으면 항상 해제 하는 버릇을 들여야함
/// 만약 핸들을 해제하지 않았다면 메모리 누수나 자원 관리가 안될수도있음
/// 프로그래머가 WM_PAINT를 요청하는 API
///  - InvalidateRect(hWnd, NULL, TRUE); -> 현재 화면을 다시 그려주세요
/// 모든 화면 그리기는 항상 연산이 끝나고 나서 움직인다
const int BLOCK = 30;       /// 한개의 블럭은 30의 크기를 가짐
const int ORIGIN_X = 70;    /// 이 게임 보드의 시작 위치는 70, 70(왼쪽 위 기준)
const int ORIGIN_Y = 70;
void DrawGameBoard(HWND hWnd, HDC hdc) {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {

            int px = ORIGIN_X + x * BLOCK;  /// 보드의 x 번째 칸이 화면에서 몇 픽셀에 있어야하는지
            int py = ORIGIN_Y + y * BLOCK;  /// 보드의 y 번째 칸이 화면에서 몇 필셀에 있어야하는지
            /// cell 을 사용하지 않고
            /// g_board[y][x].isWall 을 사용해서
            /// 만약 TRUE 라면 벽 색을 회색으로 입히고
            /// 만약 g_board[y][x].fix 가 TRUE 라면
            /// 고정된 블럭이니 블럭의 색상을 g_board[y][x] 에 입힌다
            if (g_board[y][x].isWall) {
                HBRUSH myBrush = CreateSolidBrush(RGB(150, 150, 150));
                /// os 가 들고있는 브러쉬를 myBrush 로 바꿔버림
                HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);

                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);

                /// 다 쓴 브러쉬를 교환하고
                SelectObject(hdc, osBrush);
                /// 정리
                DeleteObject(myBrush);
            }
            else if (g_board[y][x].fix) {
                /// 고정된 블럭의 rgb 값으로 g_board[y][x] 좌표를 채운다
                HBRUSH myBrush = CreateSolidBrush(
                    RGB(g_board[y][x].r,
                        g_board[y][x].g,
                        g_board[y][x].b));
                HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);
                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);
                SelectObject(hdc, osBrush);
                DeleteObject(myBrush);
            }
            /// 빈 칸 그리기(흰색)
            else {
                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);
            }
        }
    }
}

/// 현재 내가 조작하고 있는 블럭 종류
/// SpawnBlock 할 때 값을 할당해줄거임
int currentBlock[4][4];   /// 현재 조작중인 블럭 4 x 4 크기


/// 블럭이 내려와야함
/// 근데 이미 그려진 영역 안에서만 블럭이 내려와야하고 
/// 한번 내려올 때는 30px 씩 내려옴
/// 먼저 내려올 블럭이 생성될 위치는 220px 또는 250px 둘 중 하나의 위치에(왼쪽 위 기준) 블럭 생성
/// 우선은 그냥 네모난 블럭만 생성해서 내려오게 해봄
void DrawDownBlock(HWND hWnd, HDC hdc) {
    /// 근데 타이머로 움직여야하나?
    /// 정해진 시간마다 움직이게 하려면 타이머 말고 다른게 있을까..
    
}

/// 현재 점수를 보여줄 영역
/// 점수 영역은 게임 플레이 영역의 위 쪽
/// 게임 플레이 영역과 가로의 길이가 같다
/// 세로 길이는 50px?

/// 스코어 보드의 위치는 10,10 에서 그려지기 시작함
const int SCORE_ORIGIN_X = 70;
const int SCORE_ORIGIN_Y = 10;
void DrawScoreBoard(HWND hWnd, HDC hdc) {
    Rectangle(hdc, SCORE_ORIGIN_X, SCORE_ORIGIN_Y, SCORE_ORIGIN_X + (BLOCK * BOARD_W), SCORE_ORIGIN_Y + 50);
}

/// 최고점수 영역 그리기
void DrawHighScoreBoard(HWND hWnd, HDC hdc) {

}
/// 다음 블럭 영역 그리기(최고점수 영역 바로 아래)
void DrawNextBlockArea() {

}
/// 다음 레벨 알려주는 영역 그리기(다음 블럭 영역 그리기 바로 아래)
void DrawNextLevelArea() {

}

/// 내려온 블럭이 아래에 있던 벽이나 이미 설치되어있는 블럭에 닿았는지 검사
/// 그리고 블럭 위치 고정시키기
void FixBlock() {
    /// 블럭을 고정시키려면
    /// currentBlock이 고정되었을 시점에
    /// InitBoard 현재 블럭의 값을 추가함
    /// 다음번에 또 초기화 될 때에는 고정된 블럭이 추가 된 상태로
    /// InitBoard 가 호출이 될거임
    /// 하려면 g_board[BOARD_H][BOARD_W]에 추가해야함
    /// 추가하는 방법은 어떻게 하지 시발
    /// g_board 에서 블럭이 있는 위치를 1로 바꿔줌
    /// BlockX, BlockY 에 있음 근데 이 BlockX, BlockY 는
    /// currentblock 의 어디 기준이지?
    /// currentBlock 의 (0, 0) 위치를 기준으로 잡아놓음
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x] == 1) {
                /// 이 상황에서 [y][x] 가 1 이면
                /// 실제로 currentBlock 이 위치하는 좌표는
                /// blockX + x, blockY + y 이다
                /// fx = fix x, fy = fix y
                int fx = currentPiece.blockX + x;    /// 블럭을 고정하기 위해 얻은 좌표
                int fy = currentPiece.blockY + y;    /// 블럭을 고정하기 위해 얻은 좌표
                /// g_board[fy][fx] 멤버의 값을 설정해줘야함
                g_board[fy][fx].isWall = FALSE;   /// 벽이 아님
                g_board[fy][fx].fix = TRUE;       /// 고정 시키기
                g_board[fy][fx].r = currentPiece.r;
                g_board[fy][fx].g = currentPiece.g;
                g_board[fy][fx].b = currentPiece.b;

            }
        }
    }
}   

/// 다음에 나올 블럭을 미리 보여줌
void ShowNextBlock() {

}

void LoadCurrentPiece() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            /// 내 테트로미노 구조는 [0~6:블럭 타입][0~3:ROT_0~270][y:비트맵][x:비트맵]
            currentBlock[y][x] = TETROMINO[currentPiece.type][currentPiece.rot][y][x];
        }
    }
}

/// 벽에 닿았을 때 넘어가지 않게 해줌
BOOL CollisionDetection(int dx, int dy) {
    /// dx, dy 는 움직일 양을 뜻함
    /// 왼쪽 벽, 오른쪽 벽, 아래 벽 에 닿았을 때 넘어가지 않게 해줌
    /// 현재 테트로미노의 인덱스 기준으로 닿음을 확인함
    /// 그러면 1칸 떨어지고 닿는 경우도 생김 
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x] == 0) {
                /// currentBlock 인덱스 내부에서 0 이면 없는 블럭
                /// 없는 블럭이면 무시
                continue;
            }
            int nx = currentPiece.blockX + x + dx;   /// 움직이고 나서의 게임보드에서의 블럭 위치
            int ny = currentPiece.blockY + y + dy;   /// 움직이고 나서의 게임보드에서의 블럭 위치

            /// 움직이고 나서 보드 밖으로 나갔는지 확인
            if (nx < 0 || nx >= BOARD_W || ny < 0 || ny >= BOARD_H) {
                return FALSE;
            }
            /// g_board[ny][nx] 가 1 이면 벽임
            /// 이동불가
            /// 그리고 고정된 블럭에도 못감
            if (g_board[ny][nx].isWall || g_board[ny][nx].fix) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/// 해당 x 배열을 지우기 위한 함수
/// y 층의 모든 배열을 지우기
void ClearLine(int clearY, HWND hWnd) {
    /// FullLine 으로 부터 가져온 y 층을 검사
    /// y 를 clearY 로 바꿔서
    /// y 가 0 보다 클 때 까지 빼면서
    /// 아래서 부터 위로 올라가면서 검사를 진행하게 된다
    
    for (int i = clearY; i > 0; i--) {
        /// 왼쪽 벽과 오른쪽 벽을 제외하고
        /// 지우게 될 층을 지우게 될 층 바로 윗 층으로 배열을 복사하여 덮어쓴다
        /// 해당 층의 모든 배열을 가지고 오게 되므로 빈 칸도 마찬가지로 가져와서
        /// 빈칸은 빈칸으로 채워지게 된다
        for (int x = 1; x < BOARD_W - 1; x++) {
            /// 지워진 층을 바로 윗 층으로 덮어쓴다
            g_board[i][x] = g_board[i - 1][x];
        }
    }  
    InvalidateRect(hWnd, NULL, FALSE);
}

/// 배열이 가득 찼으면 그 라인을 터뜨리고 위의 라인을 가져오는 함수
void FullLine(HWND hWnd) {
    /// g_board 의 모든 라인을 조사해서
    /// y 배열의 맨 아랫줄을 빼고
    /// 루프의 방향은 맨 아래에서부터 맨 위 까지 검사
    /// 그러므로 for 문의 시작은 BOARD_H - 1 부터 > 0 까지
    for (int y = BOARD_H - 1; y > 0; y--) {
        /// isFull 은 해당 y 배열이 가득 찼는지 확인하기 위해 존재하는 변수
        BOOL isFull = TRUE;
        for (int x = 1; x < BOARD_W - 1; x++) {
            if (g_board[y][x].fix == FALSE) {
                isFull = FALSE;
                break;
            }
        }
        /// 만약 y 배열이 가득 차있다면
        if (isFull) {
            
            InvalidateRect(hWnd, NULL, FALSE);
            /// 바로 사라지면 어색하니 Sleep 0.2 초 걸어두기
            Sleep(200);
            /// 블럭 끌어내리기
            ClearLine(y, hWnd);
            /// 라인이 지워지고 윗줄이 내려왔으므로
            /// 지금 막 내려온 y 층을 검사하기 위해 y 값을 증가시켜 한번 더 검사한다
            /// y++ 하고 isFull 이 FALSE 면 알아서 스킵하게 된다
            y++;
        }
    }
}



/// 블럭을 스폰시킴
void SpawnBlock() {
    
    /// 초기 블럭의 스폰 위치는 y : 70px, x : 220px 또는 250px
    /// 랜덤함수를 이용해서 0~6 사이의 값을 하나 받음
    /// 그 값에 해당하는 테트로미노를 하나 생성함
    /// 근데 그걸 어케하냐?
    /// 우선 rand 함수로 0~6 중에 하나 고름
    currentPiece.type = (TetrominiType)(rand() % 7);  /// 0~6 사이 값 나옴, 스폰시킬 블럭의 타입이 정해짐
    currentPiece.rot = ROT_0;        /// 회전 기본값
    /// 그리고 블럭의 초기 위치도 정해줘야함
    /// 초기 위치를 가지고 있는 변수는 blockX, blockY
    currentPiece.blockX = BOARD_W / 2 - 2;
    currentPiece.blockY = 0;
    
    /// 그리고 currentPiece 의 색상도 미리 정해줘야함
    switch (currentPiece.type) {
    case I:
        currentPiece.r = 0; 
        currentPiece.g = 255; 
        currentPiece.b = 255; 
        break;
    case O:
        currentPiece.r = 255; 
        currentPiece.g = 255; 
        currentPiece.b = 0; 
        break;
    case T:
        currentPiece.r = 128; 
        currentPiece.g = 0; 
        currentPiece.b = 128;
        break;
    case S:
        currentPiece.r = 0;
        currentPiece.g = 255;
        currentPiece.b = 0;
        break;
    case Z:
        currentPiece.r = 255; 
        currentPiece.g = 0; 
        currentPiece.b = 255;
        break;
    case J:
        currentPiece.r = 0; 
        currentPiece.g = 0;
        currentPiece.b = 255; 
        break;
    case L:
        currentPiece.r = 255; 
        currentPiece.g = 165; 
        currentPiece.b = 0;
        break;
    }

    LoadCurrentPiece();
    
}


/// 화면에 스폰시킨 블럭 그리기
void DrawCurrentBlock(HWND hWnd, HDC hdc) {
    HBRUSH myBrush = CreateSolidBrush(RGB(
        currentPiece.r, currentPiece.g, currentPiece.b
    ));  // 빨간 블럭
    HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);
    /// 여기서 x, y 는 블럭배열 안에서 1인 인덱스의 위치를 찾기 위해
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x] == 1) {  /// 1이면 해당 영역에 블럭이 존재
                /// blockX, blockY : currentBlock의 위치를 보드 기준으로 나타내는 좌표
                int px = ORIGIN_X + (currentPiece.blockX + x) * BLOCK;
                int py = ORIGIN_Y + (currentPiece.blockY + y) * BLOCK;
                /// 블럭 그리기
                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);
            }
        }
    }
}

/*
    선 속성 변경
    HPEN myPen, osPen
    myPen = CreatePen(PS_SOLID, 10, RGB(200, 100, 100));
    osPen = (HPEN)SelectObject(hdc, myPen);
    위 과정은 펜을 교체하여 프로그래머가 원하는 펜으로 그림을 그림
    다 그렸으면
    SelectObject(hdc, osPen);
    DeleteObject(myPen);
    해서 펜 반납 후 오브젝트 삭제 해야함

    브러시
    HBRUSH myBrush, osBrush;
    myBrush = CreateSolidBrush(RGB(0,0,0));
    osBrush = (HBURSH)SelectObject(hdc, myBrush);
    해서 사용 후
    원래 브러쉬로 복원 해야함
    SelectObejct(hdc, osBrush);
    DeleteObject(myBrush);

*/

/// 필요함 함수들
/// 벽 초기화
/// 게임오버 확인



// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    /// 랜덤 시드 초기화
    srand(time(NULL));
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TETRIS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
///테트리스를 만들기 위한 사고
///테트리스는 여러 종류의 블럭들을 특정 공간 안에 빈 칸이 없게
///전부 다 쌓아야지만 해당 공간이 비어지는 로직이 있음
/// 그렇다면 이 공간은 어떻게 해야할까
/// 아이디어 1 => 각 층들을 전부 배열로 구현(2차원 배열)
/// 
/// 각각의 배열 안에 내가 만들어놓은 구조물(생성되는것들)을 TRUE, FALSE 로 구분하여 한 칸 씩 내려올 때 마다 보이고, 안보이고를 설정
/// 구조물의 이름은 테트로미노 라고 정의한다
///  'I', 'O', 'T', 'S', 'Z', 'J', 'L' - block 이 존재
/// I-block, O-block, T-block, S-block, Z-block, J-block, L-block
///
/// 
/// 우선 게임 판 있어야함
/// 게임을 하기 위해서는 구역이 정해져있어야함
/// 게임의 구역은 2차원 배열
/// 가로는 짝수
/// 세로는 짝수 또는 홀수로 선언
/// 가로 x 세로 => 12 x 60~70 정도
/// 총 배열의 크기는 약 780정도
/// 
/// 배열의 선언은 boolean 으로 해도 되려나 ?
/// 현재 배열에 존재하면 참 없으면 거짓?
/// 아직은 잘 모르겠다
/// 
/// 
/// 그리고 블럭들이 존재해야함
/// 블럭들은 전부 4개의 사각형으로 이루어져있음
/// 이 블럭들은 기준점이 있음
/// 기준점을 중심으로 좌로가면 -, 우로가면 +, 
/// 

/// 키보드 입력
///  - 키보드 화살표 왼쪽, 오른쪽 : 블럭 좌 우로 움직이기
///  - 키보드 화살표 아래쪽 : 블럭 빠르게 내리기(소프트 드롭)
///  - 키보드 A : 블럭 반시계 방향으로 돌리기 : 
///  - 키보드 S : 블럭 시계 방향으로 돌리기
///  - 키보드 F : 블럭 바로 내리기(하드 드롭)
///  - 키보드 esc : 일시정지 + 메뉴 노출
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    {
        switch (wParam) {
            /// 'A' 입력을 받으면 currentPiece 를 반시계로 돌림
            /// 처음엔 ROT_0 으로 내려오니
            /// 'A' 누르면 ROT_270으로 바뀌어야함
            /// ROT는 0 90 180 270 순으로
            ///       0  1   2   3  이 존재
            ///
            /// TODO:: 추후에 블럭을 빠르게 돌리면 끼임이나 덮어쓰는 현상을 고쳐야함 버그임
            case 'A':
            {
                /// 반시계 방향으로 회전
                /// ROT_0 부터 시작\
                /// 'A' 를 누를 때 마다 계속 -1 이 되어야하니까
                /// 3 -> 2 -> 1 -> 0 -> 3 -> 2 -> 1 -> 0 ,,,
                /// -1 해버리면 rot = -1 이 되고, 0~3 에서 벗어남
                /// (rot + 3) % 4 = 전 값
                /// (현재 ROT_180:2 + 3) % 4 = 1
                /// 1이 되어 ROT_90 으로 변함
                /// 시계방향은
                /// (rot + 1) % 4
                /// (ROT_180:2 + 1) % 4 = 3
                int a = ((currentPiece.rot + 3) % 4);
                Rotation nextRot = (Rotation)a;
                currentPiece.rot = nextRot;
                LoadCurrentPiece();
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case 'S':
            {
                /// currentPiece 를 시계방향으로 돌리기
                int a = ((currentPiece.rot + 1) % 4);
                Rotation nextRot = (Rotation)a;
                currentPiece.rot = nextRot;
                LoadCurrentPiece();
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_LEFT:
            {
                /// 만약 게임보드에서 -1 만큼 움직이는게 성공했을 시
                /// blockX-- 해주고 다시 그림
                if (CollisionDetection(-1, 0)) {
                    currentPiece.blockX--;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_RIGHT:
            {
                /// 만약 게임보드에서 +1 만큼 움직이는게 성공했을 시
                /// blockX++ 해주고 다시 그림
                if (CollisionDetection(1, 0)) {
                    currentPiece.blockX++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_DOWN:
            {
                /// 만약 게임 보드에서 y 가 1만큼 증가하는게 성공했을 시
                /// blockY++ 해주고 다시 그림
                if (CollisionDetection(0, 1)) {
                    currentPiece.blockY++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        }
    }
    break;
    case WM_CREATE:
    {
        /// 여기서 게임 시작하자마자
        /// 플레이 영역을 생성시킴
        /// InitBoard() 실행 
        /// InitBoard 실행 하면 맨 처음에 게임 보드가 생성됨
        InitBoard();
        SpawnBlock();
        /// 타이머 생성
        /// 0.5 초 마다 호출되는 타이머 ID = 1
        SetTimer(hWnd, 1, 900, NULL);
        
    }
    break;
    case WM_TIMER:
    {
        /// 타이머에서 블럭이 아래 벽에 닿았을 때 멈춰야함
        /// FixBlock(), SpawnBlock() 해줘야함
        if (wParam == 1) {
            /// 아래로 1칸 내려가는게 성공했을 시
            if (CollisionDetection(0, 1)) {
                /// 아래로 내려버림
                currentPiece.blockY++;
            }
            /// 아래로 1칸 내려가는게 실패했을 시
            else {
                /// 블럭을 그 위치에 고정시키고
                FixBlock();
                /// 고정시킨 다음에 x 배열이 가득 찼다면 해당 배열 빨간색으로 바꾸기
                FullLine(hWnd);
                /// 새로운 블럭을 생성
                SpawnBlock();
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            DrawGameBoard(hWnd, hdc);
            DrawScoreBoard(hWnd, hdc);
            DrawCurrentBlock(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
