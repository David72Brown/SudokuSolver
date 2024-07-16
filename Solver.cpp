#include "pch.h"
#include <iostream>
#include <atlstr.h>
#include "TestsAndMisc.h"
#include "Solver.h"

using namespace std;

static const uchar GrayCodeBitsToIncrement[] = {
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,8,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
};

static bool ExactlyOneBitSet(uint16 x)
{
    return x > 0 && ((x & (x - 1)) == 0);
}

/*  Dir means:
        0   Start from cell (*Across, *Forward), check that cell first, then go to increasing "across" and "forward" cells to find
            next undecided cell.
        +1  Start from the cell to the right of this one (i.e. further across) and start the search from there.
        -1  Start from the cell to the left and start the search from there, searching backwards towards the top-left corner.
*/
static bool FindNextUndecidedCellInLayer(uint16 Layer[][9], int* Across, int* Forward, char Dir)
{
    // Set start cell and then re-purpose Dir to be +1 (go forward) or -1 (backwards).
    if (Dir == +1)
        *Forward += 1;
    else if (Dir == -1)
        *Forward -= 1;
    else
        Dir = +1;

    for ( ; *Across >= 0 && *Across < 9; *Across += Dir)
    {
        for ( ; *Forward >= 0 && *Forward < 9; *Forward += Dir)
        {
            if (!ExactlyOneBitSet(Layer[*Across][*Forward]))
            {
                return true;
            }
        }
        *Forward = (Dir == +1) ? 0 : 8;
    }
    return false;
}

cSolver::cSolver(void)
{
    fill_n(&CellContents[0][0][0], 9 * 9 * 9, ALL_POSSIBILITIES);
}

// Returns 0 if more than one possibility remains.
uchar cSolver::GetOnlyPossibility(uint16 CellPossibilities)
{
    uchar n;

    for (n = 1; n <= 9; n++)
    {
        if ((CellPossibilities & 1) == 1)
        {
            if (CellPossibilities == 1)
                return n;
            else
                return 0;
        }
        CellPossibilities = CellPossibilities >> 1;
    }
    return 0;
}

uchar cSolver::NumPossibilities(uint16 x)
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

// Returns the next (i.e. larger) possibility e.g. 0x04 if number 3 is the next possible number.
uint16 cSolver::GetNextPossibility(uint16 Possibilities, uint16 StartPossibility)
{
    my_assert(Possibilities != 0 && (StartPossibility >= 0x001 && StartPossibility <= 0x0100) || StartPossibility == ALL_POSSIBILITIES);

    if (StartPossibility == ALL_POSSIBILITIES)
        StartPossibility = 1;
    else
        StartPossibility = StartPossibility << 1;

    // Check there are some possibility bits set at or to the left of the bit set in StartPossibility.
    if (Possibilities / StartPossibility > 0)
    {
        for (; (Possibilities & StartPossibility) == 0; )
        {
            StartPossibility = StartPossibility << 1;
        }
        return StartPossibility;
    }
    return ALL_POSSIBILITIES;
}

/* This fn ignores cases where a number appears just once in the nineset because that will cause that
   cell to be resolved, so the number will be taken out of the rest of the line/slab anyway. This fn
   only finds 2 or 3 instances of a number within one third of the nineset. */
uchar cSolver::NumberInOneThirdOfNineset(uint16* NinePtrs[9], uchar Number)
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
uchar cSolver::NumberEveryThreeInNineset(uint16* NinePtrs[9], uchar Number)
{
    uint16* RejiggedNinePtrs[9] = { NinePtrs[0], NinePtrs[3], NinePtrs[6], NinePtrs[1], NinePtrs[4], NinePtrs[7], NinePtrs[2], NinePtrs[5], NinePtrs[8] };

    return NumberInOneThirdOfNineset(RejiggedNinePtrs, Number);
}

bool cSolver::NumberAt159InNineset(uint16* NinePtrs[9], uchar Number)
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

