
// Default
#include <Windows.h>
#include <tchar.h>

// STL
#include <iostream>

// Terminal
#include "Terminal.h"

// Puzzle
#include "Puzzle.h"

// Using
using Terminal::WINDOW_NATIVE_IO;
using Terminal::COLOR;

int _tmain() {

	Terminal::Window Window(true);
	if (Window.Open(true)) {

		SetWindowText(Window.GetWindow(), _T("15puzzle")); // SetConsoleTitle also works

		POINT CurrentPosition = { 0, 0 };
		if (ScreenToClient(Window.GetWindow(), &CurrentPosition)) {
			if (SetWindowPos(Window.GetWindow(), NULL, CurrentPosition.x, CurrentPosition.y, 325, 365, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE)) {
				SetWindowLong(Window.GetWindow(), GWL_STYLE, GetWindowLong(Window.GetWindow(), GWL_STYLE) & ~WS_SIZEBOX);
			}
		}

		ShowScrollBar(Window.GetWindow(), SB_BOTH, FALSE);

		WINDOW_NATIVE_IO NativeIOs;
		if (Window.GetNativeIO(&NativeIOs)) {
			SetConsoleScreenBufferSize(NativeIOs.m_hOUT, { 1, 1 });
		}

		Terminal::Screen Screen(&Window);

		CONSOLE_SCREEN_BUFFER_INFOEX csbi;
		memset(&csbi, 0, sizeof(csbi));
		if (Screen.GetBufferInfo(&csbi)) {
			csbi.dwSize.X = 37;
			csbi.dwSize.Y = 20;
			Screen.SetBufferInfo(csbi);
		}

		Screen.Flush(true);

		Terminal::Console Console(&Screen);
		Console.tprintf(COLOR::COLOR_WHITE, _T("15puzzle [1.0.0] (zeze839@gmail.com)\n\n"));

		Puzzle Puzzle;
		if (!Puzzle.Initialize()) {
			Console.tprintf(COLOR::COLOR_WHITE, _T("[!] Error: Failed to initialize game.\n"));
			return -1;
		}

		bool bIsSolved = false;
		if (!Puzzle.Run(&Screen, &bIsSolved)) {
			Console.tprintf(COLOR::COLOR_WHITE, _T("[!] Error: Failed to run game.\n"));
			return -1;
		}

		Screen.Flush(true);
		Screen.SetCursorPosition({ 0, 0 });

		memset(&csbi, 0, sizeof(csbi));

		if (bIsSolved) {
			if (Screen.GetBufferInfo(&csbi)) {
				const SHORT unWidth = csbi.srWindow.Right - csbi.srWindow.Left;
				const SHORT unHeight = csbi.srWindow.Bottom - csbi.srWindow.Top;
				Screen.SetCursorPosition({ static_cast<SHORT>(csbi.srWindow.Left + (unWidth / 2) - 4), static_cast<SHORT>(csbi.srWindow.Top + (unHeight / 2)) });
			}

			Console.tprintf(COLOR::COLOR_WHITE, _T("YOU WIN!"));
		} else { // Doesn't make sense because the game continues indefinitely until the player wins.
			if (Screen.GetBufferInfo(&csbi)) {
				const SHORT unWidth = csbi.srWindow.Right - csbi.srWindow.Left;
				const SHORT unHeight = csbi.srWindow.Bottom - csbi.srWindow.Top;
				Screen.SetCursorPosition({ static_cast<SHORT>(csbi.srWindow.Left + (unWidth / 2) - 4), static_cast<SHORT>(csbi.srWindow.Top + (unHeight / 2)) });
			}

			Console.tprintf(COLOR::COLOR_WHITE, _T("YOU LOSE!"));
		}

		Sleep(5000);
	}

	return 0;
}
