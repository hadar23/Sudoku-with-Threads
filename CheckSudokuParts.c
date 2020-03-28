
#include "CheckSudokuParts.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

const char DIGITS[] = { '0', '1' };

// checks the rows or columns or matrices in the board
bool CheckRowsOrColsOrMatrix(int taskNum, int* boardAsArray) {
	int j;
	unsigned int answer = 0;
	int board[9][9];
	int whichPart = taskNum % 9;

	fromBoardArrayToBoardMatrix(boardAsArray, board);

	for (j = 0; j < 9; j++) {
		if (0 <= taskNum && taskNum <= 8) { // rows
			answer = answer | (1 << board[whichPart][j]);
		} else if (9 <= taskNum && taskNum <= 17) { // columns
			answer = answer | (1 << board[j][whichPart]);
		} else if (18 <= taskNum && taskNum <= 26) { // matrices
			answer = answer
					| (1
							<< board[(j / 3) + (whichPart / 3) * 3][(j % 3)
									+ (whichPart % 3) * 3]);
		}

		// every bit from bit 2, is an answer for a part of the board
		// we have 9 parts
		// answer of 1 means this part is correct
		// we need all parts to be correct in order to return: answer = true

	}
	if (answer != 0b1111111110)
		return false;
	return true;
}

// convert board from int[81] to int[9][9]
void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]) {
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			boardAsMatrix[i][j] = *(boardAsArray + i * 9 + j);
		}
	}
}

