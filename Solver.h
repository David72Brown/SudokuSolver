#pragma once

#include "TestsAndMisc.h"

typedef enum { SUCCESS, FAILURE, BAD_INITIAL_DATA } eResult;

class cSolver
{
public:
    cSolver();
    eDimensions Dimensions;
    bool Diagonals;
    void ClearCellContents(void);
    void SetCellContents(uchar Up, uchar Forward, uchar Across, uchar Number);
    uint16 GetCellContents(uchar Up, uchar Forward, uchar Across);
    eResult SolveIt(void);

private:
    uint16 CellContents[9][9][9];
    bool ContentsValid;
    uchar GetNextBitPos(uint16 x, uchar Number);
    uchar NumberEveryThreeInNineset(uint16* NinePtrs[9], uchar Number);
    void Assign3DDiagonalNineset(uint16* NinePtrs[9], uchar diag);
    uchar GetOnlyPossibility(uint16 CellPossibilities);
    uchar NumPossibilities(uint16 x);
    uint16 GetNextPossibility(uint16 Possibilities, uint16 StartPossibility);
    uchar NumberInOneThirdOfNineset(uint16* NinePtrs[9], uchar Number);
    bool NumberAt159InNineset(uint16* NinePtrs[9], uchar Number);
    void AssignNineset(uint16* NinePtrs[9], uchar UpFirst, uchar UpLast, uchar AcrossFirst, uchar AcrossLast, uchar BackFirst, uchar BackLast);
    bool IsNinesetDecided(uint16* NinePtrs[]);
    bool AdvanceNinesetOnce(uint16* NinePtrs[9]);
    bool AdvanceNineset(uint16* NinePtrs[9]);
    bool CheckNinesetValidity(uint16* NinePtrs[9]);
    bool IsGridComplete();
    void TryGuessing(void);
    void Assign2DDiagonalNineset(uint16* NinePtrs[9]);
    void RotateGrid(bool Fwd);
    bool CheckGridValid(void);
    bool AdvanceGridAsFarAsPossible(void);
    // For guesses:
        static const uint16 MAX_GUESSES = 81;
        uchar NumGuesses;
        void PushGuess(int Across, int Back, uchar Number);
        void PopGuess(int* Across, int* Back, uchar* Number);
        void PopOffAllGuesses(void);
        // For each guess, store grid contents. Each grid is 81 cells with 9 bits to record the possibilities in each cell.
        uchar Compressed2DGrids[MAX_GUESSES][(MAX_GUESSES*9+7)/8];
        // Each guess is 4 bits for Across, 4 bits for Back, 4 bits for Number.
        uint16 Compressed2DGuess[MAX_GUESSES];
};
