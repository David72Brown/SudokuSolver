/*	This is the starting state of the sudoku. Any numbers we're given from the start go in here. All other positions are set to 0.
	It is assumed all sudokus are 9x9 or 9x9x9. Anything smaller can be solved entirely by checking every possible combination.
*/

#include <atlstr.h>
#include "Data.h"
#include "Start grid.h"

// If doing diagonals for 3D puzzles, all 4 diagonals of the cube must each contain all digits 1-9.
// For 2D puzzles it's the 2 diagonals within the one plane.

// For testing, some sudokus here: https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
// 3D sudokus here: http://w01.tp1.jp/~sr10026691/Ans999PE.html

// 2D and 3D sudokus are both 2 dimensional arrays of CString. Same number of braces {} either way.
// For 2D sudokus we define only the ground floor rather than 9 floors.
const tSudokuSpec TheSudoku = {
	SUDOKU_KILLER, NO_DIAGONALS,
	{{
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   "
	}},
	{
		"AA AA EE EE FF FF FF FF KK",
		"AA EE EE II JJ JJ KK KK KK",
		"BB BB GG II JJ VV VV WW WW",
		"BB GG GG OO OO VV WW WW UU",
		"BB GG HH HH OO PP PP QQ UU",
		"BB CC CC LL OO OO QQ QQ UU",
		"CC CC LL LL NN RR QQ UU UU",
		"DD DD DD NN NN RR SS SS TT",
		"DD MM MM MM MM SS SS TT TT"
	},
	{
		{ "AA", 17 },
		{ "BB", 21 },
		{ "CC", 20 },
		{ "DD", 18 },
		{ "EE", 22 },
		{ "FF", 17 },
		{ "GG", 21 },
		{ "HH", 16 },
		{ "II",  7 },
		{ "JJ", 24 },
		{ "KK", 16 },
		{ "LL", 19 },
		{ "MM", 15 },
		{ "NN", 16 },
		{ "OO", 18 },
		{ "PP", 13 },
		{ "QQ", 16 },
		{ "RR",  8 },
		{ "SS", 14 },
		{ "TT", 23 },
		{ "UU", 25 },
		{ "VV", 16 },
		{ "WW", 23 }
	}
};
