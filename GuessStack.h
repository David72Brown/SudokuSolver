#pragma once

#include "Data.h"

extern void PushGuess(int Across, int Back, uchar Number);
extern void PopGuess(int *Across, int* Back, uchar *Number);
extern void PopOffAllGuesses(void);