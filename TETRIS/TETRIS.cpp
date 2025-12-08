// TETRIS.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "TETRIS.h"
#include <stdlib.h>
#include <time.h>
#include <array>

#define MAX_LOADSTRING 100
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


/// 각 테트로미노가 어떤 좌표를 가지는지 초기 설정
/// 7, 4 -> 테트로미노가 총 7개, 로테이션(회전) 4개
/// TETROMINO 는 불변 데이터, 참조해서 사용하는 용도. 실제로 게임 보드상에서 움직임은 currentBlock 이 담당한다
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

/// 게임 일시정지
/// esc 누르면 게임 일시정지
/// 한번더 누르면 재개
BOOL isPaused;

/// 테트로미노 구조체
struct Piece {
    TetrominiType type;   /// TETROMINO (0 ~ 6)
    Rotation rot;    /// ROT_ 0 ~ 270 (0, 90, 180, 270)
    int blockX, blockY;   /// 보드에서의 좌표(왼쪽 위 기준)
    int r, g, b;/// 테트로미노의 고유한 색상
};

/// 현재 떨어지고 있는 블럭의 정보를 담은 구조체
Piece currentPiece;

/// 다음에 생성될 블럭의 정보를 담은 구조체
Piece nextPiece;
/// nextPiece가 준비 되었는지 확인하는 플래그 변수
/// 처음 시작하자마자는 없음
BOOL hasNextPiece = FALSE;


/// !!! 게임 보드 영역의 좌표는 무조건 왼쪽 위 부터 오른쪽 아래로 갈 수록 커짐
/// 
struct Cell {
    bool isWall;        /// 벽이면 true
    bool isFixed;       /// 고정상태면 true
    int r, g, b;        /// 고정된 블럭의 색
};

Cell g_board[BOARD_H][BOARD_W];

/// SpawnBlock 할 때 값을 할당해줄거임
/// 게임 보드상에서 실제 움직임임을 행하려면 이 변수를
/// currentPiece 구조체와 함께 사용한다
int currentBlock[4][4];   /// 현재 조작중인 블럭 4 x 4 모양

/// 현재 점수
int currentScore;
/// 1000 점 단위로 속도가 점점 빨라짐
int nextSpeedUpScore = 1000;

/// SetTimer 에서 사용할 초 변수
/// 처음에는 1초
int second = 1000;

/// 한 턴에 저장 또는 불러오기를 둘 중 하나만 사용 가능하게 만드는 플래그 변수
/// 이 변수는 게임 시작할때 FALSE 로 초기화 되고
/// D, F 를 눌렀을 때 TRUE
/// SpawnBlock 호출될 때 FALSE 로 바꾸면
/// D 를 눌렀을 때 마지막에 SpawnBlock을 호출하기 때문에
/// 무한 저장 불러오기 가능해짐
/// 그럼 어디서 FALSE 로 또 바꿔야 하냐
/// FixBlock 이 불러와지는 시점에서 FALSE 로 바꿔줌
BOOL hasUsedSaveOrLoad;

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
const int BLOCK_SIZE = 25;       /// 한개의 블럭은 30의 크기를 가짐
const int ORIGIN_X = 70;    /// 이 게임 보드의 시작 위치는 70, 70(왼쪽 위 기준)
const int ORIGIN_Y = 70;
void DrawGameBoard(HWND hWnd, HDC hdc) {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {

            int px = ORIGIN_X + x * BLOCK_SIZE;  /// 보드의 x 번째 칸이 화면에서 몇 픽셀에 있어야하는지
            int py = ORIGIN_Y + y * BLOCK_SIZE;  /// 보드의 y 번째 칸이 화면에서 몇 필셀에 있어야하는지
            /// cell 을 사용하지 않고
            /// g_board[y][x].isWall 을 사용해서
            /// 만약 TRUE 라면 벽 색을 회색으로 입히고
            /// 만약 g_board[y][x].fix 가 TRUE 라면
            /// 고정된 블럭이니 블럭의 색상을 g_board[y][x] 에 입힌다
            if (g_board[y][x].isWall) {
                HBRUSH myBrush = CreateSolidBrush(RGB(150, 150, 150));
                /// os 가 들고있는 브러쉬를 myBrush 로 바꿔버림
                HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);

                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);

                /// 다 쓴 브러쉬를 교환하고
                SelectObject(hdc, osBrush);
                /// 정리
                DeleteObject(myBrush);
            }
            else if (g_board[y][x].isFixed) {
                /// 고정된 블럭의 rgb 값으로 g_board[y][x] 좌표를 채운다
                HBRUSH myBrush = CreateSolidBrush(
                    RGB(g_board[y][x].r,
                        g_board[y][x].g,
                        g_board[y][x].b));
                HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
                SelectObject(hdc, osBrush);
                DeleteObject(myBrush);
            }
            /// 빈 칸 그리기(흰색)
            else {
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
            }
        }
    }
}


