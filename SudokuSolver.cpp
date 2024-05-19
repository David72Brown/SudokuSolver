#include <iostream>
#include <atlstr.h>
using namespace std;
#include "Data.h"
#include "Start grid.h"
#include "GuessStack.h"

static bool GridValid;
static eDiagonals DoingDiagonals;
static eDimensions Dimensions;

bool ExactlyOneBitSet(uint16 x);
uchar GetOnlyPossibility(uint16 CellPossibilities);
uchar NumBitsSet(uint16 x);
void SolveItNicely(void);
uchar GetNextBitPos(uint16 x, uchar Number);
uchar GetLastBitPos(uint16 x);
uchar NumberInOneThirdOfNineset(uint16* NinePtrs[9], uchar Number);
bool NumberAt159InNineset(uint16* NinePtrs[9], uchar Number);
void AssignNineset(uint16* NinePtrs[9], uchar UpFirst, uchar UpLast, uchar AcrossFirst, uchar AcrossLast, uchar BackFirst, uchar BackLast);
uint16 CalculateChecksum(uint16* pData, uint16 NumBytes);
void DumpUnfinishedGrid(void);
void DumpPossibilities(void);
void DumpFinishedGrid(void);
bool IsNinesetDecided(uint16* NinePtrs[]);
bool AdvanceNinesetOnce(uint16* NinePtrs[9]);
bool AdvanceNineset(uint16* NinePtrs[9]);
bool CheckNinesetValidity(uint16* NinePtrs[9]);
void RunUnitTests(void);
bool IsGridComplete();
void TryGuessing(void);
void Assign2DDiagonalNineset(uint16* NinePtrs[9]);
void SkipBack(int& a, int& b, uchar Skip);
bool ExhaustivelyCheckCombinations(void);
void SetUpGridFromSpec(const tSudokuSpec *SudokuSpec);
void RotateGrid(bool Fwd);
bool AdvanceGridAsFarAsPossible(void);
void AdvanceCage(uchar CageNum);
bool CanMakeTotal(uchar CageNum);
bool FindNumberInLineAcross(uchar Number, uchar Back);
bool FindNumberInLineBack(uchar Number, uchar Across);
bool FindNumberInSlab(uchar Number, uchar Across, uchar Back);

//#define RUN_TESTS

//#define DEBUG_ON
#ifdef DEBUG_ON
#define SPIT printf
#else
#define SPIT DeadEndFn
void DeadEndFn(CString, ...) {}
#endif

void SetUpGridFromSpec(const tSudokuSpec* SudokuSpec)
{
    uchar Up, Across, Back;

    DoingDiagonals = SudokuSpec->Diagonals;
    Dimensions = SudokuSpec->Dimensions;
    GridHeight = (Dimensions == SUDOKU_3D) ? 9 : 1;

    for (Up = 0; Up < GridHeight; Up++)
    {
        for (Back = 0; Back < 9; Back++)
        {
            uchar StrAcross;
            for (StrAcross = Across = 0; Across < 9; Across++, StrAcross++)
            {
                // Skip over "|" characters in puzzle grid.
                if (Across == 3 || Across == 6)
                    StrAcross += 1;
                if (SudokuSpec->Grid[Up][Back][StrAcross] == ' ')
                    Grid[Up][Across][8 - Back] = ALL_POSSIBILITIES;
                else
                    Grid[Up][Across][8 - Back] = 1 << (SudokuSpec->Grid[Up][Back][StrAcross] - '0' - 1);
            }
        }
    }

    if (Dimensions == SUDOKU_KILLER) // I.e. with cages.
    {
        CString Tag;
        int c;

        // Count the cages.
        memset(&CageData, 0, sizeof(CageData));
        for (CageData.NumCages = 0; SudokuSpec->TagsAndSums[CageData.NumCages].CageTag != ""; )
            CageData.NumCages += 1;

        for (Back = 0; Back < 9; Back++)
        {
            // Parse cage string
            for (Across = 0; Across < 9; Across++)
            {
                // Format is e.g. "AA AA AB AB AC AC AD AD AE". That is 9 x <2-char cage tag> separated by single chars of any value.
                Tag = SudokuSpec->Cages[Back].Mid(3 * Across, 2);
                // Find cage in list.
                for (c = 0; c < CageData.NumCages; c++)
                {
                    if (Tag == SudokuSpec->TagsAndSums[c].CageTag)
                    {
                        CageData.Cages[c].GridCell[CageData.Cages[c].NumCellsInCage].Across = Across;
                        CageData.Cages[c].GridCell[CageData.Cages[c].NumCellsInCage].Back = 8 - Back;
                        CageData.Cages[c].CageSum = SudokuSpec->TagsAndSums[c].CageSum;
                        CageData.CellsToCages[Across][8-Back] = c;
                        CageData.Cages[c].NumCellsInCage += 1;
                        break;
                    }
                }
                my_assert(c < CageData.NumCages);
            }
        }
    }
}

bool ExactlyOneBitSet(uint16 x)
{
    return x > 0 && ((x & (x - 1)) == 0);
}

// Returns 0 if more than one possibility remains.
uchar GetOnlyPossibility(uint16 CellPossibilities)
{
    switch (CellPossibilities) {
    case 0x100:
        return 9;
    case 0x080:
        return 8;
    case 0x040:
        return 7;
    case 0x020:
        return 6;
    case 0x010:
        return 5;
    case 0x008:
        return 4;
    case 0x004:
        return 3;
    case 0x002:
        return 2;
    case 0x001:
        return 1;
    }
    return 0;
}

uchar NumBitsSet(uint16 x)
{
    int i;
    uchar Count;

    for (Count = 0, i = 0; i < 16; i++)
    {
        Count += x & 1;
        x = x >> 1;
    }
    return Count;
}

// Returns the next (i.e. more significant) bit position after Number. Number==1 means the LSB.
uchar GetNextBitPos(uint16 x, uchar Number)
{
    uchar Shifts;

    x = (x >> Number);
    if (x == 0)
        return 0;
    for (Shifts = 0; (x & 0x01) == 0; x = x >> 1)
        Shifts += 1;
    return Number + Shifts + 1;
}

uchar GetLastBitPos(uint16 x)
{
    uchar Shifts;

    if (x == 0)
        return 0;
    for (Shifts = 0; x != 0; x = x >> 1)
        Shifts += 1;
    return Shifts;
}

/* This fn ignores cases where a number appears just once in the nineset because that will cause that
   cell to be resolved, so the number will be taken out of the rest of the line/slab anyway. This fn
   only finds 2 or 3 instances of a number within one third of the nineset. */
uchar NumberInOneThirdOfNineset(uint16* NinePtrs[9], uchar Number)
{
    int Cell;
    bool FoundInThird[3] = { false, false, false };
    uchar WhichThird = 0;
    uchar NumCount;

    for (NumCount = 0, Cell = 0; Cell < 9; Cell++)
    {
        if ((*NinePtrs[Cell] & (1 << (Number - 1))) >= 1)
        {
            NumCount += 1;
            WhichThird = Cell / 3;
            FoundInThird[WhichThird] = true;
        }
    }
    if (FoundInThird[0] + FoundInThird[1] + FoundInThird[2] == 1)
    {
        my_assert(NumCount >= 1 && NumCount <= 3);
        if (NumCount > 1)
            return WhichThird + 1;
    }
    return 0;
}

/* This fn ignores cases where a number appears just once in the nineset because that will cause that
   cell to be resolved, so the number will be taken out of the rest of the slab anyway. This fn only
   finds 2 or 3 instances of a number within a particular set of 3 cells of the nineset. */
uchar NumberEveryThreeInNineset(uint16* NinePtrs[9], uchar Number)
{
    uint16* RejiggedNinePtrs[9] = { NinePtrs[0], NinePtrs[3], NinePtrs[6], NinePtrs[1], NinePtrs[4], NinePtrs[7], NinePtrs[2], NinePtrs[5], NinePtrs[8] };

    return NumberInOneThirdOfNineset(RejiggedNinePtrs, Number);
}

bool NumberAt159InNineset(uint16* NinePtrs[9], uchar Number)
{
    if (((*NinePtrs[0] & (1 << (Number - 1))) >> (Number - 1))
      + ((*NinePtrs[4] & (1 << (Number - 1))) >> (Number - 1))
      + ((*NinePtrs[8] & (1 << (Number - 1))) >> (Number - 1)) >= 2

        && (*NinePtrs[1] & (1 << (Number - 1))) == 0
        && (*NinePtrs[2] & (1 << (Number - 1))) == 0
        && (*NinePtrs[3] & (1 << (Number - 1))) == 0
        && (*NinePtrs[5] & (1 << (Number - 1))) == 0
        && (*NinePtrs[6] & (1 << (Number - 1))) == 0
        && (*NinePtrs[7] & (1 << (Number - 1))) == 0)

        return true;
    else
        return false;
}

