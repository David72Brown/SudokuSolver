/*	This is the starting state of the sudoku. Any numbers we're given from the start go in here. All other positions are set to 0.
	It is assumed all sudokus are 9x9 or 9x9x9.
*/

#include <atlstr.h>
#include "Data.h"
#include "Start grid.h"

// You can specify whether the diagonals also include all 9 digits when you specify the starting state of the sudoku below.
// If diagonals are to be checked then for 3D puzzles, all 4 diagonals of the cube must each contain all digits 1-9.
// For 2D puzzles it's the 2 diagonals within the one plane.

// For interest, some sudokus here: https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
// 3D sudokus here: http://w01.tp1.jp/~sr10026691/Ans999PE.html

// This program also solves "killer" sudokus, which are 2D sudokus which obey all the usual rules but additionally, areas of
// the 9x9 grid are divided up into irregularly shaped areas ("cages") and totals are provided for the numbers within each cage.

/* To define the sudoku you want solved, you need to describe it below using this format:

-----------------
2D sudoku example
-----------------

const tSudokuSpec TheSudoku = {
	SUDOKU_2D,
	DIAGONALS,							<--- Or NO_DIAGONALS if you don't want diagonals checked.
	{{
		" 2 | 8 |  5",					<--- Write out the sudoku here including the numbers we start with.
		"  7| 3 |   ",
		"  3| 6 | 72",
		" 7 |  9| 5 ",
		"  4|   |2  ",
		" 6 |3  | 9 ",
		"19 | 7 |5  ",
		"   | 9 |7  ",
		"7  | 4 | 1 "
	}},
};

-----------------
3D sudoku example
-----------------

const tSudokuSpec TheSudoku = {
	SUDOKU_3D,
	DIAGONALS,							<--- Or NO_DIAGONALS if you don't want diagonals checked.
    {{
        "   |   | 6 ",	    		<--- Write out the 9 horizontal layers of the sudoku here including the numbers we start with.
        "   |   |   ",                   This is the first layer.
        "   |   |   ",
        "92 |   |   ",
        " 5 |  4|   ",
        "   |   |   ",
        " 3 |   | 8 ",
        "   |   |   ",
        "   |   |   "
    },{
        "   |   |   ",              <--- This is the second layer.
        " 3 |   |   ",
        "   |   |   ",
        "   |  1|   ",
        "   |   |   ",
        "   |   |   ",
        "  4|   |   ",
        "9  |   |   ",
        "   |   |   "
    },{                             <--- Etc.
		...
		...
    },{
        "   |   |3  ",				<--- This is the ninth layer.
        "  3|1  |   ",
        "   |   |   ",
        "   | 1 | 8 ",
        "   |   |   ",
        "  9|35 | 4 ",
        "   |   | 7 ",
        "8  |   |   ",
        "  1|   |   "
    }}
};

-----------------
Killer sudoku
-----------------

const tSudokuSpec TheSudoku = {
	SUDOKU_KILLER, NO_DIAGONALS,
	{{
		"   |   |   ",				<--- Killer sudokus are usually provided without any starting digits.
		"   |   |   ",					 It is the totals given for each cage which enable us to solve them.
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   ",
		"   |   |   "
	}},
	{
		"AA AA EE EE FF FF FF FF KK",	<--- To define the cage boundaries, we write any unique 2-char string into
		"AA EE EE II JJ JJ KK KK KK",		 each cell of each cage. I.e. all the cells marked as "AA" here are in
		"BB BB GG II JJ VV VV WW WW",		 one cage, all the cells marked "EE" are in another cage.
		"BB GG GG OO OO VV WW WW UU",
		"BB GG HH HH OO PP PP QQ UU",
		"BB CC CC LL OO OO QQ QQ UU",
		"CC CC LL LL NN RR QQ UU UU",
		"DD DD DD NN NN RR SS SS TT",
		"DD MM MM MM MM SS SS TT TT"
	},
	{
		{ "AA", 17 },					<--- The totals for the digits in each cage are given here.
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

*/

// Please describe below the sudoku you want solved:
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