/// 블럭이 내려와야함
/// 근데 이미 그려진 영역 안에서만 블럭이 내려와야하고 
/// 한번 내려올 때는 30px 씩 내려옴
/// 먼저 내려올 블럭이 생성될 위치는 220px 또는 250px 둘 중 하나의 위치에(왼쪽 위 기준) 블럭 생성
/// 우선은 그냥 네모난 블럭만 생성해서 내려오게 해봄
void DrawDownBlock(HWND hWnd, HDC hdc) {
    /// 근데 타이머로 움직여야하나?
    /// 정해진 시간마다 움직이게 하려면 타이머 말고 다른게 있을까..
    
}

void calcScore(int count) {
    if (count == 1) {
        currentScore += 100;
    }
    else if (count == 2) {
        currentScore += 300;
    }
    else if (count == 3) {
        currentScore += 500;
    }
    else {
        currentScore += 800;
    }
}

/// 스코어 보드의 위치는 10,10 에서 그려지기 시작함
const int SCORE_ORIGIN_X = 70;
const int SCORE_ORIGIN_Y = 10;

void DrawScoreBoard(HWND hWnd, HDC hdc) {
    Rectangle(hdc, SCORE_ORIGIN_X, SCORE_ORIGIN_Y, SCORE_ORIGIN_X + (BLOCK_SIZE * BOARD_W), SCORE_ORIGIN_Y + 50);
    /// Rect 안에 현재 점수를 넣고 점수가 들어올 때 마다 계속 갱신해야함
    /// Rect 안에 현재 점수를 출력시킴
    WCHAR buf[16] = { 0, };
    wsprintfW(buf, L"현재 점수");
    TextOut(hdc, (SCORE_ORIGIN_X + (BLOCK_SIZE * BOARD_W)) / 2, SCORE_ORIGIN_Y - 7, buf, lstrlenW(buf));

    memset(buf, 0x00, 0);

    /// 현재 점수를 계속 갱신 해야함
    /// 원래 점수에 계속 + 

    wsprintfW(buf, L"%d", currentScore);
    TextOut(hdc, (SCORE_ORIGIN_X + (BLOCK_SIZE * BOARD_W)) / 2 + 30, SCORE_ORIGIN_Y + 15, buf, lstrlenW(buf));

}

/// 최고점수 영역 그리기
void DrawHighScoreBoard(HDC hdc) {
    /// 최고 점수 영역은
    /// 다음 블럭 영역의 바로 아래에 30 px 떨어져서 존재한다
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 60 + 180 + 30;
    int right = left + 180;
    int bottom = top + 120;
    Rectangle(hdc, left, top, right, bottom);
    /// 화면에 "최고 점수" 출력
    WCHAR buf[16] = { 0, };
    wsprintfW(buf, L"최고 점수");
    TextOut(hdc, left + 56, top - 7, buf, lstrlenW(buf));
}

/// 다음 블럭 영역 그리기
void DrawNextBlockArea(HDC hdc) {
    /// 다음 블럭 영역은 오른쪽 맨 위에 크기는 약 300 x 200
    /// 시작점은 어디에 위치해야하는가
    /// left   : 2 * SCORE_ORIGIN_X + BLOCK * BOARD_W
    /// top    : SCORE_ORIGIN_Y
    /// right  : left + 300
    /// bottom : top + 180 -> 테트로미노 중 가장 긴 블럭이 I 인데 이걸 세우면
    ///          총 120 의 세로 길이를 가지게 된다
    ///          여유 공간을 고려하여 위 아래 여유롭게 30 30 공간을 주면 좋을듯
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 60;
    int right = left + 180;
    int bottom = top + 160;
    Rectangle(hdc, left, top, right, bottom);
    /*
     WCHAR buf[32] = { 0, };
    wsprintfW(buf, L"KEYDOWN: %d", wParam);
    HDC hdc = GetDC(hWnd);
    TextOut(hdc, 300, 10, buf, lstrlenW(buf));

    ReleaseDC(hWnd, hdc);
    */
    /// 화면에 "다음 블럭" 출력
    WCHAR buf[16] = { 0, };
    wsprintfW(buf, L"다음 블럭");
    TextOut(hdc, left + 56, top - 7, buf, lstrlenW(buf));

    /// nextPiece 그리기
    HBRUSH myBrush = CreateSolidBrush(RGB(
        nextPiece.r, nextPiece.g, nextPiece.b
    ));
    HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);

    /// 영역 안쪽에 여유를 주고 그리기
    int paddingX = left + 30;
    int paddingY = top + 30;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (TETROMINO[nextPiece.type][nextPiece.rot][y][x] == 1) {
                int px = paddingX + x * BLOCK_SIZE;
                int py = paddingY + y * BLOCK_SIZE;
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
            }
        }
    }

    SelectObject(hdc, osBrush);
    DeleteObject(myBrush);
}