void cSolver::AssignNineset(uint16 *NinePtrs[9], uchar UpFirst, uchar UpLast, uchar AcrossFirst, uchar AcrossLast, uchar FwdFirst, uchar FwdLast)
{
    uchar Up, Across, Forward;
    uchar Count; // We should always assign exactly 9 cells.

    Count = 0;
    for (Up = UpFirst; Up <= UpLast; Up++)
    {
        for (Across = AcrossFirst; Across <= AcrossLast; Across++)
        {
            for (Forward = FwdFirst; Forward <= FwdLast; Forward++)
            {
                my_assert(Count < 9);
                NinePtrs[Count++] = &CellContents[Up][Across][Forward];
            }
        }
    }
    my_assert(Count == 9);
}

void cSolver::Assign2DDiagonalNineset(uint16 *NinePtrs[9])
{
    int Cell;

    for (Cell = 0; Cell < 9; Cell++)
    {
        NinePtrs[Cell] = &CellContents[0][Cell][Cell];
    }
}

void cSolver::Assign3DDiagonalNineset(uint16* NinePtrs[9], uchar diag)
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
                NinePtrs[Up] = &CellContents[Up][x][x];
                break;
            case 1:
                // Bottom left far to top right near.
                NinePtrs[Up] = &CellContents[Up][x][8 - x];
                break;
            case 2:
                // Bottom right near to top left far.
                NinePtrs[Up] = &CellContents[Up][8 - x][x];
                break;
            case 3:
                NinePtrs[Up] = &CellContents[Up][8 - x][8 - x];
                // Bottom right far to top left near.
                break;
            }
        }
    }
}

void cSolver::RotateGrid(bool Fwd)
{
    static uint16 RotatedGrid[9][9][9];
    int Up, Across, Forward;

    if (Dimensions == SUDOKU_2D)
    {
        for (Across = 0; Across < 9; Across++)
        {
            for (Forward = 0; Forward < 9; Forward++)
            {
                if (Fwd)
                    RotatedGrid[0][8 - Forward][Across] = CellContents[0][Across][Forward];
                else
                    RotatedGrid[0][Across][Forward] = CellContents[0][8 - Forward][Across];
            }
        }
        memcpy(CellContents, RotatedGrid[0], sizeof(RotatedGrid[0]));
    }
    else
    {
        // Don't think we ever rotate a 3D grid backwards.
        my_assert(Fwd);
        for (Up = 0; Up < 9; Up++)
        {
            for (Across = 0; Across < 9; Across++)
            {
                for (Forward = 0; Forward < 9; Forward++)
                {
                    RotatedGrid[Across][Forward][Up] = CellContents[Up][Across][Forward];
                }
            }
        }
        memcpy(CellContents, RotatedGrid, sizeof(RotatedGrid));
    }
}

// Returns true if all cells in the nineset are decided.
bool cSolver::IsNinesetDecided(uint16 *NinePtrs[])
{
    int c;

    for (c = 0; c < 9; c++)
    {
        if (NumPossibilities(*NinePtrs[c]) != 1)
            return false;
    }
    return true;
}

// Returns true if was able to reduce number of possibilities for one or more cell.
bool cSolver::AdvanceNinesetOnce(uint16 * NinePtrs[9])
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
        // If 3 cells are [1,2], [1] and [1,2] then that's not valid.
        if (NumPossibilities(PossUnion) < NumCellsInvolved)
        {
            ContentsValid = false;
            return false;
        }
        if (NumPossibilities(PossUnion) == NumCellsInvolved)
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
bool cSolver::AdvanceNineset(uint16 * NinePtrs[9])
{
    // Keep processing this nineset until it stops advancing.
    bool Advanced;

    for (Advanced = false; AdvanceNinesetOnce(NinePtrs); )
        Advanced = true;

    return Advanced;
}

