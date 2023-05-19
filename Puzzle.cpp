#include "Puzzle.h"

// Using
using Terminal::COLOR;

Puzzle::Puzzle() {
	m_hProv = NULL;
	m_bStart = false;
	memset(&m_Area, 0, sizeof(m_Area));

	CryptAcquireContext(&m_hProv, NULL, _T("Microsoft Base Cryptographic Provider v1.0"), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
}

Puzzle::~Puzzle() {
	CryptReleaseContext(m_hProv, NULL);
}

bool Puzzle::Initialize() {
	if (!m_hProv || m_bStart) {
		return false;
	}

	static const Matrix4x4 Area = {
		{  1,  2,  3,  4 },
		{  5,  6,  7,  8 },
		{  9, 10, 11, 12 },
		{ 13, 14, 15,  0 }
	};

	memcpy(&m_Area, Area, sizeof(Area));

	for (unsigned char unY = 0; unY < 4; ++unY) {
		for (unsigned char unX = 0; unX < 4; ++unX) {
			unsigned char unXY[2] = { 0, 0 };
			if (!CryptGenRandom(m_hProv, sizeof(unXY), reinterpret_cast<PBYTE>(&unXY))) {
				continue;
			}

			unXY[0] %= 4;
			unXY[1] %= 4;

			std::swap(m_Area[unY][unX], m_Area[unXY[1]][unXY[0]]);
		}
	}

	m_bStart = true;

	return true;
}

bool Puzzle::Run(Terminal::Screen* const pScreen, bool* pIsSolved) {
	if (!m_hProv || !m_bStart || !pScreen) {
		return false;
	}

	COORD CurrentPosition = { 0, 0 };
	pScreen->GetCursorPosition(&CurrentPosition);
	pScreen->HideCursor();

	bool bIsSolved = false;
	SHORT LastButtonStates[4] = { 0, 0, 0, 0 };
	while (!bIsSolved && m_bStart) {
		bIsSolved = true;
		unsigned char unSequence = 1;
		for (unsigned char unY = 0; bIsSolved && (unY < 4); ++unY) {
			for (unsigned char unX = 0; bIsSolved && (unX < 4); ++unX) {
				const unsigned int unValue = m_Area[unY][unX];
				if (unValue != unSequence % 16) {
					bIsSolved = false;
				}

				++unSequence;
			}
		}

		if (bIsSolved) {
			if (pIsSolved) {
				*pIsSolved = bIsSolved;
			}

			m_bStart = false;
			break;
		}

		unsigned char unEmptyXY[2] = { 0, 0 };
		for (unsigned char unY = 0; unY < 4; ++unY) {
			for (unsigned char unX = 0; unX < 4; ++unX) {
				if (m_Area[unY][unX] == 0) {
					unEmptyXY[0] = unX;
					unEmptyXY[1] = unY;
					break;
				}
			}
		}

		const SHORT CurrentButtonStates[4] = {
			GetAsyncKeyState(VK_UP) & 0x8000, GetAsyncKeyState(VK_DOWN) & 0x8000,
			GetAsyncKeyState(VK_LEFT) & 0x8000, GetAsyncKeyState(VK_RIGHT) & 0x8000
		};

		for (unsigned char i = 0; i < 4; ++i) {
			if (CurrentButtonStates[i] && !LastButtonStates[i]) {
				switch (i) {
				case 0: // Up
					if (unEmptyXY[1] < 3) {
						std::swap(m_Area[unEmptyXY[1]][unEmptyXY[0]], m_Area[unEmptyXY[1] + 1][unEmptyXY[0]]);
					}
					break;
				case 1: // Down
					if (unEmptyXY[1] > 0) {
						std::swap(m_Area[unEmptyXY[1]][unEmptyXY[0]], m_Area[unEmptyXY[1] - 1][unEmptyXY[0]]);
					}
					break;
				case 2: // Left
					if (unEmptyXY[0] < 3) {
						std::swap(m_Area[unEmptyXY[1]][unEmptyXY[0]], m_Area[unEmptyXY[1]][unEmptyXY[0] + 1]);
					}
					break;
				case 3: // Right
					if (unEmptyXY[0] > 0) {
						std::swap(m_Area[unEmptyXY[1]][unEmptyXY[0]], m_Area[unEmptyXY[1]][unEmptyXY[0] - 1]);
					}
					break;
				default:
					break;
				}
			}
			LastButtonStates[i] = CurrentButtonStates[i];
		}

		Terminal::Console Console(pScreen);

		pScreen->SetCursorPosition({ 0, CurrentPosition.Y });

		for (unsigned char unY = 0; unY < 4; ++unY) {
			for (unsigned char unX = 0; unX < 4; ++unX) {
				COORD PositionOffset = { unX + unX * 5, CurrentPosition.Y + unY + unY * 3 };
				pScreen->SetCursorPosition(PositionOffset);

				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#######\n"));
				PositionOffset.Y += 1;

				pScreen->SetCursorPosition(PositionOffset);
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));
				Console.tprintf({ COLOR::COLOR_GRAY, COLOR::COLOR_DARK_YELLOW }, _T("     "));
				pScreen->SetCursorPosition({ static_cast<SHORT>(PositionOffset.X + 6), PositionOffset.Y });
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));
				PositionOffset.Y += 1;

				pScreen->SetCursorPosition(PositionOffset);
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));
				Console.tprintf({ COLOR::COLOR_GRAY, COLOR::COLOR_DARK_YELLOW }, _T("     "));
				pScreen->SetCursorPosition({ static_cast<SHORT>(PositionOffset.X + 6), PositionOffset.Y });
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));

				const unsigned int unValue = m_Area[unY][unX];
				if (unValue != 0) {
					pScreen->SetCursorPosition({ static_cast<SHORT>(PositionOffset.X + 3), PositionOffset.Y });
					Console.tprintf({ COLOR::COLOR_GRAY, COLOR::COLOR_DARK_RED }, _T("%lu"), unValue);
				}
				else {
					pScreen->SetCursorPosition({ static_cast<SHORT>(PositionOffset.X + 1), PositionOffset.Y });
					Console.tprintf({ COLOR::COLOR_GRAY, COLOR::COLOR_DARK_YELLOW }, _T("     "));
				}

				PositionOffset.Y += 1;

				pScreen->SetCursorPosition(PositionOffset);
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));
				Console.tprintf({ COLOR::COLOR_GRAY, COLOR::COLOR_DARK_YELLOW }, _T("     "));
				pScreen->SetCursorPosition({ static_cast<SHORT>(PositionOffset.X + 6), PositionOffset.Y });
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#"));
				PositionOffset.Y += 1;

				pScreen->SetCursorPosition(PositionOffset);
				Console.tprintf({ COLOR::COLOR_DARK_YELLOW, COLOR::COLOR_DARK_YELLOW }, _T("#######\n"));
			}
		}

		Sleep(5);
	}

	pScreen->ShowCursor();

	return true;
}
