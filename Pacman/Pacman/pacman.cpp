#include <iostream>
#include <array>
#include <windows.h>
#include <conio.h> 

using namespace std;

///
///
//		메인 분석 : 콘솔을 이용한 팩맨 게임으로 매 화면 새로 그려지는 그리드를 이용해 게임을 진행한다.
//					규칙은 기본 팩맨과 유사하며, 적과 과일 아이템이 구현되어 있지 않은 상태이다.
//					모든 점을 다 먹으면 게임이 클리어되며, 무한루프로 점수가 점점 올라가는 구조이다. 
//					노란색 점을 먹으면 무적 모드로 되며, 적을 먹을 수 있는 상태로 변한다. 이건 그냥 실험
///
///


// Keyboard ASCII codes
#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27

// Window handle
static HANDLE hStdout;
static CONSOLE_SCREEN_BUFFER_INFO csbi;
DWORD dummy;

class Pacman
{
public:
	bool mouthOpen = false;		// 팩맨 입모양
	bool godMode = false;		// 무적모드 (적을 잡아먹을 수 있는 상태)
	int godModeMaxMoves = 40;	// 무적모드 상태에서 이동할 수 있는 횟수
	int godModeMovesLeft = 40;	// 무적모드 이동 남은 횟수

								// Reset
	void Reset();
};

void Pacman::Reset()		// 팩맨 리셋
{
	mouthOpen = false;	// 입을 열었다 닫았다 하는 변수
	godMode = false;	// 무적 모드 On / Off
	godModeMaxMoves = 40;
	godModeMovesLeft = 40;
}

class Game
{
public:
	int lives = 3;		// 목숨
	int score = 0;		// 점수
	int dotsRemaining = 0;			// 남은 점 개수
	int teleporterLeft = 0;			// 왼쪽 텔레포트 지점
	int teleporterRight = 0;		// 오른쪽 텔레포트 지점
	int status = 0;
	int themeMusicPlayed = 0;		// 테마 곡이 플레이 되었는가
	bool exit = false;		// 게임 종료 변수

							// Reset
	void Reset();
};

void Game::Reset()			// 게임 리셋
{
	lives = 3;		// 목숨
	score = 0;		// 점수
	dotsRemaining = 0;		// 남은 점 개수
	teleporterLeft = 0;
	teleporterRight = 0;
	status = 0;
	themeMusicPlayed = 0;
}

class Grid			// 그리드 (게임 Status)
{
public:
	// Grid numbers			 
	static const int nothing = 0;			// 빈 공간
	static const int wall = 1;				// 벽
	static const int white_dot = 2;			// 먹는 점
	static const int yellow_dot = 3;		// 먹으면 무적 모드
	static const int blinky = 4;			// 적군 1
	static const int inky = 5;				// 적군 2
	static const int pinky = 6;				// 적군 3
	static const int clyde = 7;				// 적군 4
	static const int pacman = 8;			// 팩맨
	static const int teleport = 9;			// 텔레포트

											// Grid char equivalents			// 각 상태가 콘솔에서 표시될 문자열
	static const char nothing_char = ' ';
	static const char wall_char = '#';
	static const char white_dot_char = '.';
	static const char yellow_dot_char = '.';
	static const char blinky_char = 'o';
	static const char inky_char = 'o';
	static const char pinky_char = 'o';
	static const char clyde_char = 'o';
	static const char pacman_char = 'O';
	static const char pacman_mouth_open_char = 'C';

	// Grid colors
	static const int nothing_color = 0; // Black
	static const int wall_color = 1; // Blue
	static const int white_dot_color = 15; // White
	static const int yellow_dot_color = 14; // Yellow
	static const int blinky_color = 4; // 
	static const int inky_color = 5; // 
	static const int pinky_color = 6; // 
	static const int clyde_color = 7; // 
	static const int pacman_color = 14; // Yellow
	static const int pacman_flash_color = 15; // White
	static const int pacman_godmode_color = 13; // Pink

												// Actual grid
	array<int, 575> originalGrid, currentGrid;
	static const int columns = 23;		// 세로 23
	static const int rows = 25;			// 가로 25