void AssignNineset(uint16 *NinePtrs[9], uchar UpFirst, uchar UpLast, uchar AcrossFirst, uchar AcrossLast, uchar BackFirst, uchar BackLast)
{
    static uchar Up, Across, Back; // Static to save constant allocs and deallocs since this function will be run many times.
    static uchar Count; // We should always assign exactly 9 cells.

    Count = 0;
    for (Up = UpFirst; Up <= UpLast; Up++)
    {
        for (Across = AcrossFirst; Across <= AcrossLast; Across++)
        {
            for (Back = BackFirst; Back <= BackLast; Back++)
            {
                my_assert(Count < 9);
                NinePtrs[Count++] = &Grid[Up][Across][Back];
            }
        }
    }
    my_assert(Count == 9);
}

void Assign2DDiagonalNineset(uint16 *NinePtrs[9])
{
    static int Cell;

    for (Cell = 0; Cell < 9; Cell++)
    {
        NinePtrs[Cell] = &Grid[0][Cell][Cell];
    }
}

void Assign3DDiagonalNineset(uint16* NinePtrs[9], uchar diag)
{
    int Up, x;

    for (Up = 0; Up < 9; Up++)
    {
        for (x = 0; x < 9; x++)
        {
            switch (diag)
            {
            case 0:
                // Bottom left near to top right far.
                NinePtrs[Up] = &Grid[Up][x][x];
                break;
            case 1:
                // Bottom left far to top right near.
                NinePtrs[Up] = &Grid[Up][x][8 - x];
                break;
            case 2:
                // Bottom right near to top left far.
                NinePtrs[Up] = &Grid[Up][8 - x][x];
                break;
            case 3:
                NinePtrs[Up] = &Grid[Up][8 - x][8 - x];
                // Bottom right far to top left near.
                break;
            }
        }
    }
}

void RotateGrid(bool Fwd)
{
    static uint16 RotatedGrid[9][9][9];
    static int Up, Across, Back;

    // You can't use a rotated grid when examining cages because the mappings from cage cells to Grid
    // cells will get messed up. You can rotate a Killer grid, you just need to rotate it back again
    // before checking cages.
    if (Dimensions == SUDOKU_2D || Dimensions == SUDOKU_KILLER)
    {
        for (Across = 0; Across < 9; Across++)
        {
            for (Back = 0; Back < 9; Back++)
            {
                if (Fwd)
                    RotatedGrid[0][8 - Back][Across] = Grid[0][Across][Back];
                else
                    RotatedGrid[0][Across][Back] = Grid[0][8 - Back][Across];
            }
        }
        memcpy(Grid, RotatedGrid[0], sizeof(RotatedGrid[0]));
    }
    else
    {
        for (Up = 0; Up < 9; Up++)
        {
            for (Across = 0; Across < 9; Across++)
            {
                for (Back = 0; Back < 9; Back++)
                {
                    if (Fwd)
                        RotatedGrid[Across][Back][Up] = Grid[Up][Across][Back];
                    else
                    {
                        RotatedGrid[Up][Across][Back] = Grid[Across][Back][Up];
                        // Don't think we ever rotate a 3D grid backwards.
                        my_assert(false);
                    }
                }
            }
        }
        memcpy(Grid, RotatedGrid, sizeof(RotatedGrid));
    }
}

uint16 CalculateChecksum(uint16* pData, uint16 NumBytes)
{
    uchar* BytesToChecksum = (uchar*)pData;
    int i, iNumberOfBytesToChecksum = NumBytes;
    int iSum1 = 0;
    int iSum2 = 0;
    int iCheck1 = 0;
    int iCheck2 = 0;
    int iChecksum;
    BYTE* pBYTE = BytesToChecksum;
    for (i = 0; i < iNumberOfBytesToChecksum; i++) {
        iSum1 = (iSum1 + *pBYTE) % 255;
        iSum2 = (iSum1 + iSum2) % 255;
        pBYTE++;
    }
    iCheck1 = 255 - ((iSum1 + iSum2) % 255);
    iCheck2 = 255 - ((iSum1 + iCheck1) % 255);
    iChecksum = (int)(iCheck1 & 0x00ff);
    iChecksum |= (int)((iCheck2 & 0x00ff) << 8);
    return iChecksum;
}

void DumpUnfinishedGrid (void)
{
    int Up, Back;
    uint16 xsum;

    xsum = CalculateChecksum (&Grid[0][0][0], sizeof(Grid));

    for (Up = 0; Up < GridHeight; Up++)
    {
        printf("+-----------+-----------+-----------+\n");
        for (Back = 8; Back >= 0; Back--)
        {
            if (NumBitsSet(Grid[Up][0][Back]) == 1)
                printf("| %d  ", GetNextBitPos(Grid[Up][0][Back], 0));
            else
                printf("|%03x ", Grid[Up][0][Back]);
            if (NumBitsSet(Grid[Up][1][Back]) == 1)
                printf(" %d  ", GetNextBitPos(Grid[Up][1][Back], 0));
            else
                printf("%03x ", Grid[Up][1][Back]);
            if (NumBitsSet(Grid[Up][2][Back]) == 1)
                printf(" %d |", GetNextBitPos(Grid[Up][2][Back], 0));
            else
                printf("%03x|", Grid[Up][2][Back]);
            if (NumBitsSet(Grid[Up][3][Back]) == 1)
                printf(" %d  ", GetNextBitPos(Grid[Up][3][Back], 0));
            else
                printf("%03x ", Grid[Up][3][Back]);
            if (NumBitsSet(Grid[Up][4][Back]) == 1)
                printf(" %d  ", GetNextBitPos(Grid[Up][4][Back], 0));
            else
                printf("%03x ", Grid[Up][4][Back]);
            if (NumBitsSet(Grid[Up][5][Back]) == 1)
                printf(" %d |", GetNextBitPos(Grid[Up][5][Back], 0));
            else
                printf("%03x|", Grid[Up][5][Back]);
            if (NumBitsSet(Grid[Up][6][Back]) == 1)
                printf(" %d  ", GetNextBitPos(Grid[Up][6][Back], 0));
            else
                printf("%03x ", Grid[Up][6][Back]);
            if (NumBitsSet(Grid[Up][7][Back]) == 1)
                printf(" %d  ", GetNextBitPos(Grid[Up][7][Back], 0));
            else
                printf("%03x ", Grid[Up][7][Back]);
            if (NumBitsSet(Grid[Up][8][Back]) == 1)
                printf(" %d |\n", GetNextBitPos(Grid[Up][8][Back], 0));
            else
                printf("%03x|\n", Grid[Up][8][Back]);
            if (Back == 6 || Back == 3)
                printf("+-----------+-----------+-----------+\n");
        }
        printf("+-----------+-----------+-----------+         %08d\n\n", xsum);
    }
}

void DumpPossibilities(void)
{
    int U, B;

    for (U = 0; U < GridHeight; U++)
    {
        printf("+===+===+===+===+===+===+===+===+===+\n");
        for (B = 8; B >= 0; B--)
        {
#define DO(Up,Across,Back,Num) ((Grid[Up][Across][Back]&(1<<(Num-1)))==0)?' ':(Num+'0')
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 0, B, 1), DO(U, 0, B, 2), DO(U, 0, B, 3), DO(U, 1, B, 1), DO(U, 1, B, 2), DO(U, 1, B, 3), DO(U, 2, B, 1), DO(U, 2, B, 2), DO(U, 2, B, 3));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 3, B, 1), DO(U, 3, B, 2), DO(U, 3, B, 3), DO(U, 4, B, 1), DO(U, 4, B, 2), DO(U, 4, B, 3), DO(U, 5, B, 1), DO(U, 5, B, 2), DO(U, 5, B, 3));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 6, B, 1), DO(U, 6, B, 2), DO(U, 6, B, 3), DO(U, 7, B, 1), DO(U, 7, B, 2), DO(U, 7, B, 3), DO(U, 8, B, 1), DO(U, 8, B, 2), DO(U, 8, B, 3));
            printf("H\n");
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 0, B, 4), DO(U, 0, B, 5), DO(U, 0, B, 6), DO(U, 1, B, 4), DO(U, 1, B, 5), DO(U, 1, B, 6), DO(U, 2, B, 4), DO(U, 2, B, 5), DO(U, 2, B, 6));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 3, B, 4), DO(U, 3, B, 5), DO(U, 3, B, 6), DO(U, 4, B, 4), DO(U, 4, B, 5), DO(U, 4, B, 6), DO(U, 5, B, 4), DO(U, 5, B, 5), DO(U, 5, B, 6));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 6, B, 4), DO(U, 6, B, 5), DO(U, 6, B, 6), DO(U, 7, B, 4), DO(U, 7, B, 5), DO(U, 7, B, 6), DO(U, 8, B, 4), DO(U, 8, B, 5), DO(U, 8, B, 6));
            printf("H\n");
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 0, B, 7), DO(U, 0, B, 8), DO(U, 0, B, 9), DO(U, 1, B, 7), DO(U, 1, B, 8), DO(U, 1, B, 9), DO(U, 2, B, 7), DO(U, 2, B, 8), DO(U, 2, B, 9));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 3, B, 7), DO(U, 3, B, 8), DO(U, 3, B, 9), DO(U, 4, B, 7), DO(U, 4, B, 8), DO(U, 4, B, 9), DO(U, 5, B, 7), DO(U, 5, B, 8), DO(U, 5, B, 9));
            printf("H%c%c%c|%c%c%c|%c%c%c", DO(U, 6, B, 7), DO(U, 6, B, 8), DO(U, 6, B, 9), DO(U, 7, B, 7), DO(U, 7, B, 8), DO(U, 7, B, 9), DO(U, 8, B, 7), DO(U, 8, B, 8), DO(U, 8, B, 9));
            if (B == 3 || B == 6 || B == 9)
                printf("H\n+===+===+===+===+===+===+===+===+===+\n");
            else
                printf("H\n+---+---+---+---+---+---+---+---+---+\n");
        }
    }
}

