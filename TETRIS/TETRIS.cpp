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
    int type;   /// TETROMINO (0 ~ 6)
    int rot;    /// ROT_ 0 ~ 270 (0, 90, 180, 270)
    int x, y;   /// 보드에서의 좌표(왼쪽 위 기준)
};

/// !!! 게임 보드 영역의 좌표는 무조건 왼쪽 위 부터 오른쪽 아래로 갈 수록 커짐
/// g_board[BOARD_H][BOARD_W] 로 정의 한 이유
/// 행 -> 열 순으로 위에서부터 아래로 행을 쌓아간다
int g_board[BOARD_H][BOARD_W];
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
            /// x 가 0 이면 왼쪽 벽
            if (x == 0) {
                g_board[y][x] = 1;  /// y행의 x열을 1로(왼쪽 벽)
            }
            /// x 가 BOARD_W - 1 이면(인덱스 기준이라 -1 해야함) 오른쪽 벽
            else if (x == BOARD_W - 1) {
                g_board[y][x] = 1;  /// y행의 x열을 1로(오른쪽 벽)
            }
            /// y 가 BOARD_H - 1 이면(인덱스 기준이라 -1 해야함) 아래 벽
            else if (y == BOARD_H - 1) {
                g_board[y][x] = 1;  /// y행의 모든 x 를 1로(아래 벽)
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
const int ORIGIN_X = 70;    /// 이 게임 보드의 시작 위치는 50, 50(왼쪽 위 기준)
const int ORIGIN_Y = 70;
void DrawGameBoard(HWND hWnd, HDC hdc) {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            /// cell 에 InitBoard() 한 값을 넣고
            /// 1, 0 으로 나눠서 그릴지 말지 판단
            /// 1이면 그림
            /// 2이면 안그림
            int cell = g_board[y][x];   /// 초기화 한 보드 배열의 y,x 인덱스의 값을 넣음
            int px = ORIGIN_X + x * BLOCK;  /// 보드의 x 번째 칸이 화면에서 몇 픽셀에 있어야하는지
            int py = ORIGIN_Y + y * BLOCK;  /// 보드의 y 번째 칸이 화면에서 몇 필셀에 있어야하는지
               
            /// cell 이 1 이면 그려야함(벽)
            if (cell == 1) {
                HBRUSH myBrush = CreateSolidBrush(RGB(150, 150, 150));
                /// os 가 들고있는 브러쉬를 myBrush 로 바꿔버림
                HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);
                
                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);

                /// 다 쓴 브러쉬를 교환하고
                SelectObject(hdc, osBrush);
                /// 정리
                DeleteObject(myBrush);
            }
            /// cell 이 0 이면 안그림
            else {
                Rectangle(hdc, px, py, px + BLOCK, py + BLOCK);
            }
        }
    }
}

/// 현재 내가 조작하고 있는 블럭 종류
/// SpawnBlock 할 때 값을 할당해줄거임
int currentBlockType;
int currentBlock[4][4];   /// 현재 조작중인 블럭 4 x 4 크기
int blockX, blockY;       /// 블럭 x,y 좌표 (currentBlock)
/// 기본 ROT 는 0 으로 시작, 키보드 입력을 받아서 회전시키면 + - 해야함
int currentRot;           /// 블럭 회전 반경, 이걸로 인덱스 접근할거임 (0~3 범위)


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

}

/// 다음에 나올 블럭을 미리 보여줌
void ShowNextBlock() {

}

