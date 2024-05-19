#pragma once

#include <assert.h>
#include <atlstr.h>

typedef unsigned char uchar;
typedef unsigned short int uint16;
#define ALL_POSSIBILITIES 0x1FF
#define MAX_NUM_CAGES 50
#define MAX_CELLS_IN_CAGE 9

#define MY_ASSERTS_ON
#ifdef MY_ASSERTS_ON
#define my_assert(assertion) assert(assertion)
#else
#define my_assert(assertion)
#endif

typedef enum { SUDOKU_2D, SUDOKU_3D, SUDOKU_KILLER } eDimensions;
typedef enum { NO_DIAGONALS, DIAGONALS } eDiagonals;

typedef struct {
    eDimensions Dimensions;
    eDiagonals Diagonals;
    CString Grid[9][9];
    // Only for killer (caged) sudokus:
    CString Cages[9];
    struct {
        CString CageTag;
        uchar CageSum;
    } TagsAndSums[MAX_NUM_CAGES];
} tSudokuSpec;

typedef struct {
    
    tSudokuSpec TestSudokuSpec;
    CString ExpectedGrid[9][9];
} tTestSudoku;

// Textual cage info is converted into these data structures.
typedef struct {
    uchar NumCellsInCage;
    uchar CageSum;
    struct {
        uchar Across;
        uchar Back;
    } GridCell[MAX_CELLS_IN_CAGE];
} tCage;

typedef struct {
    uchar NumCages;
    tCage Cages[MAX_NUM_CAGES];
    uchar CellsToCages[9][9]; // For any grid cell, this will find you the cage it falls in.
} tCageData;

extern const uchar GrayCodeBitsToIncrement[];
extern const tTestSudoku TestSudokus[];
extern const uchar NumTestSudokus;

extern uint16 Grid[9][9][9]; // Indexed as Grid[Up][Across][Back]
extern uchar GridHeight;     // 1 for 2D, 9 for 3D.
extern tCageData CageData;
