#include "Data.h"
#include "GuessStack.h"

static uchar NumGuesses = 0;

#define MAX_GUESSES 81

static uchar Compressed2DGrids[MAX_GUESSES][92]; // Each grid is 81 cells with 9 bits to record the possibilities in each cell.
static uint16 Compressed2DGuess[MAX_GUESSES];  // Each guess is 4 bits for Across, 4 bits for Back, 4 bits for Number.

void PushGuess(int Across, int Back, uchar Number)
{
	static int a, b;
	static uint16 bit;

	my_assert(NumGuesses < MAX_GUESSES);

	// Store the grid.
	memset(Compressed2DGrids[NumGuesses], 0, sizeof(Compressed2DGrids[0]));
	bit = 0;
	for (a = 0; a < 9; a++)
	{
		for (b = 0; b < 9; b++)
		{
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0001)      << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0002) >> 1 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0004) >> 2 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0008) >> 3 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0010) >> 4 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0020) >> 5 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0040) >> 6 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0080) >> 7 << (bit % 8);
			bit += 1;
			Compressed2DGrids[NumGuesses][bit / 8] |= (Grid[0][a][b] & 0x0100) >> 8 << (bit % 8);
			bit += 1;
		}
	}

	// Store the guess itself.
	// Least significant 4 bits store Across value.
	Compressed2DGuess[NumGuesses] = Across;
	// Next least sig 4 bits store Back value.
	Compressed2DGuess[NumGuesses] |= (Back << 4);
	// Next least sig 4 bits store Number value.
	Compressed2DGuess[NumGuesses] |= (Number << 8);

	NumGuesses += 1;
}

void PopGuess(int* Across, int* Back, uchar* Number)
{
	static int a, b;
	static uint16 bit;

	my_assert(NumGuesses > 0);

	NumGuesses -= 1;

	// Decompress the grid.
	bit = 0;
	for (a = 0; a < 9; a++)
	{
		for (b = 0; b < 9; b++)
		{
			Grid[0][a][b] =  (Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 1;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 2;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 3;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 4;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 5;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 6;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 7;
			bit += 1;
			Grid[0][a][b] |= ((Compressed2DGrids[NumGuesses][bit / 8] >> (bit % 8)) & 0x01) << 8;
			bit += 1;
		}
	}

	// For the guess itself, least significant 4 bits store Across value.
	*Across = Compressed2DGuess[NumGuesses] & 0x000F;
	// Next least sig 4 bits store Back value.
	*Back = (Compressed2DGuess[NumGuesses] & 0x00F0) >> 4;
	// Next least sig 4 bits store Number value.
	*Number = (Compressed2DGuess[NumGuesses] & 0x0F00) >> 8;
}

void PopOffAllGuesses(void)
{
	NumGuesses = 0;
	// All guesses are stored in static memory allocated at top of this file, so nothing to dealloc here.
}