void DumpFinishedGrid(void)
{
    int Up, Back;

    for (Up = 0; Up < GridHeight; Up++)
    {
        printf("+---+---+---+\n");
        for (Back = 8; Back >= 0; Back--)
        {
            printf("|%d%d%d|%d%d%d|%d%d%d|\n",  GetNextBitPos(Grid[Up][0][Back], 0),
                                                GetNextBitPos(Grid[Up][1][Back], 0),
                                                GetNextBitPos(Grid[Up][2][Back], 0),
                                                GetNextBitPos(Grid[Up][3][Back], 0),
                                                GetNextBitPos(Grid[Up][4][Back], 0),
                                                GetNextBitPos(Grid[Up][5][Back], 0),
                                                GetNextBitPos(Grid[Up][6][Back], 0),
                                                GetNextBitPos(Grid[Up][7][Back], 0),
                                                GetNextBitPos(Grid[Up][8][Back], 0));
            if (Back == 6 || Back == 3)
                printf("+---+---+---+\n");
        }
        printf("+---+---+---+\n\n");
    }
}

// Returns true if all cells in the nineset are decided.
bool IsNinesetDecided(uint16 *NinePtrs[])
{
    int c;

    for (c = 0; c < 9; c++)
    {
        if (NumBitsSet(*NinePtrs[c]) != 1)
            return false;
    }
    return true;
}

// Returns true if was able to reduce number of possibilities for one or more cell.
bool AdvanceNinesetOnce(uint16 * NinePtrs[9])
{
    /* For every possible subset of the 9 cells, check to see if it will definitely use any particular number,
       then we can cross that number out of the list of possibilities for all other cells in the set of 9.
       E.g. if 3 of the cells have possibilities [1,2,3], [2,3] and [1,3] then we know that 1, 2 and 3 will
       be used up by these cells, so we can removed 3 from the set of possibilities for the other 6 cells.
    */

    static uint16 NinesetCopy[9]; // Static so we save the time of alloc'ing and dealloc'ing it every time this function is called.
    static uchar CellsInvolved[9];
    uint16 PossUnion;
    uchar NumCellsInvolved;
    int Cell, GrayCode;

    // Copy the cells passed in so we can check later whether they've changed.
    for (Cell = 0; Cell < 9; Cell++)
        NinesetCopy[Cell] = *NinePtrs[Cell];

    // If the whole nineset has been solved then there's nothing to do.
    if (IsNinesetDecided(NinePtrs))
    {
        return false;
    }

    // To determine all the possible subsets, go through all 9-bit values (not all-0 because at least 1 cell must be involved)
    // where each 0 or 1 indicates whether that cell is included in the subset. For speed do this as a 9-bit Gray code rather
    // than just counting from 000000001 to 111111111.
    memset(CellsInvolved, 0, sizeof(CellsInvolved));
    for (GrayCode = 0; GrayCode < 512; GrayCode++)
    {
        CellsInvolved[GrayCodeBitsToIncrement[GrayCode]] = 1 - CellsInvolved[GrayCodeBitsToIncrement[GrayCode]];
        // Create the union of possibilities of all involved cells.
        PossUnion = 0;
        for (Cell = 0; Cell < 9; Cell++)
        {
            if (CellsInvolved[Cell])
            {
                PossUnion = PossUnion | *NinePtrs[Cell];
            }
        }
        NumCellsInvolved = CellsInvolved[0] + CellsInvolved[1] + CellsInvolved[2] + CellsInvolved[3] + CellsInvolved[4]
                         + CellsInvolved[5] + CellsInvolved[6] + CellsInvolved[7] + CellsInvolved[8];
        // If 3 cells are [1,2], [1] and [1,2] then we're buggered.
        if (NumBitsSet(PossUnion) < NumCellsInvolved)
            return false;
        if (NumBitsSet(PossUnion) == NumCellsInvolved)
        {
            // Good, the involved cells use up all of their union of possibilities so we can take those possibilities out of the other cells.
            for (Cell = 0; Cell < 9; Cell++)
            {
                if (!CellsInvolved[Cell])
                    *NinePtrs[Cell] &= ~PossUnion;
            }
        }
    }
    // Retval indicates whether any of the 9 cells' possibilities have been changed, so compare with our saved copy of what they were.
    for (Cell = 0; Cell < 9; Cell++)
    {
        if (NinesetCopy[Cell] != *NinePtrs[Cell])
            return true;
    }
    return false;
}

// Returns true if was able to reduce number of possibilities for one or more cell.
bool AdvanceNineset(uint16 * NinePtrs[9])
{
    // Keep processing this nineset until it stops advancing.
    static bool Advanced;

    for (Advanced = false; AdvanceNinesetOnce(NinePtrs); )
        Advanced = true;

    return Advanced;
}

bool CheckNinesetValidity(uint16* NinePtrs[9])
{
    static bool Found[9]; // Static to save thrashing the stack.
    static int i;
    static uchar ResolvedNum;

    memset(Found, 0, sizeof(Found));

    for (i = 0; i < 9; i++)
    {
        if (*NinePtrs[i] == 0)
        {
            GridValid = false;
            return false;
        }
        ResolvedNum = GetOnlyPossibility(*NinePtrs[i]);
        if (ResolvedNum != 0)
        {
            if (Found[ResolvedNum - 1])
            {
                GridValid = false;
                return false;
            }
            Found[ResolvedNum-1] = true;
        }
    }
    return true;
}

void AdvanceCage(uchar CageNum)
{
    int Cell;
    uint16 SavedPossibilities;
    uchar Number;
    bool MadeChanges;

    // Go through every possibility in every cell in the cage, "select" that possibility and see whether the other cells can still
    // add up to the rest of the total required.
    for (MadeChanges = true; MadeChanges; )
    {
        MadeChanges = false;
        for (Cell = 0; GridValid && Cell < CageData.Cages[CageNum].NumCellsInCage; Cell++)
        {
            SavedPossibilities = Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back];
            for (Number = GetNextBitPos(SavedPossibilities, 0); GridValid && Number > 0; Number = GetNextBitPos(SavedPossibilities, Number))
            {
                // Check all other cells can form residual total.
                Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back] = 1 << (Number - 1);
                if (CanMakeTotal(CageNum) == false)
                {
                    SavedPossibilities &= ~(1 << (Number - 1));
                    MadeChanges = true;
                }
            }
            Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back] = SavedPossibilities;
            if (SavedPossibilities == 0)
            {
                GridValid = false;
            }
        }
    }
}

bool CanMakeTotal (uchar CageNum)
{
    int Cell;
    uint16 SavedPossibilities = 0;
    uchar Number, Sum;

    // Go through every possibility in every cell in the cage, "select" that possibility and see whether the other cells can still
    // add up to the rest of the total required.
    for (Cell = 0; Cell < CageData.Cages[CageNum].NumCellsInCage; Cell++)
    {
        if (NumBitsSet(Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back]) > 1)
        {
            SavedPossibilities = Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back];
            for (Number = GetNextBitPos(SavedPossibilities, 0); Number > 0; Number = GetNextBitPos(SavedPossibilities, Number))
            {
                // If were choosing a number which has already been chosen in another cell in this line or 3x3 slab then can't use it.
                if (FindNumberInLineAcross(Number, CageData.Cages[CageNum].GridCell[Cell].Back) == false
                 && FindNumberInLineBack(Number, CageData.Cages[CageNum].GridCell[Cell].Across) == false
                 && FindNumberInSlab(Number, CageData.Cages[CageNum].GridCell[Cell].Across, CageData.Cages[CageNum].GridCell[Cell].Back) == false)
                {
                    // Check all other cells can form residual total.
                    Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back] = 1 << (Number - 1);
                    if (CanMakeTotal(CageNum))
                    {
                        Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back] = SavedPossibilities;
                        return true;
                    }
                }
            }
            Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back] = SavedPossibilities;
        }
    }
    if (SavedPossibilities == 0)
    {
        // We didn't check any cells so they must have all been resolved numbers.
        for (Cell = 0, Sum = 0; Cell < CageData.Cages[CageNum].NumCellsInCage; Cell++ )
        {
            Sum += GetNextBitPos(Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back], 0);
        }
        return Sum == CageData.Cages[CageNum].CageSum;
    }
    return false;
}