/// 저장을 하기 위해서는 Piece 타입의 전역변수로 선언해두고
Piece saveBlock;
/// BOOL 타입의 전역변수 하나 선언해두기
/// 현재 저장된 블럭이 있는지
/// 만약 저장된 블럭이 있다면 추가적으로 저장하지 못하게 만들어야함
/// 처음 시작했을 때 에는 FALSE 로 두어서 저장된 블럭이 없다는것을 알림
BOOL hasSaveBlock = FALSE;

int blockToSave[4][4];

void LoadSavePiece() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            /// 저장한 블럭을 화면에 노출시키기 위해 저장한 블럭의 정보들로 덮어쓰기
            blockToSave[y][x] = TETROMINO[saveBlock.type][saveBlock.rot][y][x];
        }
    }
}

/// 현재 블럭을 저장하고 나중에 꺼내 쓸 수 있게 해주는 영역
/// 블럭이 생성되어서 내려오는 도중에 특정 키를 누르면
/// 내려오고 있는 블럭을 저장하고
/// 나중에 꺼내쓸 수 있음
/// 저장 공간은 한개
/// 이미 저장이 되어있다면 사용 후 저장할 수 있음
/// saveBlockArea는 다음 블럭 영역 바로 아래에 둘거임
/// 
/// 그리고 저장된 블럭이 이미 있다면 현재 블럭이랑 스왑하는 형식으로 해야할듯
void DrawSaveBlockArea(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 60 + 180 + 30;
    int right = left + 180;
    int bottom = top + 180;
    Rectangle(hdc, left, top, right, bottom);
    WCHAR str[32] = { 0, };
    wsprintf(str, L"저장한 블럭");
    TextOut(hdc, left + 50, top - 7, str, lstrlenW(str));

    /// 맨 마지막에 saveBlock 을 그린다
    /// Piece 구조체에 존재하는 blockX, blockY 를 사용할수는 없다
    /// blockX, blockY 는 게임보드의 좌표를 따르기 때문
    
    /// 저장된 블럭이 있다면 return;
    if (!hasSaveBlock) {
        ReleaseDC(hWnd, hdc);
        return;
    }
    LoadSavePiece();
    
    /// 새로운 좌표를 추가해야한다
    /// 우선 브러쉬를 만들고
    HBRUSH myBrush = CreateSolidBrush(RGB(
        saveBlock.r, saveBlock.g, saveBlock.b
    )); /// 저장된 블럭의 색깔
    HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (blockToSave[y][x] == 1) {
                /// 화면에 출력하기 위한 x, y 좌표
                /// fx = fix x => ORIGIN_X + BLOCK * BOARD_W + 30 + 90
                /// fy = fix y => ORIGIN_Y + 50 + 400
                int fx = left + 30 + x * BLOCK_SIZE;
                int fy = top + 30 + y * BLOCK_SIZE;

                Rectangle(hdc, fx, fy, fx + BLOCK_SIZE, fy + BLOCK_SIZE);
            }
        }
    }

    SelectObject(hdc, osBrush);
    DeleteObject(myBrush);
    /// 위에서 블럭을 저장했으니 TRUE 로 바꿔서 추가적으로 저장하지 못하게 해야함
    hasSaveBlock = TRUE;

}