bool cSolver::CheckNinesetValidity(uint16* NinePtrs[9])
{
    static bool Found[9]; // Static to save thrashing the stack.
    int i;
    uchar ResolvedNum;

    memset(Found, 0, sizeof(Found));

    for (i = 0; i < 9; i++)
    {
        if (*NinePtrs[i] == 0)
        {
            ContentsValid = false;
            return false;
        }
        ResolvedNum = GetOnlyPossibility(*NinePtrs[i]);
        if (ResolvedNum != 0)
        {
            if (Found[ResolvedNum - 1])
            {
                ContentsValid = false;
                return false;
            }
            Found[ResolvedNum-1] = true;
        }
    }
    return true;
}

bool cSolver::IsGridComplete()
{
    uchar Up, Across, Forward;
    for (Up = 0; Up < (Dimensions == SUDOKU_2D ? 1 : 9); Up++)
    {
        for (Across = 0; Across < 9; Across++)
        {
            for (Forward = 0; Forward < 9; Forward++)
            {
                my_assert(CellContents[Up][Across][Forward] > 0);
                if (ExactlyOneBitSet(CellContents[Up][Across][Forward]) == false)
                    return false;
            }
        }
    }
    return true;
}

bool cSolver::AdvanceGridAsFarAsPossible(void) // Retval indicates whether this function was able to advance the grid at all.
{
    static uint16 BackupGrid[9][9][9];
    bool MadeAnyChanges;
    uchar Up, Across, Forward;
    uchar third, u, a, b, Rotations, diag;
    uint16* NinePtrs[9];
    uchar Number;

    // Keep working the grid until we can't make any more progress.
    for (MadeAnyChanges = true, ContentsValid = true; MadeAnyChanges && ContentsValid; )
    {
        memcpy(BackupGrid, CellContents, sizeof(BackupGrid));

        if (Dimensions == SUDOKU_2D)
        {
            for (Rotations = 0; Rotations < 2; Rotations++)
            {
                // Examine each of the 9 rows going left to right.
                for (Forward = 0; ContentsValid && Forward < 9; Forward++)
                {
                    // Copy row of cells into a nineset to check.
                    AssignNineset(NinePtrs, 0, 0, 0, 8, Forward, Forward);
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
                                    for (b = 3 * (Forward / 3); b < 3 * (Forward / 3) + 2; b++)
                                    {
                                        if (b != Forward)
                                        {
                                            CellContents[0][a][b] &= ~(1 << (Number - 1));
                                            if (CellContents[0][a][b] == 0)
                                            {
                                                ContentsValid = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Examine the nine 3x3 slabs.
                for (Across = 0; Across < 9; Across += 3)
                {
                    for (Forward = 0; ContentsValid && Forward < 9; Forward += 3)
                    {
                        AssignNineset(NinePtrs, 0, 0, Across, Across + 2, Forward, Forward + 2);
                        AdvanceNineset(NinePtrs);
                        CheckNinesetValidity(NinePtrs);
                        for (Number = 1; ContentsValid && Number <= 9; Number++)
                        {
                            // If within a slab we find that all occurrences of i are in cells in a (non-diagonal) line, i can be removed from all other cells in that line.
                            third = NumberInOneThirdOfNineset(NinePtrs, Number);
                            if (third >= 1)
                            {
                                // Number confined to 3 consecutive cells in nineset, so in line of constant Across but increasing Forward.
                                for (b = 0; b < 9; b++)
                                {
                                    if (b < Forward || b >= Forward + 3)
                                    {
                                        CellContents[0][Across + third - 1][b] &= ~(1 << (Number - 1));
                                        if (CellContents[0][Across + third - 1][b] == 0)
                                        {
                                            ContentsValid = false;
                                        }
                                    }
                                }
                            }
                            if (Diagonals)
                            {
                                // If within a slab we find that all occurrences of i are in cells in a diagonal line which coincides
                                // with the diagonal across the whole plane, i can be removed from all other cells in that diagonal.
                                if (Across == Forward && NumberAt159InNineset(NinePtrs, Number))
                                {
                                    for (a = 0; a < 9; a++)
                                    {
                                        if (a < Across || a > Across + 2)
                                        {
                                            CellContents[0][a][a] &= ~(1 << (Number - 1));
                                            if (CellContents[0][a][a] == 0)
                                            {
                                                ContentsValid = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Now to check diagonals if we need to.
                if (Diagonals)
                {
                    // Left near to right far. (The left far to right near diagonal will be done when the grid is rotated.)
                    Assign2DDiagonalNineset(NinePtrs);
                    AdvanceNineset(NinePtrs);
                    CheckNinesetValidity(NinePtrs);
                    // If within a diagonal we find that all occurrences of i are in the first/second/third third of the line, i can be removed from all other cells in that slab.
                    for (Number = 1; ContentsValid && Number <= 9; Number++)
                    {
                        third = NumberInOneThirdOfNineset(NinePtrs, Number);
                        if (third >= 1)
                        {
                            for (a = 3 * (third - 1); a < 3 * (third - 1) + 2; a++)
                            {
                                for (b = 3 * (third - 1); b < 3 * (third - 1) + 2; b++)
                                {
                                    if (a != b)
                                    {
                                        CellContents[0][a][b] &= ~(1 << (Number - 1));
                                        if (CellContents[0][a][b] == 0)
                                        {
                                            ContentsValid = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                RotateGrid(Rotations == 0);
            }
        }
        else // so 3D
        {
            for (Rotations = 0; Rotations < 3; Rotations++)
            {
                // Examine each of the 81 rows going left to right.
                for (Up = 0; Up < 9; Up++)
                {
                    for (Forward = 0; ContentsValid && Forward < 9; Forward++)
                    {
                        // Copy row of cells into a nineset to check.
                        AssignNineset(NinePtrs, Up, Up, 0, 8, Forward, Forward);
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
                                        for (b = 3 * (Forward / 3); b < 3 * (Forward / 3) + 2; b++)
                                        {
                                            if (b != Forward)
                                            {
                                                CellContents[Up][a][b] &= ~(1 << (Number - 1));
                                                if (CellContents[Up][a][b] == 0)
                                                {
                                                    ContentsValid = false;
                                                }
                                            }
                                        }
                                        for (u = 3 * (Up / 3); u < 3 * (Up / 3) + 2; u++)
                                        {
                                            if (u != Up)
                                            {
                                                CellContents[u][a][Forward] &= ~(1 << (Number - 1));
                                                if (CellContents[u][a][Forward] == 0)
                                                {
                                                    ContentsValid = false;
                                                }
                                            }
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
                        for (Forward = 0; ContentsValid && Forward < 9; Forward += 3)
                        {
                            AssignNineset(NinePtrs, Up, Up, Across, Across + 2, Forward, Forward + 2);
                            AdvanceNineset(NinePtrs);
                            CheckNinesetValidity(NinePtrs);
                            for (Number = 1; Number <= 9; Number++)
                            {
                                // If within a slab we find that all occurrences of i are in cells in a (non-diagonal) line, i can be removed from all other cells in that line.
                                third = NumberInOneThirdOfNineset(NinePtrs, Number);
                                if (third >= 1)
                                {
                                    // Number confined to 3 consecutive cells in nineset, so in line of constant Across but increasing Forward.
                                    for (b = 0; b < 9; b++)
                                    {
                                        if (b < Forward || b >= Forward + 3)
                                        {
                                            CellContents[Up][Across + third - 1][b] &= ~(1 << (Number - 1));
                                            if (CellContents[Up][Across + third - 1][b] == 0)
                                            {
                                                ContentsValid = false;
                                            }
                                        }
                                    }
                                }
                                // And check for lines going the other way. None of the other rotations out of the 3 rotations for a 3D sudoku will check this.
                                third = NumberEveryThreeInNineset(NinePtrs, Number); // E.g. positions 0,3,6 or 1,4,7 or 2,5,8.
                                if (third >= 1)
                                {
                                    // Number confined to 3 consecutive cells in nineset, so in line of constant Forward but increasing Across.
                                    for (a = 0; a < 9; a++)
                                    {
                                        if (a < Across || a >= Across + 3)
                                        {
                                            CellContents[Up][a][Forward + third - 1] &= ~(1 << (Number - 1));
                                            if (CellContents[Up][a][Forward + third - 1] == 0)
                                            {
                                                ContentsValid = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                RotateGrid(true);
            }
            // Now to check diagonals if we need to.
            if (Diagonals)
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

        MadeAnyChanges = (memcmp(BackupGrid, CellContents, sizeof(BackupGrid)) != 0);
    }

    return MadeAnyChanges;
}

void cSolver::TryGuessing(void)
{
    // Need guesswork. If this program is doing its job, this can only be because there are multiple solutions and therefore you can't solve it using deductive reasoning alone.
    static uint16 BackupGrid[9][9][9];
    uchar Number;
    int Across, Forward;

    my_assert(ContentsValid && !IsGridComplete());

    NumGuesses = 0;
    Number = 0;
    for (Across = Forward = 0; Across < 9 && Forward < 9; )
    {
        if (ExactlyOneBitSet(CellContents[0][Across][Forward]))
        {
            if (Forward == 8) { Forward = 0; Across += 1; } else { Forward += 1; }
            Number = 0;
            my_assert(ContentsValid);
        }
        else
        {
            Number = GetNextBitPos(CellContents[0][Across][Forward], Number);
            if (Number == 0)
            {
                if (Across == 0 && Forward == 0)
                {
                    break;
                }
                PopGuess(&Across, &Forward, &Number);
                my_assert(ContentsValid);
            }
            else
            {
                PushGuess(Across, Forward, Number);
                CellContents[0][Across][Forward] = 1 << (Number - 1);
                AdvanceGridAsFarAsPossible();
                if (ContentsValid)
                {
                    if (Forward == 8) { Forward = 0; Across += 1; } else { Forward += 1; }
                    Number = 0;
                }
                else
                {
                    PopGuess(&Across, &Forward, &Number);
                    ContentsValid = true;
                }
            }
        }
    }
    PopOffAllGuesses();
}

eResult cSolver::SolveIt(void)
{
    // Begin by assuming the puzzle setter didn't provide duff data. We'll find out later if that's not true.
    ContentsValid = true;
    AdvanceGridAsFarAsPossible();

    // Look at the results.
    if (ContentsValid)
    {
        if (IsGridComplete())
        {
            // Success. No guessing needed.
            return SUCCESS;
        }
        else
        {
            // Could not solve it deductively so will try guessing.
            TryGuessing();
            if (ContentsValid)
            {
                if (IsGridComplete())
                {
                    return SUCCESS;
                }
                else
                {
                    // Can't solve even with guesswork. Shouldn't happen.
                    return FAILURE;
                }
            }
            else
            {
                // Arrived at invalid grid. The starting numerals led us here so they must have been duff.
                return BAD_INITIAL_DATA;
            }
        }
    }
    else
    {
        // Arrived at invalid grid. The starting numerals led us here so they must have been duff.
        return BAD_INITIAL_DATA;
    }
}

void cSolver::SetCellContents(uchar Up, uchar Forward, uchar Across, uchar Number)
{
    if (Number >= 1 && Number <= 9)
    {
        CellContents[Up][Forward][Across] = 1 << (Number - 1);
        my_assert(CellContents[Up][Forward][Across] != 0);
    }
    else
    {
        CellContents[Up][Forward][Across] = ALL_POSSIBILITIES;
    }
}

uint16 cSolver::GetCellContents(uchar Up, uchar Forward, uchar Across)
{
    uchar Shifts;

    if (NumPossibilities(CellContents[Up][Forward][Across]) == 1)
    {
        for (Shifts = 0; ; Shifts++)
        {
            if ((CellContents[Up][Forward][Across] >> Shifts) == 1)
                return Shifts + 1;
        }
    }
    return ALL_POSSIBILITIES;
}

void cSolver::ClearCellContents(void)
{
    fill_n(&CellContents[0][0][0], 9 * 9 * 9, ALL_POSSIBILITIES);
}

void cSolver::PushGuess(int Across, int Forward, uchar Number)
{
    int a, f, numbits, lsbytenum;

    my_assert(NumGuesses < MAX_GUESSES);

    // Store the grid.
    memset(Compressed2DGrids[NumGuesses], 0, sizeof(Compressed2DGrids[0]));
    numbits = 8;
    for (a = 0; a < 9; a++)
    {
        for (f = 0; f < 9; f++)
        {
            lsbytenum = (9 * a + f) * 9 / 8;
            Compressed2DGrids[NumGuesses][lsbytenum]     |= (CellContents[0][a][f] & ((1 << numbits) - 1)) << (8 - numbits);
            Compressed2DGrids[NumGuesses][lsbytenum + 1] |= CellContents[0][a][f] >> numbits;
            numbits = (numbits == 1) ? 8 : numbits - 1;
        }
    }

    // Store the guess itself.
    // Least significant 4 bits store Across value.
    Compressed2DGuess[NumGuesses] = Across;
    // Next least sig 4 bits store Forward value.
    Compressed2DGuess[NumGuesses] |= (Forward << 4);
    // Next least sig 4 bits store Number value.
    Compressed2DGuess[NumGuesses] |= (Number << 8);

    NumGuesses += 1;
}

void cSolver::PopGuess(int* Across, int* Forward, uchar* Number)
{
    int a, f, numbits, lsbytenum;

    my_assert(NumGuesses > 0);

    // Decompress the grid.
    numbits = 8;
    for (a = 0; a < 9; a++)
    {
        for (f = 0; f < 9; f++)
        {
            lsbytenum = (9 * a + f) * 9 / 8;
            CellContents[0][a][f] = (Compressed2DGrids[NumGuesses-1][lsbytenum] >> (8 - numbits)) & ((1 << numbits) - 1);
            CellContents[0][a][f] |= (Compressed2DGrids[NumGuesses-1][lsbytenum + 1] & ((1 << (9-numbits)) - 1)) << numbits;
            numbits = (numbits == 1) ? 8 : numbits - 1;
        }
    }

    // For the guess itself, least significant 4 bits store Across value.
    *Across = Compressed2DGuess[NumGuesses-1] & 0x000F;
    // Next least sig 4 bits store Forward value.
    *Forward = (Compressed2DGuess[NumGuesses-1] & 0x00F0) >> 4;
    // Next least sig 4 bits store Number value.
    *Number = (Compressed2DGuess[NumGuesses-1] & 0x0F00) >> 8;

    NumGuesses -= 1;
}

void cSolver::PopOffAllGuesses(void)
{
    NumGuesses = 0;
}

// Returns the next (i.e. more significant) bit position after Number. Number==1 means the LSB.
uchar cSolver::GetNextBitPos(uint16 x, uchar Number)
{
    uchar Shifts;

    x = (x >> Number);
    if (x == 0)
        return 0;
    for (Shifts = 0; (x & 0x01) == 0; x = x >> 1)
        Shifts += 1;
    return Number + Shifts + 1;
}

bool cSolver::CheckGridValid(void)
{
    int u, a, f;
    int UMax;

    UMax = Dimensions == SUDOKU_2D ? 0 : 8;

    for (u = 0; u <= UMax; u++)
    {
        for (a = 0; a < 9; a++)
        {
            for (f = 0; f < 9; f++)
            {
                if (CellContents[u][a][f] == 0)
                {
                    ContentsValid = false;
                }
            }
        }
    }
    return ContentsValid;
}