bool FindNumberInLineAcross(uchar Number, uchar Back)
{
    for (uchar Across = 0; Across < 9; Across++)
    {
        if (Grid[0][Across][Back] == (1 << (Number - 1)))
            return true;
    }
    return false;
}

bool FindNumberInLineBack(uchar Number, uchar Across)
{
    for (uchar Back = 0; Back < 9; Back++)
    {
        if (Grid[0][Across][Back] == (1 << (Number - 1)))
            return true;
    }
    return false;
}

bool FindNumberInSlab(uchar Number, uchar Across, uchar Back)
{
    uchar a, b;

    for (a = (Across/3)*3; a < (Across / 3)*3+3; a++)
    {
        for (b = (Back / 3) * 3; b < (Back / 3) * 3 + 3; b++)
        {
            if (Grid[0][a][b] == (1 << (Number - 1)))
                return true;
        }
    }
    return false;
}

bool CheckCageValidity(uchar CageNum)
{
    uchar Cell, Max, Min;

    Min = Max = 0;
    for (Cell = 0; Cell < CageData.Cages[CageNum].NumCellsInCage; Cell++)
    {
        Min += GetNextBitPos(Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back], 0);
        Max += GetLastBitPos(Grid[0][CageData.Cages[CageNum].GridCell[Cell].Across][CageData.Cages[CageNum].GridCell[Cell].Back]);
    }
    // Is the sum correct for the cage or, by adding min/max numbers for unresolved cells, could it be correct?
    if (Min > CageData.Cages[CageNum].CageSum || Max < CageData.Cages[CageNum].CageSum)
    {
        GridValid = false;
    }
    return GridValid;
}