/// 게임오버를 알리는 기능 구현
/// 게임 오버의 조건은 블럭이 생성되는 영역 까지 fix 된 블럭들이 쌓여 닿았을 시
/// 단순히 새로 스폰된 블럭이 fix 블럭과 닿는다면 게임오버시킨다
/// bool 타입으로 WM_TIMER 의 SpawnBlock() 위에서 GameOver 인지 검사한다
bool isGameOver() {
    /// 게임 오버 검사는 새로 생성되는 블럭과 닿는다면 게임 오버
    /// 하지만 지금 여기서 그렇게 하면 논리적 오류가 생긴다
    /// 게임 오버를 확인하기 위해서는 블럭을 먼저 생성해야하는데
    /// 블럭을 생성하기 전에 게임오버를 검사하기 때문에 논리적 오류가 생겨 정상작동을 안한다
    /// 그렇다면 블럭을 SpawnBlock() 한다음에 fix 된 블럭과 바로 닿는지 확인을 한다
    /// 만약 스폰된 블럭이 한칸이라도 움직이면 gameover 가 아니고
    /// 한칸이라도 움직이지 않고 바로 고정이 된다면 gameover 해야함
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            /// currentBlock[y][x] 가 0 이면 4 * 4 배열 안에서 빈 공간임
            if (currentBlock[y][x] == 0) {
                /// 빈공간은 필요 없으니까 건너뛰기
                continue;
            }
            /// 빈 공간이 아닌 구역 = 실제로 Piece 가 존재하는 영역
            /// 여기서 겹침 확인을 한다
            
            /// nx, ny 는 새로 스폰된 블럭이 게임 보드에서 어느 좌표에 위치해있는지 확인하는 변수
            int nx = currentPiece.blockX + x;
            int ny = currentPiece.blockY + y;


            /// currentBlock은 게임보드에서 nx, ny 좌표에 존재함
            /// 근데 nx, ny 좌표에 고정 블럭이 존재한다면 게임 오버
            if (g_board[ny][nx].isFixed) {
                return TRUE;
            }
        }
    }
    /// 아무 문제 없음, 게임 계속함
    return FALSE;
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
                g_board[fy][fx].isFixed = TRUE;       /// 고정 시키기
                g_board[fy][fx].r = currentPiece.r;
                g_board[fy][fx].g = currentPiece.g;
                g_board[fy][fx].b = currentPiece.b;

            }
        }
    }
    hasUsedSaveOrLoad = FALSE;
}   

/// 현재 속도를 보여주는 영역
void ShowCurrentSecond(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    int left = 800;
    int top = 100;
    int right = 1000;
    int bottom = 200;

    Rectangle(hdc, left, top, right, bottom);
    WCHAR str[32] = { 0, };
    wsprintf(str, L"현재 속도 : %d    ", second);
    TextOut(hdc, 850, 150, str, lstrlenW(str));
    
}

void LoadCurrentPiece() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            /// 내 테트로미노 구조는 [0~6:블럭 타입][0~3:ROT_0~270][y:비트맵][x:비트맵]
            /// currentBlock[y][x] 에 TETROMINO 가 가지고 있는 currentPiece.type, currentPiece.rot 
            /// 에 해당하는 비트맵 [y][x] 를 저장시킨다 (테트로미노가 어떻게 생겼는지, I, O, T, ...)
            currentBlock[y][x] = TETROMINO[currentPiece.type][currentPiece.rot][y][x];
        }
    }
}

/// 아래 함수는 화살표 DOWN 에 대한 충돌감지만 수행한다
/// 키보드 'A', 'S' 에 관한 충돌감지 함수도 제작해야한다
/// 
/// 벽, fix 된 블럭 둘 다 끼이는 상황
/// 내려오는 블럭이 끼이지 않게
/// 블럭을 돌리기 전에 돌아갈 수 있는지를 확인부터 하고 안된다면 돌리지 않는다
/// 반대도 마찬가지로 검사부터 하고 안된다면 돌리지 않는다

/// 여기서 A, S 에 대한 충돌감지 코드 작성
/// A S 중 어떤걸 파라미터로 가져오는지 알아야함
bool canMoveCurrentBlockAS(Rotation nextRot) {
    /// currnetBlock 의 y, x 좌표를 이용해서
    /// 키보드 A, S 가 눌렸을 때 돌아가는 과정에서
    /// 벽이나 블럭이 존재하는지 확인 한 후에
    /// 만약 존재한다면 돌아가지 못하게, 존재하지 않는다면 돌아가게 만들어야한다
    
    int nx = 0;
    int ny = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {

            /// 회전 후 모양에서 블럭의 값이 1 인 부분만 검사한다
            if (TETROMINO[currentPiece.type][nextRot][y][x] == 0) {
                continue;
            }

            /// 현재 블럭이 존재하는 좌표와
            /// 게임 보드에서의 존재하는 블럭의 좌표를 비교한다
            nx = currentPiece.blockX + x;
            ny = currentPiece.blockY + y;
            
            /// 보드 밖으로 나가면 회전 불가
            if (nx < 0 || nx >= BOARD_W || ny < 0 || ny >= BOARD_H) {
                return FALSE;
            }

            /// 벽이나 고정 블럭이랑 겹치면 회전 불가
            if (g_board[ny][nx].isWall || g_board[ny][nx].isFixed) {
                return FALSE;
            }

            /// nx, ny 를 구한 뒤 
            /// 게임 보드에서 nx, ny 좌표와 겹치는 물체가 있다면 FALSE 반환
            
            
        }
    }

    return TRUE;
}