										// Locations of characters
	int pacmanLocation = 0;		// 각 캐릭터의 그리드 상 위치
	int blinkyLocation = 0;
	int inkyLocation = 0;
	int pinkyLocation = 0;
	int clydeLocation = 0;
	int leftTeleporterLocation = -1;
	int rightTeleporterLocation = -1;

	// Dots count
	int dotsRemaining = 0;

	// Initialize grid
	Grid();

	// Flash screen when all dots are eaten
	void Flash();

	// Reset grid
	void Reset();

	// Draw grid
	void Draw(Game& game, Pacman& pacman);
};

void Grid::Reset()
{
	// Copy grid
	currentGrid = originalGrid = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1,
		1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 1,
		1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		9, 0, 0, 0, 0, 2, 0, 0, 1, 4, 5, 0, 6, 7, 1, 0, 0, 2, 0, 0, 0, 0, 9,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
		1, 3, 2, 2, 1, 2, 2, 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 1, 2, 2, 3, 1,
		1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1,
		1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};

	// Find out where characters are
	int dotsCounter = 0;

	for (int i = 0; i < originalGrid.size(); ++i) {		// 맵의 1칸씩 확인하며 그리드를 만든다.
		switch (originalGrid[i]) {
		case pacman:
			pacmanLocation = i;			// 팩맨 위치 획득
			break;
		case white_dot: case yellow_dot:		// 남은 점 개수
			dotsCounter++;
			break;
		case teleport:			// 텔레포트
			if (leftTeleporterLocation == -1) {
				leftTeleporterLocation = i;		// 왼쪽 텔레포트 위치 획득
			}
			else {
				rightTeleporterLocation = i;	// 오른쪽 텔레포트 위치 획득
			}

			break;
		case blinky:			// 적군 위치 획득
			blinkyLocation = i;
			break;
		case inky:
			inkyLocation = i;
			break;
		case pinky:
			pinkyLocation = i;
			break;
		case clyde:
			clydeLocation = i;
			break;
		default:
			break;
		}
	}

	dotsRemaining = dotsCounter;
}

void Grid::Flash()	// 클리어 시 반짝반짝 효과
{
	int alternate = 1;	// 교체용 변수
	int color = 0;

	for (int k = 0; k < 10; k++) {
		if (alternate == 0) {
			color = 1;		// 파랑색
			alternate = 1;
		}
		else {
			color = 15;		// 흰색
			alternate = 0;
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				switch (currentGrid[(i * columns) + j]) {
				case wall:			//   벽이라면
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);		// 색상 변경
					SetConsoleCursorPosition(hStdout, { (short)j, (short)i });			// 커서 위치 지정		
					cout << "#";
					break;
				default:
					break;
				}
			}
		}

		Sleep(200);
	}
}

Grid::Grid()
{
	Reset();
}

void Grid::Draw(Game& game, Pacman& pacmanObj)		// 그리드를 실제로 그리는 작업
{
	char character, color;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			switch (currentGrid[(i * columns) + j]) {
			case wall:		// 벽
				character = wall_char; color = wall_color; break;
			case white_dot:			// 먹는 점
				character = white_dot_char; color = white_dot_color; break;
			case yellow_dot:		// 무적 모드로 변하는 점
				character = yellow_dot_char; color = yellow_dot_color; break;
			case blinky:			// 적 1, 2, 3, 4
				character = blinky_char; color = blinky_color; break;
			case inky:
				character = inky_char; color = inky_color; break;
			case pinky:
				character = pinky_char; color = pinky_color; break;
			case clyde:
				character = clyde_char; color = clyde_color; break;
			case pacman:		// 팩맨
								// Make pacman chomp
				if (pacmanObj.mouthOpen) {		// 움직일 때 입을 열었다 닫았다
					character = pacman_mouth_open_char;
				}
				else {
					character = pacman_char;
				}

				// Flash pacman for last 10 moves of godmode
				if (pacmanObj.godMode) {		// 무적 모드가 풀리기 10 스텝 전 반짝이는 효과
					if ((pacmanObj.godModeMovesLeft <= 10) && (pacmanObj.godModeMovesLeft % 2 == 1)) {
						color = pacman_flash_color;
					}
					else {
						color = pacman_godmode_color;
					}
				}
				else {
					color = pacman_color;
				}

				break;
				// Nothing & Teleport
			default:
				character = nothing_char; color = nothing_color; break;
			}

			// character를 그림 (character는 팩맨이 아닌 각각의 그리드 data를 말함)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
			SetConsoleCursorPosition(hStdout, { (short)j, (short)i });
			cout << character;
		}
	}

	// Print score and lives
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetConsoleCursorPosition(hStdout, { 0, 26 });

	int spacing;		// 점수 자릿수에 따라 줄이 밀려나므로 공백을 메꾸는 작업

	if (game.score > 9999) {
		spacing = 3;
	}
	else if (game.score > 999) {
		spacing = 4;
	}
	else if (game.score > 99) {
		spacing = 5;
	}
	else if (game.score > 9) {
		spacing = 6;
	}
	else {
		spacing = 7;
	}

	cout << "Score: " << game.score;		// 점수와 남은 목숨 표시

	for (int i = 0; i < spacing; i++) {		// 공백
		cout << " ";
	}

	cout << "Lives: " << game.lives;
}