void RunUnitTests(void)
{
#ifdef RUN_TESTS
    uint16 NineToPointTo[9];
    uint16* NinePtrs[9] = { &NineToPointTo[0], &NineToPointTo[1], &NineToPointTo[2], &NineToPointTo[3], &NineToPointTo[4], &NineToPointTo[5], &NineToPointTo[6], &NineToPointTo[7], &NineToPointTo[8] };
    int TestNum, Cell;
    uchar Number;

    // Test Gray code over 16 bits is correct.
    int i, tot;
    uchar Bits[9];
    bool IntsFound[512];
    memset(Bits, 0, sizeof(Bits));
    memset(IntsFound, 0, sizeof(IntsFound));
    IntsFound[0] = true;
    for (i = 0; i < 511; i++)
    {
        Bits[GrayCodeBitsToIncrement[i]] = 1 - Bits[GrayCodeBitsToIncrement[i]];
        tot = 1 * Bits[0] + 2 * Bits[1] + 4 * Bits[2] + 8 * Bits[3] + 16 * Bits[4] + 32 * Bits[5] + 64 * Bits[6] + 128 * Bits[7] + 256 * Bits[8];
        my_assert(IntsFound[tot] == false);
        IntsFound[tot] = true;
    }

    // Test NumBitsSet().
    my_assert(NumBitsSet(0) == 0);
    my_assert(NumBitsSet(1) == 1);
    my_assert(NumBitsSet(65535) == 16);
    my_assert(NumBitsSet(8) == 1);
    my_assert(NumBitsSet(12) == 2);
    my_assert(NumBitsSet(255) == 8);
    my_assert(NumBitsSet(337) == 4);

    // Test AdvanceNineset() and CheckNinesetValidity().
    {
        CString StartPoss[][9] = {
            {
                "12389", "127", "56789", "45", "23", "12345", "789", "789", "789" // How it starts.
            },
            {
                "1", "2", "1234", "145", "256", "12346", "127", "123456789", "123789"
            },
            {
                "1", "2", "34", "567", "34567", "567", "1234", "78", "9"
            },
            {
                "1", "2", "0", "145", "256", "12346", "127", "123456789", "123789"
            },
            {
                "1", "2", "12", "9", "256", "12346", "127", "123456789", "123789"
            }
        };
        CString EndPoss[][9] = {
            {
                "123", "12", "6", "45", "23", "45", "789", "789", "789" // What it should become.
            },
            {
                "1", "2", "34", "45", "56", "346", "7", "89", "89"
            },
            {
                "1", "2", "34", "567", "567", "567", "34", "8", "9"
            },
            {
                "1", "2", "0", "45", "56", "346", "7", "3456789", "3789"
            },
            {
                "1", "2", "0", "9", "56", "346", "7", "34568", "368"
            }
        };
        bool Valid[] = {
            true,
            true,
            true,
            false,
            false
        };
        my_assert(sizeof(StartPoss) == sizeof(EndPoss) && sizeof(StartPoss) / sizeof(StartPoss[0]) == sizeof(Valid));
        CString s;
        for (TestNum = 0; TestNum < sizeof(Valid); TestNum++)
        {
            memset(NineToPointTo, 0, sizeof(NineToPointTo));
            for (Cell = 0; Cell < 9; Cell++)
            {
                for (Number = 1; Number <= 9; Number++)
                {
                    if (StartPoss[TestNum][Cell].Find(Number + '0') != -1)
                    {
                        NineToPointTo[Cell] |= (1 << (Number - 1));
                    }
                }
            }
            AdvanceNineset(NinePtrs);
            for (Cell = 0; Cell < 9; Cell++)
            {
                if (Valid[TestNum])
                {
                    my_assert(CheckNinesetValidity(NinePtrs));
                    for (Number = 1; Number <= 9; Number++)
                    {
                        my_assert((NineToPointTo[Cell] & (1 << (Number - 1))) > 0 == (EndPoss[TestNum][Cell].Find(Number + '0') != -1));
                    }
                }
                else
                {
                    my_assert(CheckNinesetValidity(NinePtrs) == false);
                }
            }
        }
    }

    // Test GetNextBitPos().
    my_assert(GetNextBitPos(1, 0) == 1);
    my_assert(GetNextBitPos(8, 0) == 4);
    my_assert(GetNextBitPos(10, 0) == 2);
    my_assert(GetNextBitPos(16, 0) == 5);
    my_assert(GetNextBitPos(30, 0) == 2);
    my_assert(GetNextBitPos(128, 0) == 8);
    my_assert(GetNextBitPos(256, 0) == 9);

    // Test GetLastBitPos().
    my_assert(GetLastBitPos(0x0000) == 0);
    my_assert(GetLastBitPos(0x0003) == 2);
    my_assert(GetLastBitPos(0x0153) == 9);
    my_assert(GetLastBitPos(0x8008) == 16);

    // Test NumberInOneThirdOfNineset().
    {
        uchar Result;
        struct {
            CString NineSet[9];
            uchar Number;
            uchar RetVal;
        } TestData[] = {
            { { "12389", "7", "5689", "45", "23", "12345", "89", "89", "89" }, 7, 0 },
            { { "12389", "127", "156789", "45", "23", "12345", "789", "789", "789" }, 1, 0 },
            { { "12389", "127", "56789", "45", "23", "2345", "789", "789", "789" }, 1, 1 },
            { { "12389", "12", "56789", "457", "237", "12345", "789", "789", "789" }, 7, 0 },
            { { "12389", "12", "5689", "457", "23", "123457", "89", "89", "89" }, 7, 2 },
            { { "1238", "12", "56789", "457", "237", "12345", "789", "789", "789" }, 9, 0 },
            { { "1238", "12", "5678", "457", "237", "12345", "789", "9", "789" }, 9, 3 }
        };
        for (TestNum = 0; TestNum < sizeof(TestData) / sizeof(TestData[0]); TestNum++)
        {
            memset(NineToPointTo, 0, sizeof(NineToPointTo));
            for (Cell = 0; Cell < 9; Cell++)
            {
                for (Number = 1; Number <= 9; Number++)
                {
                    if (TestData[TestNum].NineSet[Cell].Find(Number + '0') != -1)
                    {
                        NineToPointTo[Cell] |= (1 << (Number - 1));
                    }
                }
            }
            Result = NumberInOneThirdOfNineset(NinePtrs, TestData[TestNum].Number);
            my_assert(Result == TestData[TestNum].RetVal);
        }
    }

    // Test NumberAt159InNineset().
    {
        bool Result;
        struct {
            CString NineSet[9];
            uchar Number;
            bool RetVal;
        } TestData[] = {
            { { "12389", "7", "5689", "45", "23", "12345", "89", "89", "89" }, 7, false },
            { { "12389", "12", "56789", "45", "123", "2345", "789", "789", "1789" }, 1, false },
            { { "12389", "27", "56789", "45", "23", "2345", "789", "789", "1789" }, 1, true },
            { { "12389", "27", "56789", "45", "123", "2345", "789", "789", "1789" }, 1, true },
            { { "1238", "12", "5678", "457", "2379", "12345", "78", "8", "789" }, 9, true },
            { { "1238", "12", "567", "457", "23789", "12345", "7", "6", "789" }, 8, true }
        };
        for (TestNum = 0; TestNum < sizeof(TestData) / sizeof(TestData[0]); TestNum++)
        {
            memset(NineToPointTo, 0, sizeof(NineToPointTo));
            for (Cell = 0; Cell < 9; Cell++)
            {
                for (Number = 1; Number <= 9; Number++)
                {
                    if (TestData[TestNum].NineSet[Cell].Find(Number + '0') != -1)
                    {
                        NineToPointTo[Cell] |= (1 << (Number - 1));
                    }
                }
            }
            Result = NumberAt159InNineset(NinePtrs, TestData[TestNum].Number);
            my_assert(Result == TestData[TestNum].RetVal);
        }
    }

    // Test RotateGrid() for 2D grids.
    {
        uint16 TestGrid[9][9][9] = { {
            {11,12,13,14,15,16,17,18,19},
            {21,22,23,24,25,26,27,28,29},
            {31,32,33,34,35,36,37,38,39},
            {41,42,43,44,45,46,47,48,49},
            {51,52,53,54,55,56,57,58,59},
            {61,62,63,64,65,66,67,68,69},
            {71,72,73,74,75,76,77,78,79},
            {81,82,83,84,85,86,87,88,89},
            {91,92,93,94,95,96,97,98,99}
        }};
        uint16 ExpectedGrid[9][9] = {
            {19,29,39,49,59,69,79,89,99},
            {18,28,38,48,58,68,78,88,98},
            {17,27,37,47,57,67,77,87,97},
            {16,26,36,46,56,66,76,86,96},
            {15,25,35,45,55,65,75,85,95},
            {14,24,34,44,54,64,74,84,94},
            {13,23,33,43,53,63,73,83,93},
            {12,22,32,42,52,62,72,82,92},
            {11,21,31,41,51,61,71,81,91}
        };
        Dimensions = SUDOKU_2D;
        memcpy(Grid[0], TestGrid, sizeof(TestGrid));
        RotateGrid(true);
        my_assert(memcmp(Grid[0], ExpectedGrid, sizeof(ExpectedGrid)) == 0);
        RotateGrid(false);
        my_assert(memcmp(Grid[0], TestGrid, sizeof(TestGrid)) == 0);
    }

    // Test RotateGrid() for 3D grids.
    {
        Grid[0][0][0] = 0x0001;
        Grid[0][0][8] = 0x0002;
        Grid[0][8][0] = 0x0003;
        Grid[0][8][8] = 0x0004;
        Grid[8][0][0] = 0x0005;
        Grid[8][0][8] = 0x0006;
        Grid[8][8][0] = 0x0007;
        Grid[8][8][8] = 0x0008;
        Grid[2][2][2] = 0x0011;
        Grid[2][2][6] = 0x0012;
        Grid[2][6][2] = 0x0013;
        Grid[2][6][6] = 0x0014;
        Grid[6][2][2] = 0x0015;
        Grid[6][2][6] = 0x0016;
        Grid[6][6][2] = 0x0017;
        Grid[6][6][6] = 0x0018;
        Dimensions = SUDOKU_3D;
        RotateGrid (true);
        my_assert(Grid[0][0][0] == 0x0001);
        my_assert(Grid[0][0][8] == 0x0005);
        my_assert(Grid[0][8][0] == 0x0002);
        my_assert(Grid[0][8][8] == 0x0006);
        my_assert(Grid[8][0][0] == 0x0003);
        my_assert(Grid[8][0][8] == 0x0007);
        my_assert(Grid[8][8][0] == 0x0004);
        my_assert(Grid[8][8][8] == 0x0008);
        my_assert(Grid[2][2][2] == 0x0011);
        my_assert(Grid[2][2][6] == 0x0015);
        my_assert(Grid[2][6][2] == 0x0012);
        my_assert(Grid[2][6][6] == 0x0016);
        my_assert(Grid[6][2][2] == 0x0013);
        my_assert(Grid[6][2][6] == 0x0017);
        my_assert(Grid[6][6][2] == 0x0014);
        my_assert(Grid[6][6][6] == 0x0018);
    }

    // Test guess-stack.
    {
        int Across, Back;
        uchar NumTests;
        struct {
            uint16 TestGrid[1][9][9];
            int Across;
            int Back;
            uchar Number;
        } TestData[] = {
            { { {
                { 0x00AB, 0x0196, 0x0173, 0x00C8, 0x01A9, 0x0059, 0x01B8, 0x0193, 0x0056 },
                { 0x00FE, 0x00BC, 0x01DA, 0x00B9, 0x0098, 0x0017, 0x0023, 0x0164, 0x0152 },
                { 0x01CA, 0x019B, 0x008C, 0x019D, 0x016E, 0x007F, 0x018B, 0x017A, 0x006C },
                { 0x007D, 0x01B9, 0x00B8, 0x0117, 0x0023, 0x0164, 0x0052, 0x01B9, 0x0048 },
                { 0x0117, 0x0123, 0x0064, 0x0052, 0x01B1, 0x0123, 0x0047, 0x0187, 0x0162 },
                { 0x0033, 0x0013, 0x0042, 0x0198, 0x00BC, 0x0069, 0x018F, 0x01DA, 0x0159 },
                { 0x018C, 0x01A9, 0x0198, 0x01A8, 0x0198, 0x01F7, 0x0198, 0x00E6, 0x0098 },
                { 0x0097, 0x0116, 0x0023, 0x017C, 0x00B4, 0x0113, 0x0141, 0x0123, 0x0045 },
                { 0x0123, 0x0149, 0x0087, 0x0163, 0x0049, 0x0087, 0x0112, 0x0034, 0x00AA }
            } },
                0,4, 9
            },
            { { {
                { 0x0112, 0x007B, 0x0112, 0x0037, 0x0122, 0x0136, 0x0145, 0x0087, 0x0164 },
                { 0x0188, 0x0189, 0x018F, 0x017C, 0x01A8, 0x0099, 0x018A, 0x01E6, 0x0099 },
                { 0x00CD, 0x01B6, 0x01E7, 0x00FA, 0x019C, 0x01B8, 0x01C9, 0x017A, 0x0068 },
                { 0x019C, 0x011B, 0x0141, 0x0037, 0x0015, 0x0162, 0x0037, 0x0123, 0x0044 },
                { 0x0038, 0x01FB, 0x00C6, 0x0093, 0x0019, 0x0134, 0x0029, 0x01DA, 0x0158 },
                { 0x00A8, 0x018A, 0x0195, 0x019B, 0x0196, 0x0167, 0x003C, 0x0093, 0x005B },
                { 0x0177, 0x0022, 0x0136, 0x004D, 0x00B8, 0x019B, 0x0081, 0x01B9, 0x0045 },
                { 0x00F9, 0x0039, 0x0081, 0x017E, 0x00B2, 0x01CD, 0x00AB, 0x0064, 0x0152 },
                { 0x0023, 0x0124, 0x0198, 0x0073, 0x004A, 0x0098, 0x0176, 0x0134, 0x00A1 }
            } },
                2,8, 3
            },
            { { {
                { 0x0197, 0x00C3, 0x0141, 0x00B3, 0x0142, 0x0037, 0x0116, 0x0112, 0x0054 },
                { 0x0171, 0x0074, 0x0052, 0x0029, 0x014B, 0x008D, 0x01B9, 0x016B, 0x0083 },
                { 0x0039, 0x0089, 0x018F, 0x00BA, 0x0054, 0x0023, 0x0113, 0x016D, 0x019C },
                { 0x0026, 0x0037, 0x0112, 0x0144, 0x01A8, 0x0073, 0x0149, 0x0083, 0x00A9 },
                { 0x00AC, 0x0189, 0x01B8, 0x00A3, 0x0157, 0x013B, 0x0096, 0x0159, 0x0069 },
                { 0x0115, 0x0023, 0x0047, 0x01B7, 0x0166, 0x0147, 0x0023, 0x0128, 0x0121 },
                { 0x00FB, 0x0197, 0x0023, 0x0094, 0x005D, 0x00AE, 0x00BC, 0x0116, 0x0128 },
                { 0x00C9, 0x00DF, 0x018B, 0x016A, 0x0168, 0x01CA, 0x019B, 0x0177, 0x00CE },
                { 0x018A, 0x0187, 0x0198, 0x0196, 0x0199, 0x018C, 0x01A9, 0x00FE, 0x0088 }
            } },
                3,6, 1
            },
        };
        NumTests = sizeof(TestData) / sizeof(TestData[0]);
        for (TestNum = 0; TestNum < NumTests; TestNum++)
        {
            memcpy(Grid[0], TestData[TestNum].TestGrid, sizeof(Grid[0]));
            PushGuess(TestData[TestNum].Across, TestData[TestNum].Back, TestData[TestNum].Number);
        }
        for (TestNum = 0; TestNum < sizeof(TestData) / sizeof(TestData[0]); TestNum++)
        {
            PopGuess(&Across, &Back, &Number);
            my_assert(Across == TestData[NumTests-TestNum-1].Across
                && Back == TestData[NumTests - TestNum - 1].Back
                && Number == TestData[NumTests - TestNum - 1].Number);
            my_assert(memcmp(Grid[0], TestData[NumTests - TestNum - 1].TestGrid, sizeof(Grid[0])) == 0);
        }
    }

    // Test CanMakeTotal().
    {
        struct { uchar NumCellsInCage; uchar CageSum; bool Result; uint16 Possibilities[9]; } TestCages[] = {
            { 4, 21, true, 0x015C, 0x0158, 0x019F, 0x014B },
            { 4, 21, true, 0x01C1, 0x0104, 0x01CF, 0x019E },
            { 4, 18, true, 0x012B, 0x012D, 0x01B7, 0x0114 },
            { 4, 21, true, 0x01B9, 0x01D1, 0x01D5, 0x0168 },
            { 4, 33, true, 0x0139, 0x0120, 0x0100, 0x0164 },
            { 4, 24, true, 0x01E8, 0x01D4, 0x01CA, 0x01C6 },
            { 4, 22, true, 0x01F1, 0x013F, 0x0136, 0x01BC },
            { 4, 20, true, 0x0199, 0x0180, 0x01EA, 0x019C },
            { 4, 18, true, 0x0109, 0x0142, 0x014C, 0x01EF },
            { 4, 31, true, 0x01E2, 0x015D, 0x0161, 0x0119 },
            { 3,  9, true, 0x0116, 0x010E, 0x0132         },
            { 3, 14, true, 0x0161, 0x01DD, 0x013D         },
            { 3, 18, true, 0x01EE, 0x0190, 0x018D         },
            { 3,  7, true, 0x0139, 0x0165, 0x0132         },
            { 3, 17, true, 0x0190, 0x01F8, 0x0148         },
            { 2, 11, true, 0x01F0, 0x01F7                 },
            { 2, 16, true, 0x01D2, 0x01A6                 },
            { 2,  8, true, 0x015E, 0x01A8                 },
            { 2, 14, true, 0x0119, 0x015C                 },
            { 2,  8, true, 0x01E5, 0x0137                 }
        };
        CageData.NumCages = sizeof(TestCages) / sizeof(TestCages[0]);
        for (int cage = 0; cage < CageData.NumCages; cage++)
        {
            CageData.Cages[cage].NumCellsInCage = TestCages[cage].NumCellsInCage;
            CageData.Cages[cage].CageSum = TestCages[cage].CageSum;
            for (Cell = 0; Cell < 9; Cell++)
            {
                // Place these [up to] 4 cells into the following Grid locations: [0,0], [1,3], [2,6], [3,1]. So we can put a 7 in all of them without clashing.
                Grid[0][Cell][(3*Cell)%8] = TestCages[cage].Possibilities[Cell];
                CageData.Cages[cage].GridCell[Cell].Across = Cell;
                CageData.Cages[cage].GridCell[Cell].Back = (3 * Cell) % 8;
            }
            my_assert(CanMakeTotal(cage) == TestCages[cage].Result);
        }
    }

    // More CanMakeTotal() tests.
    {
        struct { uchar NumCellsInCage; uint16 Possibilities[3]; bool Results[27]; } TestCages[] = {
            { 3, { 0x015C, 0x0158, 0x019F }, { false,  false,  false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x01C1, 0x0104, 0x01CF }, { false,  false,  false,  false,  true,   true,   true,   true,   false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x012B, 0x012D, 0x01B7 }, { false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  true,   true  }},
            { 3, { 0x01B9, 0x01D1, 0x01D5 }, { false,  false,  true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x0139, 0x0120, 0x0100 }, { false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true,   false,  false,  true,   true,   true,   true,   true,   true,   false,  false,  true  }},
            { 3, { 0x01E8, 0x01D4, 0x01CA }, { false,  false,  false,  false,  false,  false,  false,  false,  true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x01F1, 0x013F, 0x0136 }, { false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x0199, 0x0180, 0x01EA }, { false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x0109, 0x0142, 0x014C }, { false,  false,  false,  false,  false,  true,   true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  true,   false,  true  }},
            { 3, { 0x01E2, 0x015D, 0x0161 }, { false,  false,  false,  true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x0116, 0x010E, 0x0132 }, { false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  true  }},
            { 3, { 0x0161, 0x01DD, 0x013D }, { false,  false,  true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x01EE, 0x0190, 0x018D }, { false,  false,  false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 3, { 0x0139, 0x0165, 0x0132 }, { false,  false,  false,  true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  true  }},
            { 3, { 0x0190, 0x01F8, 0x0148 }, { false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true  }},
            { 2, { 0x01F0, 0x01F7         }, { false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  false,  false,  false,  false,  false,  false,  false }},
            { 2, { 0x01D2, 0x01A6         }, { false,  false,  false,  true,   true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  false,  false,  false,  false,  false,  false,  false }},
            { 2, { 0x015E, 0x01A8         }, { false,  false,  false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  false,  false,  false,  false,  false,  false,  false }},
            { 2, { 0x0119, 0x015C         }, { false,  false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  true,   false,  true,   false,  false,  false,  false,  false,  false,  false,  false,  false }},
            { 2, { 0x01E5, 0x0137         }, { false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  false,  false,  false,  false,  false,  false,  false }}
        };
        CageData.NumCages = sizeof(TestCages) / sizeof(TestCages[0]);
        for (int cage = 0; cage < CageData.NumCages; cage++)
        {
            CageData.Cages[cage].NumCellsInCage = TestCages[cage].NumCellsInCage;
            for (Cell = 0; Cell < TestCages[cage].NumCellsInCage; Cell++)
            {
                // Place these [up to] 4 cells into the following Grid locations: [0,0], [1,3], [2,6], [3,1]. So we can put a 7 in all of them without clashing.
                Grid[0][Cell][(3 * Cell) % 8] = TestCages[cage].Possibilities[Cell];
                CageData.Cages[cage].GridCell[Cell].Across = Cell;
                CageData.Cages[cage].GridCell[Cell].Back = (3 * Cell) % 8;
            }
            for (uchar IntendedSum = 1; IntendedSum <= 27; IntendedSum++)
            {
                CageData.Cages[cage].CageSum = IntendedSum;
                my_assert(CanMakeTotal(cage) == TestCages[cage].Results[IntendedSum-1]);
            }
        }
    }

    // Test various sudokus.
    for (TestNum = 0; TestNum < NumTestSudokus; TestNum++)
    {
        uchar Up, Across, Back, StrAcross;
        SetUpGridFromSpec(&TestSudokus[TestNum].TestSudokuSpec);
        printf("Solving test sudoku %d (in range 0 to %d)...\n", TestNum, NumTestSudokus-1);
        SolveItNicely();
        my_assert(GridValid && IsGridComplete());
        for (Up = 0; Up < GridHeight; Up++)
        {
            for (Across = StrAcross = 0; Across < 9; Across++, StrAcross++)
            {
                // Skip over "|" characters in puzzle grid.
                if (Across == 3 || Across == 6)
                    StrAcross += 1;
                for (Back = 0; Back < 9; Back++)
                {
                    my_assert(Grid[Up][Across][Back] == (1 << (TestSudokus[TestNum].ExpectedGrid[Up][8 - Back][StrAcross] - '0' - 1)));
                }
            }
        }
    }
    printf("+------------------+\n");
    printf("| All tests passed |\n");
    printf("+------------------+\n\n");

#else // so not RUN_TESTS

printf("+++++++++++++++++++++++++++++++++++++++++\n");
printf("+++++++++++++++++++++++++++++++++++++++++\n");
printf("++                                     ++\n");
printf("++  Remember to turn testing back on.  ++\n");
printf("++                                     ++\n");
printf("+++++++++++++++++++++++++++++++++++++++++\n");
printf("+++++++++++++++++++++++++++++++++++++++++\n\n");

#endif // RUN_TESTS
}

bool IsGridComplete()
{
    static uchar Up, Across, Back; // Static to save constant allocs and deallocs since this function will be run many times.
    for (Up = 0; Up < GridHeight; Up++)
    {
        for (Across = 0; Across < 9; Across++)
        {
            for (Back = 0; Back < 9; Back++)
            {
                my_assert(Grid[Up][Across][Back] > 0);
                if (ExactlyOneBitSet(Grid[Up][Across][Back]) == false)
                    return false;
            }
        }
    }
    return true;
}

bool AdvanceGridAsFarAsPossible(void) // Retval indicates whether this function was able to advance the grid at all.
{
    static uint16 BackupGrid[9][9][9];
    bool MadeAnyChanges;
    uchar Up, Across, Back;
    uchar third, u, a, b, Rotations, diag;
    uint16* NinePtrs[9];
    uchar Number;

    // Keep working the grid until we can't make any more progress.
    for (MadeAnyChanges = true, GridValid = true; MadeAnyChanges && GridValid; )
    {
        memcpy(BackupGrid, Grid, sizeof(BackupGrid));

        if (Dimensions == SUDOKU_2D || Dimensions == SUDOKU_KILLER)
        {
            for (Rotations = 0; Rotations < 2; Rotations++)
            {
                // Examine each of the 9 rows going left to right.
                for (Back = 0; GridValid && Back < 9; Back++)
                {
                    // Copy row of cells into a nineset to check.
                    AssignNineset(NinePtrs, 0, 0, 0, 8, Back, Back);
                    AdvanceNineset(NinePtrs);
                    if (CheckNinesetValidity(NinePtrs))
                    {
                        // If within a line we find that all cells which still have digit i as a possibility are all in the first / second / third section
                        // of the line, then we can remove i as a possibility for all other cells in the horizontal slab those cells lie in.
                        for (Number = 1; Number <= 9; Number++)
                        {
                            third = NumberInOneThirdOfNineset(NinePtrs, Number);
                            if (third >= 1)
                            {
                                // Remove number from left/middle/right horizontal slab.
                                for (a = 3 * (third - 1); a < 3 * third; a++)
                                {
                                    for (b = 3 * (Back / 3); b < 3 * (Back / 3) + 2; b++)
                                    {
                                        if (b != Back)
                                            Grid[0][a][b] &= ~(1 << (Number - 1));
                                    }
                                }
                            }
                        }
                    }
                }

                // Examine the nine 3x3 slabs.
                for (Across = 0; Across < 9; Across += 3)
                {
                    for (Back = 0; GridValid && Back < 9; Back += 3)
                    {
                        AssignNineset(NinePtrs, 0, 0, Across, Across + 2, Back, Back + 2);
                        AdvanceNineset(NinePtrs);
                        CheckNinesetValidity(NinePtrs);
                        for (Number = 1; GridValid && Number <= 9; Number++)
                        {
                            // If within a slab we find that all occurrences of i are in cells in a (non-diagonal) line, i can be removed from all other cells in that line.
                            third = NumberInOneThirdOfNineset(NinePtrs, Number);
                            if (third >= 1)
                            {
                                // Number confined to 3 consecutive cells in nineset, so in line of constant Across but increasing Back.
                                for (b = 0; b < 9; b++)
                                {
                                    if (b < Back || b >= Back + 3)
                                        Grid[0][Across + third - 1][b] &= ~(1 << (Number - 1));
                                }
                            }
                            if (DoingDiagonals == DIAGONALS)
                            {
                                // If within a slab we find that all occurrences of i are in cells in a diagonal line which coincides
                                // with the diagonal across the whole plane, i can be removed from all other cells in that diagonal.
                                if (Across == Back && NumberAt159InNineset(NinePtrs, Number))
                                {
                                    for (a = 0; a < 9; a++)
                                    {
                                        if (a < Across || a > Across + 2)
                                            Grid[0][a][a] &= ~(1 << (Number - 1));
                                    }
                                }
                            }
                        }
                    }
                }

                // Now to check diagonals if we need to.
                if (DoingDiagonals == DIAGONALS)
                {
                    // Left near to right far. (The left far to right near diagonal will be done when the grid is rotated.)
                    Assign2DDiagonalNineset(NinePtrs);
                    AdvanceNineset(NinePtrs);
                    CheckNinesetValidity(NinePtrs);
                    // If within a diagonal we find that all occurrences of i are in the first/second/third third of the line, i can be removed from all other cells in that slab.
                    for (Number = 1; GridValid && Number <= 9; Number++)
                    {
                        third = NumberInOneThirdOfNineset(NinePtrs, Number);
                        if (third >= 1)
                        {
                            for (a = 3 * (third - 1); a < 3 * (third - 1) + 2; a++)
                            {
                                for (b = 3 * (third - 1); b < 3 * (third - 1) + 2; b++)
                                {
                                    if (a != b)
                                        Grid[0][a][b] &= ~(1 << (Number - 1));
                                }
                            }
                        }
                    }
                }
                RotateGrid(Rotations == 0);
            }
            if (Dimensions == SUDOKU_KILLER)
            {
                // Go through every cage to see if we can reduce the possibilities within its cells.
                for (int Cage = 0; Cage < CageData.NumCages; Cage++)
                {
                    AdvanceCage(Cage);
                    if (CheckCageValidity(Cage) == false)
                        break;
                }
            }
        }
        else // so 3D
        {
            for (Rotations = 0; Rotations < 3; Rotations++)
            {
                // Examine each of the 81 rows going left to right.
                for (Up = 0; Up < 9; Up++)
                {
                    for (Back = 0; GridValid && Back < 9; Back++)
                    {
                        // Copy row of cells into a nineset to check.
                        AssignNineset(NinePtrs, Up, Up, 0, 8, Back, Back);
                        AdvanceNineset(NinePtrs);
                        if (CheckNinesetValidity(NinePtrs))
                        {
                            // If within a line we find that all cells which still have digit i as a possibility are all in the first / second / third section
                            // of the line, then we can remove i as a possibility for all other cells in the horizontal slab those cells lie in.
                            for (Number = 1; Number <= 9; Number++)
                            {
                                third = NumberInOneThirdOfNineset(NinePtrs, Number);
                                if (third >= 1)
                                {
                                    // Remove number from relevant 3x3 horizontal and vertical slabs.
                                    for (a = 3 * (third - 1); a < 3 * third; a++)
                                    {
                                        for (b = 3 * (Back / 3); b < 3 * (Back / 3) + 2; b++)
                                        {
                                            if (b != Back)
                                                Grid[Up][a][b] &= ~(1 << (Number - 1));
                                        }
                                        for (u = 3 * (Up / 3); u < 3 * (Up / 3) + 2; u++)
                                        {
                                            if (u != Up)
                                                Grid[u][a][Back] &= ~(1 << (Number - 1));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Examine the nine 3x3 horizontal slabs.
                for (Up = 0; Up < 9; Up++)
                {
                    for (Across = 0; Across < 9; Across += 3)
                    {
                        for (Back = 0; GridValid && Back < 9; Back += 3)
                        {
                            AssignNineset(NinePtrs, Up, Up, Across, Across + 2, Back, Back + 2);
                            AdvanceNineset(NinePtrs);
                            CheckNinesetValidity(NinePtrs);
                            for (Number = 1; Number <= 9; Number++)
                            {
                                // If within a slab we find that all occurrences of i are in cells in a (non-diagonal) line, i can be removed from all other cells in that line.
                                third = NumberInOneThirdOfNineset(NinePtrs, Number);
                                if (third >= 1)
                                {
                                    // Number confined to 3 consecutive cells in nineset, so in line of constant Across but increasing Back.
                                    for (b = 0; b < 9; b++)
                                    {
                                        if (b < Back || b >= Back + 3)
                                            Grid[Up][Across + third - 1][b] &= ~(1 << (Number - 1));
                                    }
                                }
                                // And check for lines going the other way. None of the other rotations out of the 3 rotations for a 3D sudoku will check this.
                                third = NumberEveryThreeInNineset(NinePtrs, Number); // E.g. positions 0,3,6 or 1,4,7 or 2,5,8.
                                if (third >= 1)
                                {
                                    // Number confined to 3 consecutive cells in nineset, so in line of constant Back but increasing Across.
                                    for (a = 0; a < 9; a++)
                                    {
                                        if (a < Across || a >= Across + 3)
                                            Grid[Up][a][Back + third - 1] &= ~(1 << (Number - 1));
                                    }
                                }
                            }
                        }
                    }
                }
                RotateGrid(true);
            }
            // Now to check diagonals if we need to.
            if (DoingDiagonals == DIAGONALS)
            {
                my_assert(false); // Diagonals not yet tested on 3D sudokus cos I can't find one online.
                for (diag = 0; diag < 3; diag++)
                {
                    Assign3DDiagonalNineset(NinePtrs, diag); // E.g. 0 means Bottom left near to top right far.
                    AdvanceNineset(NinePtrs);
                    CheckNinesetValidity(NinePtrs);
                }
            }
        }

        MadeAnyChanges = (memcmp(BackupGrid, Grid, sizeof(BackupGrid)) != 0);
    }

    return MadeAnyChanges;
}

void TryGuessing(void)
{
    // Need guesswork. If this program is doing its job, this can only be because there are multiple solutions and therefore you can't solve it using deductive reasoning alone.
    // I want to find all solutions because that proves that guesswork was needed. If I explore all guesses and still find only 1 solution then the program should have been
    // able to find it without guesswork, so that will tell me I need to work on my program.
    static uint16 BackupGrid[9][9][9];
    uchar Number;
    int Across, Back;

    my_assert(GridValid && !IsGridComplete());

    Across = Back = 0;
    Number = 0;
    for (; Across < 9 && Back < 9; )
    {
        Number = GetNextBitPos(Grid[0][Across][Back], Number);
        if (Number == 0)
        {
            if (Across == 0 && Back == 0)
            {
                PopOffAllGuesses();
                GridValid = true;
                break;
            }
            PopGuess(&Across, &Back, &Number);
            Grid[0][Across][Back] &= ~(1 << (Number - 1));
            GridValid = true;
            AdvanceGridAsFarAsPossible();
        }
        else
        {
            PushGuess(Across, Back, Number);
            Grid[0][Across][Back] = 1 << (Number - 1);
            AdvanceGridAsFarAsPossible();
            if (GridValid)
            {
                if (Back == 8) { Back = 0; Across += 1; }
                else Back += 1;
                Number = 0;
            }
            else
            {
                PopGuess(&Across, &Back, &Number);
                GridValid = true;
            }
        }
    }
    PopOffAllGuesses();
}

int main()
{
    RunUnitTests();

    // Copy puzzle into Grid. All unspecified cells in the whole grid must have all of their possibilities set.
    SetUpGridFromSpec (&TheSudoku);

//#define EXHAUSTIVE_LOOPING
#ifdef EXHAUSTING_COMBINATIONS

    // I've only implemented exhaustive checking of combinations for normal 2D sudokus.
    my_assert(Dimensions == SUDOKU_2D);

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("++                                                 ++\n");
    printf("++  Remember to turn off exhaustive permutations.  ++\n");
    printf("++                                                 ++\n");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    ExhaustivelyCheckCombinations();

#else // so not EXHAUSTING_COMBINATIONS

    SolveItNicely();

#endif
}

void SolveItNicely(void) // Nicely means you can do some guessing if you need to but you're not doing plain exhaustive checking of all combinations.
{
    // Assume the puzzle setter didn't provide duff data. I'd probably notice when I entered the puzzle in the first place anyway.
    GridValid = true;
    AdvanceGridAsFarAsPossible();

    // Look at the results.
    if (GridValid)
    {
        if (IsGridComplete())
        {
            printf("SUCCESS! No guessing needed.\n");
            DumpFinishedGrid();
        }
        else
        {
            printf("Could not solve it deductively so will try guessing.\n");
            DumpPossibilities();
            TryGuessing();
            if (GridValid)
            {
                if (IsGridComplete())
                {
                    printf("SUCCESS!\n");
                    DumpFinishedGrid();
                }
                else
                {
                    printf("Can't solve even with guesswork. Shouldn't happen.\n");
                    DumpUnfinishedGrid();
                }
            }
            else
            {
                printf("Arrived at invalid grid. The starting numerals led us here so they must have been duff.\n");
                DumpUnfinishedGrid();
            }
        }
    }
    else
    {
        printf("Arrived at invalid grid. The starting numerals led us here so they must have been duff.\n");
        DumpUnfinishedGrid();
    }
}

void SkipBack(int& a, int& b, uchar Skip)
{
    if (b < Skip)
    {
        b += 9 - Skip;
        a--;
    }
    else
    {
        b -= Skip;
    }
}

#ifdef EXHAUSTIVE_LOOPING
bool ExhaustivelyCheckCombinations(void)
{
    uchar Fixed[9][9]; // indexed as Fixed[Across][Back]
    uchar GuessGrid[9][9];
    int Across, Back, AStart, AEnd, BStart, BEnd, LastUnfixedAcross, LastUnfixedBack;
    uchar StrAcross, Number;

    // Set up fixed numbers.
    for (Back = 0; Back < 9; Back++)
    {
        for (StrAcross = Across = 0; Across < 9; Across++, StrAcross++)
        {
            // Skip over "|" characters in puzzle grid.
            if (Across == 3 || Across == 6)
                StrAcross += 1;
            if (InitialGrid[Back][StrAcross] == ' ')
                Fixed[Across][8 - Back] = 0;
            else
                Fixed[Across][8 - Back] = InitialGrid[Back][StrAcross] - '0';
        }
    }

    // Find coords of last cell which is not fixed and which therefore needs a guess in it.
    for (Across = 8; Across >= 0; Across--)
    {
        for (Back = 8; Back >= 0; Back--)
        {
            if (Fixed[Across][Back] == 0)
            {
                LastUnfixedAcross = Across;
                LastUnfixedBack = Back;
                Across = Back = -1; // End the looping.
            }
        }
    }

    // Make exhaustive guesses.
    memcpy(GuessGrid, Fixed, sizeof(GuessGrid));
    for (Across = 0; Across < 9; Across++)
    {
        for (Back = 0; Back < 9; Back++)
        {
            if (Fixed[Across][Back] == 0)
            {
                for (Number = 1 + GuessGrid[Across][Back]; Number <= 9; Number++)
                {
                    // Check this number not already used in row, col or slab.
                    if (GuessGrid[0][Back] != Number && GuessGrid[1][Back] != Number && GuessGrid[2][Back] != Number
                     && GuessGrid[3][Back] != Number && GuessGrid[4][Back] != Number && GuessGrid[5][Back] != Number
                     && GuessGrid[6][Back] != Number && GuessGrid[7][Back] != Number && GuessGrid[8][Back] != Number)
                    {
                        if (GuessGrid[Across][0] != Number && GuessGrid[Across][1] != Number && GuessGrid[Across][2] != Number
                         && GuessGrid[Across][3] != Number && GuessGrid[Across][4] != Number && GuessGrid[Across][5] != Number
                         && GuessGrid[Across][6] != Number && GuessGrid[Across][7] != Number && GuessGrid[Across][8] != Number)
                        {
                            AStart = (Across / 3) * 3;
                            AEnd = AStart + 2;
                            BStart = (Back / 3) * 3;
                            BEnd = BStart + 2;
                            if (GuessGrid[AStart][BStart] != Number && GuessGrid[AStart][BStart + 1] != Number && GuessGrid[AStart][BStart + 2] != Number
                             && GuessGrid[AStart + 1][BStart] != Number && GuessGrid[AStart + 1][BStart + 1] != Number && GuessGrid[AStart + 1][BStart + 2] != Number
                             && GuessGrid[AStart + 2][BStart] != Number && GuessGrid[AStart + 2][BStart + 1] != Number && GuessGrid[AStart + 2][BStart + 2] != Number)
                            {
                                // Check putting this number here doesn't screw up either diagonal.
                                if (DoingDiagonals != DIAGONALS ||
                                    (
                                        (Across != Back       || (  GuessGrid[0][0] != Number && GuessGrid[1][1] != Number && GuessGrid[2][2] != Number
                                                                 && GuessGrid[3][3] != Number && GuessGrid[4][4] != Number && GuessGrid[5][5] != Number
                                                                 && GuessGrid[6][6] != Number && GuessGrid[7][7] != Number && GuessGrid[8][8] != Number))
                                     && (Across != (8 - Back) || (  GuessGrid[0][8] != Number && GuessGrid[1][7] != Number && GuessGrid[2][6] != Number
                                                                 && GuessGrid[3][5] != Number && GuessGrid[4][4] != Number && GuessGrid[5][3] != Number
                                                                 && GuessGrid[6][2] != Number && GuessGrid[7][1] != Number && GuessGrid[8][0] != Number))
                                    ))
                                {
                                    // Valid guess.
                                    GuessGrid[Across][Back] = Number;
                                    if (Across == LastUnfixedAcross && Back == LastUnfixedBack)
                                    {
                                        printf("From exhaustive guesswork...\n");
                                        printf("+---------+---------+---------+\n");
                                        for (Back = 8; Back >= 0; Back--)
                                        {
                                            if (GuessGrid[0][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf("| %d ", GuessGrid[0][Back]);
                                            if (GuessGrid[1][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d ", GuessGrid[1][Back]);
                                            if (GuessGrid[2][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d |", GuessGrid[2][Back]);
                                            if (GuessGrid[3][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d ", GuessGrid[3][Back]);
                                            if (GuessGrid[4][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d ", GuessGrid[4][Back]);
                                            if (GuessGrid[5][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d |", GuessGrid[5][Back]);
                                            if (GuessGrid[6][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d ", GuessGrid[6][Back]);
                                            if (GuessGrid[7][Back] == 0)
                                                printf("|   ");
                                            else
                                                printf(" %d ", GuessGrid[7][Back]);
                                            if (GuessGrid[8][Back] == 0)
                                                printf("   |\n");
                                            else
                                                printf(" %d |\n", GuessGrid[8][Back]);
                                            if (Back == 6 || Back == 3)
                                                printf("+---------+---------+---------+\n");
                                        }
                                        printf("+---------+---------+---------+\n\n");
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                if (Number == 10)
                {
                    // All guesses in this cell have been explored so remove all guesses in this cell.
                    GuessGrid[Across][Back] = 0;
                    // Look again at the previous cell. Must bodge Across and Back so that they become what we want after the loop increments.
                    // This also means skipping back past any fixed cells.
                    for (SkipBack(Across, Back, 1); ; SkipBack(Across, Back, 1))
                    {
                        if (Across < 0)
                        {
                            // Already looking at[0][0] so can't go back any further.
                            printf("No more solutions.\n");
                            Across = Back = 9; // to cause the looping to stop.
                            return false;
                        }
                        else
                        {
                            if (Fixed[Across][Back] == 0)
                            {
                                SkipBack(Across, Back, 1);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
#endif // EXHAUSTIVE_LOOPING