/// 벽에 닿았을 때 넘어가지 않게 해줌
/// 여기서 내려오는 블럭이 게임 보드에 끼이지 않게 수정해야함
BOOL canMoveCurrentBlock(int dx, int dy) {
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
            if (g_board[ny][nx].isWall || g_board[ny][nx].isFixed) {
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
/// 이 함수 내부에서 점수 갱신도 해야함
void FullLine(HWND hWnd) {
    int count = 0;
    /// g_board 의 모든 라인을 조사해서
    /// y 배열의 맨 아랫줄을 빼고
    /// 루프의 방향은 맨 아래에서부터 맨 위 까지 검사
    /// 그러므로 for 문의 시작은 BOARD_H - 1 부터 > 0 까지
    for (int y = BOARD_H - 1; y > 0; y--) {
        /// isFull 은 해당 y 배열이 가득 찼는지 확인하기 위해 존재하는 변수
        BOOL isFull = TRUE;
        for (int x = 1; x < BOARD_W - 1; x++) {
            if (g_board[y][x].isFixed == FALSE) {
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
            count++;
            /// 라인이 지워지고 윗줄이 내려왔으므로
            /// 지금 막 내려온 y 층을 검사하기 위해 y 값을 증가시켜 한번 더 검사한다
            /// y++ 하고 isFull 이 FALSE 면 알아서 스킵하게 된다
            y++;
        }
    }
    if (count > 0) {
        calcScore(count);
    }
}



/// 블럭을 스폰시킴
void SpawnBlock() {
    /// 현재 아래의 로직은 currentPiece 를 정의만함
    /// 새로 추가해야할것
    /// nextPiece 정의
    
    /// 아직 nextPiece 가 없다면
    if (!hasNextPiece) {
        /// nextPiece 에 테트로미노 타입 지정
        nextPiece.type = (TetrominiType)(rand() % 7);
        nextPiece.rot = ROT_0;

        /// 그리고 색상 지정
        switch (nextPiece.type) {
        case I:
            nextPiece.r = 0;
            nextPiece.g = 255;
            nextPiece.b = 255;
            break;
        case O:
            nextPiece.r = 255;
            nextPiece.g = 255;
            nextPiece.b = 0;
            break;
        case T:
            nextPiece.r = 128;
            nextPiece.g = 0;
            nextPiece.b = 128;
            break;
        case S:
            nextPiece.r = 0;
            nextPiece.g = 255;
            nextPiece.b = 0;
            break;
        case Z:
            nextPiece.r = 255;
            nextPiece.g = 0;
            nextPiece.b = 255;
            break;
        case J:
            nextPiece.r = 0;
            nextPiece.g = 0;
            nextPiece.b = 255;
            break;
        case L:
            nextPiece.r = 255;
            nextPiece.g = 165;
            nextPiece.b = 0;
            break;
        }
        /// nextBlock 초기화 끝났으니까
        /// hasNextPiece TRUE
        hasNextPiece = TRUE;
    }
    /// 이제 currentBlock 에 nextPiece 를 저장함
    /// 저장하는 이유는 nextPiece 가 다음 턴에 출력되게 하기 위해
    currentPiece = nextPiece;
    /// 초기 위치를 가지고 있는 멤버는 blockX, blockY
    /// 초기 블럭의 스폰 위치는 y : 70px, x : 220px 또는 250px
    currentPiece.blockX = BOARD_W / 2 - 2;
    currentPiece.blockY = 0;
    /// 그리고 또 다음 턴을 위해 새로운 nextPiece 를 뽑아둔다
    nextPiece.type = (TetrominiType)(rand() % 7);
    nextPiece.rot = ROT_0;
    /// 그리고 색상 지정
    switch (nextPiece.type) {
    case I:
        nextPiece.r = 0;
        nextPiece.g = 255;
        nextPiece.b = 255;
        break;
    case O:
        nextPiece.r = 255;
        nextPiece.g = 255;
        nextPiece.b = 0;
        break;
    case T:
        nextPiece.r = 128;
        nextPiece.g = 0;
        nextPiece.b = 128;
        break;
    case S:
        nextPiece.r = 0;
        nextPiece.g = 255;
        nextPiece.b = 0;
        break;
    case Z:
        nextPiece.r = 255;
        nextPiece.g = 0;
        nextPiece.b = 255;
        break;
    case J:
        nextPiece.r = 0;
        nextPiece.g = 0;
        nextPiece.b = 255;
        break;
    case L:
        nextPiece.r = 255;
        nextPiece.g = 165;
        nextPiece.b = 0;
        break;
    }

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

    /// 여기서 게임오버 확인함
    /// 새로 스폰된 블럭이 움직이지 않은 상태에서
    /// 현재 fix 된 블럭과 좌표가 겹치는게 하나라도 있으면 게임 오버시킴

    LoadCurrentPiece();
    /// 먼저 LoadCurrentPiece() 한 다음에 바로 닿는게 있다면 게임종료
    
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
                int px = ORIGIN_X + (currentPiece.blockX + x) * BLOCK_SIZE;
                int py = ORIGIN_Y + (currentPiece.blockY + y) * BLOCK_SIZE;
                /// 블럭 그리기
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
            }
        }
    }
    SelectObject(hdc, osBrush);
    DeleteObject(myBrush);
}