void movePacman(Game& game, Grid& grid, Pacman& pacman, int newLocation) {		// 팩맨 이동
	pacman.mouthOpen = !pacman.mouthOpen;		// 입을 열고 닫음

	int locationOffset = newLocation;
	int oldLocation = grid.pacmanLocation;		// 그리드에 있는 팩맨의 위치를 oldLocation에 넣는다.
	newLocation = oldLocation + newLocation;

	int oldCharacter = grid.currentGrid[oldLocation];		// 현재 캐릭터 위치
	int newCharacter = grid.currentGrid[newLocation];		// 이동할 캐릭터 위치

															// Process godmode
	if (pacman.godMode) {
		if (pacman.godModeMovesLeft - 1 == 0) {		// 무적 이동횟수가 다되면
			pacman.godMode = false;
			pacman.godModeMovesLeft = pacman.godModeMaxMoves;
		}
		else {
			pacman.godModeMovesLeft--;
		}
	}

	if (oldLocation == grid.leftTeleporterLocation && locationOffset == -1) { // 왼쪽 텔레포트 지점에서 왼쪽으로 가면
		grid.currentGrid[oldLocation] = grid.teleport;		// 이동 전 위치 그리드를 teleport로 바꿈 (팩맨 제거)
		grid.pacmanLocation = grid.rightTeleporterLocation;		// 팩맨의 좌표를 오른쪽 텔레포트 좌표으로 이동
		grid.currentGrid[grid.rightTeleporterLocation] = grid.pacman;	// 오른쪽 텔레포트 그리드를 팩맨으로 바꿈
	}
	else if (oldLocation == grid.rightTeleporterLocation && locationOffset == 1) { // 오른쪽 텔레포트 지점 -> 오른쪽 이동
		grid.currentGrid[oldLocation] = grid.teleport;
		grid.pacmanLocation = grid.leftTeleporterLocation;
		grid.currentGrid[grid.leftTeleporterLocation] = grid.pacman;
	}
	else if (newLocation == grid.leftTeleporterLocation || newLocation == grid.rightTeleporterLocation) {		// 텔레포트 후
		grid.currentGrid[oldLocation] = grid.nothing;		// 이동 전 위치 그리드를 빈 공간으로
		grid.pacmanLocation = newLocation;					// 팩맨의 좌표를 이동 후 좌표로
		grid.currentGrid[newLocation] = grid.pacman;		// 이동 후 위치를 팩맨으로 바꿈 
	}
	else if (newCharacter == grid.white_dot) {		// 캐릭터 이동 후 위치가 먹는 점의 위치라면
		game.score++;				// 점수 증가
		grid.dotsRemaining--;				// 남은 점 개수 --
		grid.currentGrid[grid.pacmanLocation] = grid.nothing;		// 팩맨 이동
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;

		if (grid.dotsRemaining == 0) {	// 모든 점을 다 먹었다면
			grid.Draw(game, pacman);		// 그려놓고
			grid.Flash();					// 반짝 반짝 효과
			grid.Reset();					// 게임 리셋
			pacman.Reset();					// 팩맨 리셋
		}
	}
	else if (newCharacter == grid.yellow_dot) {		// 무적 모드 발동 이벤트
		Beep((DWORD) 261.63 * 2, 200); // C
		Beep((DWORD) 329.63 * 2, 200); // E
		grid.dotsRemaining--;
		grid.currentGrid[grid.pacmanLocation] = grid.nothing;
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;
		pacman.godMode = true;		// 무적모드 활성화
		pacman.godModeMovesLeft = pacman.godModeMaxMoves;		// 무적모드 남은 이동 = 무적모드 최대 이동수

		if (grid.dotsRemaining == 0) {		// 모든 점 다 먹었을 때(Yellow_dot)
			grid.Draw(game, pacman);
			grid.Flash();
			grid.Reset();
			pacman.Reset();
		}
	}
	else if (newCharacter == grid.nothing) {	// 빈 곳으로 이동할 경우
		grid.currentGrid[oldLocation] = grid.nothing;
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;
	}
}