/// 벽에 닿았을 때 넘어가지 않게 해줌
void CollisionDetection() {
    /// 왼쪽 벽, 오른쪽 벽, 아래 벽 에 닿았을 때 넘어가지 않게 해줌
    /// 겹침확인은 IntersectRect() 로 했었는데
    /// 이건 그거 말고
    /// 내가 그려놓은 게임보드를 기준으로 해보자
    /// 우선 내가 색을 다르게 칠 해둔 영역 밖으로 나가면 안되는것이니
    /// 이 CollisionDetection 함수는 내가 KEYDONW 할때 마다 호출시켜야함
    /// 호출해서 blockX, blockY 값을 항상 비교
    /// 그리고 시간이 지나면 어차피 blockY 를 비교해야하기 때문에 타이머에도 넣어야하나?
    /// g_board[][] 와 비교하는건 필수
    
    for (int y = 0; y < BOARD_H; y++) {
        /// 만약 해당 인덱스가 1 이면 벽이고
        /// 그 상황에서 currentBlock 이 벽 쪽 으로 KEYDOWN 을 눌렀을 때
        /// 못가게 막아야함
        /// blockX가 존재해야하는 인덱스를 확인해야함
         
        /// 현재 상황은 블럭이 왼쪽, 오른쪽 벽에 닿았을 때
        if (blockX == -1) {  /// blockX 값이 -1 이면 맨 왼쪽
            blockX++;     /// 아무리 나가도 다시 0의 위치로 돌아옴
        }
        /// 오른쪽 검사는 좀 까다로움
        /// 블럭의 오른쪽 기준으로 검사해야함
        else if (blockX == BOARD_W - 1) {

        }
        /// 현재 상황은 블럭이 아래쪽 벽에 닿았을 때
    }
}

/// 블럭을 스폰시킴
void SpawnBlock() {
    srand(time(NULL));
    /// 초기 블럭의 스폰 위치는 y : 70px, x : 220px 또는 250px
    /// 랜덤함수를 이용해서 0~6 사이의 값을 하나 받음
    /// 그 값에 해당하는 테트로미노를 하나 생성함
    /// 근데 그걸 어케하냐?
    /// 우선 rand 함수로 0~6 중에 하나 고름
    currentBlockType = rand() % 7;  /// 0~6 사이 값 나옴, 스폰시킬 블럭의 타입이 정해짐
    
    /// 그리고 블럭의 초기 위치도 정해줘야함
    /// 초기 위치를 가지고 있는 변수는 blockX, blockY
    blockX = BOARD_W / 2 - 2;
    blockY = 0;
    /// 항상 테트로미노의 [N][0] 은 ROT_0 블럭(기본) 이다
    /// 그래서 처음 블럭 스폰할 때에는 ROT_0 의 블럭을 스폰시켜야한다
    /// 그리고 선택되어진 블럭을 currentBlock 에 복사함
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            /// 내 테트로미노 구조는 [0~6:블럭 타입][0~3:ROT_0~270][y:비트맵][x:비트맵]
            currentBlock[y][x] = TETROMINO[currentBlockType][currentRot][y][x];
        }
    }
}

/// 화면에 스폰시킨 블럭 그리기
void DrawCurrentBlock(HWND hWnd, HDC hdc) {
    HBRUSH myBrush = CreateSolidBrush(RGB(255, 0, 0));  // 빨간 블럭
    HBRUSH osBrush = (HBRUSH)SelectObject(hdc, myBrush);
    /// 여기서 x, y 는 블럭배열 안에서 1인 인덱스의 위치를 찾기 위해
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x] == 1) {  /// 1이면 해당 영역에 블럭이 존재
                /// blockX, blockY : currentBlock의 위치를 보드 기준으로 나타내는 좌표
                int px = ORIGIN_X + (blockX + x) * BLOCK;
                int py = ORIGIN_Y + (blockY + y) * BLOCK;
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
///  - 키보드 A : 블럭 반시계 방향으로 돌리기
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
            case VK_LEFT:
            {
                blockX--;
                CollisionDetection();
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
            case VK_RIGHT:
            {
                blockX++;
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
            case VK_DOWN:
            {
                blockY++;
                InvalidateRect(hWnd, NULL, TRUE);
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
        if (wParam == 1) {
            blockY++;
            srand(time(NULL));
            InvalidateRect(hWnd, NULL, TRUE);
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
