
#ifndef CHECKSUDOKUPARTS_H_
#define CHECKSUDOKUPARTS_H_

#include "utils.h"

bool CheckRowsOrColsOrMatrix(int taskNum, int* boardAsArray);
void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]);

#endif /* CHECKSUDOKUPARTS_H_ */