void moveLeft(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, -1);			// 왼쪽
}
void moveRight(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, 1);			// 오른쪽
}
void moveUp(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, -23);		// 열의 수만큼 줄임, 그리드 상으로 이전 줄
}
void moveDown(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, 23);			// 열의 수만큼 늘임, 그리드 상으로 다음 줄
}

void themeMusic() {		// 메인 테마 음악, 음계를 이용해서 만듬
	Beep((DWORD) 245.94, 200); // B
	Beep((DWORD) 245.94 * 2, 200); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 200); // Eb
	Beep((DWORD) 245.94 * 2, 100); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 400); // Eb
	Sleep(100);

	Beep((DWORD) 261.63, 200); // C 
	Beep((DWORD) 261.63 * 2, 200); // C
	Beep((DWORD) 391.00, 200); // G
	Beep((DWORD) 329.63, 200); // E
	Beep((DWORD) 261.63 * 2, 100); // C
	Beep((DWORD) 391.00, 200); // G
	Beep((DWORD) 329.63, 400); // E
	Sleep(100);

	Beep((DWORD) 245.94, 200); // B
	Beep((DWORD) 245.94 * 2, 200); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 200); // Eb
	Beep((DWORD) 245.94 * 2, 100); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 400); // Eb
	Sleep(100);

	Beep((DWORD) 155.56 * 2, 100); // Eb
	Beep((DWORD) 329.63, 100); // E
	Beep((DWORD) 349.23, 200); // F

	Beep((DWORD) 349.23, 100); // F
	Beep((DWORD) 369.99, 100); // F#
	Beep((DWORD) 391.00, 200); // G

	Beep((DWORD) 391.00, 100); // G
	Beep((DWORD) 415.30, 100); // Ab
	Beep((DWORD) 440.00, 200); // A
	Beep((DWORD) 245.00 * 2, 400); //B
}

int main()
{
	// Center the console to the center of the screen
	HWND console = GetConsoleWindow();
	int consoleWidth = 800;		// 게임 화면 크기
	int consoleHeight = 600;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);	// 컴퓨터 스크린 화면 크기
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(console, (screenWidth - consoleWidth) / 2, (screenHeight - consoleHeight) / 2, consoleWidth, consoleHeight, TRUE);
	// 콘솔을 화면 중앙으로 배치하는 작업

	// Get stdout handle
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);		// 커서 위치 반환

													// Hide cursor
	CONSOLE_CURSOR_INFO CURSOR;		// 커서 숨기기 코드
	CURSOR.dwSize = 1;
	CURSOR.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &CURSOR);

	// Set up game
	Game game;
	Pacman pacman;
	Grid grid;

	// Main game loop
	while (!game.exit) {
		if (!game.themeMusicPlayed) {
			grid.Draw(game, pacman);		// 테마 음악을 틀고 그리드를 그림.
			themeMusic();
			game.themeMusicPlayed = 1;
		}
		else {
			if (_kbhit()) {
				switch (_getch()) {		// 키보드 키입력
				case KB_ESCAPE:
					game.exit = true;
					break;
				case KB_LEFT:
					moveLeft(game, grid, pacman);
					break;
				case KB_RIGHT:
					moveRight(game, grid, pacman);
					break;
				case KB_UP:
					moveUp(game, grid, pacman);
					break;
				case KB_DOWN:
					moveDown(game, grid, pacman);
					break;
				}
			}

			grid.Draw(game, pacman);
		}
	}

	return 0;
}
