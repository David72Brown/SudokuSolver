#pragma once

#include <assert.h>
#include <atlstr.h>

typedef unsigned char uchar;
typedef unsigned short int uint16;
const uint16 ALL_POSSIBILITIES = 0x1FF;

#define MY_ASSERTS_ON
#ifdef MY_ASSERTS_ON
#define my_assert(assertion) assert(assertion)
#else
#define my_assert(assertion)
#endif

typedef enum { SUDOKU_2D, SUDOKU_3D } eDimensions;

typedef struct {
    bool Diagonals;
    CString GridStr[9][9]; // Enough for a 3D sudoku. Indexed as GridStr[Up][Forward].
    CString ExpectedGrid[9][9];
} tTestSudoku;

extern const tTestSudoku Tests_2D[];
extern const tTestSudoku Tests_3D[];