/// 키 설명 표시 영역
void DrawKeyGuide(HDC hdc)
{
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 60 + 180 + 30 + 180 + 10; /// 저장영역 아래에서 10px 띄움

    WCHAR guide1[32], guide2[32], guide3[32], guide4[32], guide5[32], guide6[32], guide7[32];
    wsprintf(guide1, L"← / → : 좌우 이동");
    wsprintf(guide2, L"↓ : 소프트 드롭");
    wsprintf(guide3, L"↑ : 하드 드롭");
    wsprintf(guide4, L"A : 반시계 회전");
    wsprintf(guide5, L"S : 시계 회전");
    wsprintf(guide6, L"D 블럭 저장, F 블럭 불러오기");
    wsprintf(guide7, L"ESC 게임 일시정지, 게임 재개");

    TextOut(hdc, left, top, guide1, lstrlen(guide1));
    TextOut(hdc, left, top + 20, guide2, lstrlen(guide2));
    TextOut(hdc, left, top + 40, guide3, lstrlen(guide3));
    TextOut(hdc, left, top + 60, guide4, lstrlen(guide4));
    TextOut(hdc, left, top + 80, guide5, lstrlen(guide5));
    TextOut(hdc, left, top + 100, guide6, lstrlen(guide6));
    TextOut(hdc, left, top + 120, guide7, lstrlen(guide7));

}

/// 화면에 일시정지 표시
void DrawPause(HDC hdc) {
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 10;

    WCHAR showPause[32];
    wsprintf(showPause, L"일시 정지");
    TextOut(hdc, left, top, showPause, lstrlen(showPause));
}

