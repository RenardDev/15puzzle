#pragma once

#ifndef _PUZZLE_H_
#define _PUZZLE_H_

// Default
#include <Windows.h>

// Terminal
#include "Terminal.h"

// Definitions
typedef unsigned int Matrix4x4[4][4];

class Puzzle {
public:
	Puzzle();
	~Puzzle();

public:
	bool Initialize();

public:
	bool Run(Terminal::Screen* const pScreen, bool* pIsSolved);

private:
	HCRYPTPROV m_hProv;
	bool m_bStart;
	Matrix4x4 m_Area;
};

#endif // !_PUZZLE_H_