/// 일시정지 가리기
void CoverPause(HDC hdc) {
    int left = 30 + SCORE_ORIGIN_X + BLOCK_SIZE * BOARD_W;
    int top = SCORE_ORIGIN_Y + 10;

    WCHAR showPause[32];
    wsprintf(showPause, L"                        ");
    TextOut(hdc, left, top, showPause, lstrlen(showPause));
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
/// 구조물의 이름은 테트로미노 라고 정의한다
///  'I', 'O', 'T', 'S', 'Z', 'J', 'L' - block 이 존재
/// I-block, O-block, T-block, S-block, Z-block, J-block, L-block
///
/// 
/// 
/// 추가해야하는것
/// 점수가 높아지면 높아질수록 블럭들이 점점 빠르게 내려오도록 해야함
/// 이제 점수를 추가해야함
/// 싱글라인 클리어     : 100 점
/// 더블라인 클리어     : 300 점
/// 트리플라인 클리어   : 500 점
/// 테트리스라인 클리어 : 800 점
/// 라인 클리어 말고도 많은 배점이 있지만 우선 이것들부터 적용하고 시작

/// 키보드 입력
///  - 키보드 화살표 왼쪽, 오른쪽 : 블럭 좌 우로 움직이기
///  - 키보드 화살표 아래쪽 : 블럭 빠르게 내리기(소프트 드롭)
///  - 키보드 화살표 위 쪽 : 블럭 바로 내리기 (하드 드롭)
///  - 키보드 A : 블럭 반시계 방향으로 돌리기 : 
///  - 키보드 S : 블럭 시계 방향으로 돌리기
///  - 키보드 F : 저장한 블럭 꺼내쓰기
///  - 키보드 D : 블럭 저장하기
///  - 키보드 esc : 일시정지 + 메뉴 노출
/// 
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
            
            /// 'A'와 'S' KEYDOWN 이 왔을 때
            /// CollisionDetectAS() 함수가 호출되어야한다
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
                /// 
                /// 아래의 변수들은 무작정 블럭을 돌리기만 한다
                /// 이 변수들을 CollisionDetectAS 로 옮겨서 검사를 해야함
                
                /// 만약 A S 로 현재 내려오는 블럭을 움직이는게 성공했을 시
                /// A S 의 방향으로 현재 내려오는 블럭을 움직이게 한다
                
                int a = ((currentPiece.rot + 3) % 4); /// 0 ~ 3 의 enum 존재
                Rotation nextRot = (Rotation)a;
                if (canMoveCurrentBlockAS(nextRot)) {
                    currentPiece.rot = nextRot;
                    LoadCurrentPiece();
                    InvalidateRect(hWnd, NULL, FALSE);
                }

            }
            break;
            case 'S':
            {
                /// currentPiece 를 시계방향으로 돌리기
                /// 마찬가지로 CollisionDetectAS 에서
                /// S 방향으로 블럭 돌리는게 가능하다면 돌리고
                /// 안된다면 안돌린다
                
                int a = ((currentPiece.rot + 1) % 4);
                Rotation nextRot = (Rotation)a;
                if (canMoveCurrentBlockAS(nextRot)) {
                    currentPiece.rot = nextRot;
                    LoadCurrentPiece();
                    InvalidateRect(hWnd, NULL, FALSE);
                }
                
            }
            break;
            case 'D':
            {
                /// 해당 턴에 D F 를 둘 중 하나라도 사용했으면 break
                if (hasUsedSaveOrLoad) {
                    break;
                }
                /// 저장된 블럭이 없다면 현재 블럭을 저장시키고
                /// 새로운 블럭을 생성시킨다
                if (!hasSaveBlock) {
                    /// 저장된 블럭이 없는 상태에서 D 를 누르면 현재 내려오는 블럭이 저장되고
                    /// 저장된 블럭이 있는 상태에서 D 를 누르면 현재 블럭이 저장된 블럭으로 바뀐다
                    /// 동시에 현재 내려오던 블럭은 그냥 없애버린다
                    /// 만약 저장된 블럭이 없는 상태에서 D 를 눌렀다면
                    /// currentPiece 를 saveBlock 에 저장시키고 바로 새로운 블럭을 내려보낸다

                    /// saveBlock 에 currentPiece 를 저장시키고
                    /// 좌표를 저장하면 저장 당시의 좌표에 불러와짐
                    ///saveBlock.blockX = currentPiece.blockX;
                    ///saveBlock.blockY = currentPiece.blockY;
                    saveBlock.r = currentPiece.r;
                    saveBlock.g = currentPiece.g;
                    saveBlock.b = currentPiece.b;
                    saveBlock.type = currentPiece.type;
                    saveBlock.rot = ROT_0;
                    
                    /// currentPiece 를 초기화 시켜야함
                    currentPiece.blockX = NULL;
                    currentPiece.blockY = NULL;
                    currentPiece.r = NULL;
                    currentPiece.g = NULL;
                    currentPiece.b = NULL;
                    currentPiece.type = (TetrominiType)NULL;
                    currentPiece.rot = ROT_0;
                    hasSaveBlock = TRUE;
                    hasUsedSaveOrLoad = TRUE;
                    /// 초기화 해서 currentPiece 를 없애버렸으니 바로 새로운 블럭을 생성시킨다
                    SpawnBlock();
                }
            }
            break;
            /// 저장된 블럭을 불러올 때
            /// 현재 내려오고 있는 블럭은 그냥 삭제시켜버림 
            case 'F':
            {
                /// 해당 턴에 D F 를 둘 중 하나라도 사용했으면 break
                if (hasUsedSaveOrLoad) {
                    break;
                }
                if (!hasSaveBlock) {
                    break;
                }
                /// 저장된 블럭을 가지고 있을 때 에만 작동하게
                else {
                    /// 이미 내려고오 있는 currentPiece 를 덮어쓴다
                    currentPiece = saveBlock;
                    /// 그리고 처음부터 내려오게
                    currentPiece.blockX = BOARD_W / 2 - 2;
                    currentPiece.blockY = 0;
                    /// 현재 저장된 블럭을 불러와서 사용 했으니
                    /// 저장된 블럭이 없다는것을 알려줘야함
                    LoadCurrentPiece();
                    /// 불러오기를 했으니까 저장된 블럭은 없음
                    /// FALSE 로 바꿔줌으로써 D 진입 가능하게 바꿈
                    hasSaveBlock = FALSE;
                    hasUsedSaveOrLoad = TRUE;
                }
            }
            break;
            case VK_LEFT:
            {
                /// 만약 게임보드에서 -1 만큼 움직이는게 성공했을 시
                /// blockX-- 해주고 다시 그림
                if (canMoveCurrentBlock(-1, 0)) {
                    currentPiece.blockX--;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_RIGHT:
            {
                /// 만약 게임보드에서 +1 만큼 움직이는게 성공했을 시
                /// blockX++ 해주고 다시 그림
                if (canMoveCurrentBlock(1, 0)) {
                    currentPiece.blockX++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_DOWN:
            {
                /// 만약 게임 보드에서 y 가 1만큼 증가하는게 성공했을 시
                /// blockY++ 해주고 다시 그림
                if (canMoveCurrentBlock(0, 1)) {
                    currentPiece.blockY++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_UP:
            {
                /// 하드드롭
                /// fix 또는 isWall 이 나올 때 까지 반복문으로 CollisionDetection 호출
                while (canMoveCurrentBlock(0, 1)) {
                    currentPiece.blockY++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
            case VK_ESCAPE:
            {
                HDC hdc = GetDC(hWnd);
                if (!isPaused) {
                    /// isPaused 가 FALSE 라면 게임을 멈춰주고 TRUE 로 바꿔줌
                    KillTimer(hWnd, 1);
                    isPaused = TRUE;
                    /// isPaused 가 TRUE 일 때 DrawPause 를 그리지 않고
                    /// 항상 DrawPause 를 그린 뒤 isPaused 가 FALSE 일 때 CoverPause 를 호출해
                    DrawPause(hdc);
                }
                else if (isPaused) {
                    /// isPaused 가 TRUE 라면 게임을 재개하고 FALSE 로 바꿔줌
                    SetTimer(hWnd, 1, second, NULL);
                    isPaused = FALSE;
                    CoverPause(hdc);
                }
                ReleaseDC(hWnd, hdc);
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
        /// 0.1 초 마다 호출되는 타이머 ID = 1
        /// 점수가 올라갈 때 마다 블럭 생성되는 시간이 점점 줄어들면서
        /// 블럭이 내려오는 속도도 점점 빨라지게
        /// 그럼 초 를 담는 변수를 전역으로 선언해둔다
        SetTimer(hWnd, 1, second, NULL);
        
    }
    break;
    case WM_TIMER:
    {
        /// 타이머에서 블럭이 아래 벽에 닿았을 때 멈춰야함
        /// FixBlock(), SpawnBlock() 해줘야함
        if (wParam == 1) {
            /// 초기 속도를 정한 뒤 
            /// 만약 초기 속도랑 현재 속도랑 다르다면
            /// 그 때 타이머를 죽이고 새로 만들어야함
            /// 이미 second 가 있으니까
            /// 1000점 쌓일 때 마다 속도가 점점 빨라지도록 설계
            if (currentScore >= nextSpeedUpScore)
            {
                /// 속도 증가
                second -= 100;
                
                if (second < 100) {
                    second = 100;   // 최소 100ms 까지만 줄이기
                }
                /// 타이머를 죽이고 다시 생성
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, second, NULL);

                /// 다음 목표 점수 갱신
                nextSpeedUpScore += 1000;
            }
            /// 아래로 1칸 내려가는게 성공했을 시
            if (canMoveCurrentBlock(0, 1)) {
                /// 아래로 내려버림
                currentPiece.blockY++;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            /// 아래로 1칸 내려가는게 실패했을 시
            else {
                /// 블럭을 그 위치에 고정시키고
                FixBlock();
                /// 고정시킨 다음에 x 배열이 가득 찼다면 해당 배열 빨간색으로 바꾸기
                FullLine(hWnd);
                /// 새로운 블럭을 생성
                SpawnBlock();
                InvalidateRect(hWnd, NULL, FALSE);

                /// 여기서 게임 오버 검사
                /// 게임 오버하면 타이머를 멈추고 메시지 박스를 띄운다
                if (isGameOver()) {
                    KillTimer(hWnd, 1);
                    WCHAR showScore[32] = { 0, };
                    wsprintf(showScore, L"점수 : %d", currentScore);
                    MessageBox(hWnd, showScore, L"게임 오버", MB_OK);
                }
            }
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
            DrawNextBlockArea(hdc);
            DrawHighScoreBoard(hdc);
            DrawCurrentBlock(hWnd, hdc);
            DrawSaveBlockArea(hWnd);
            ///ShowCurrentSecond(hWnd);
            DrawKeyGuide(hdc);
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
