
#include "ThreadsOption1.h"

#include "ThreadsOption1.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "CheckSudokuParts.h"

// global sudoku board and results array
sudokuboard* sudokuBoard;

int main(int argc, char **argv) {
	bool goodAnswer = true;
	int i, taskNum[27], returnValue;
	char boardAsString[STRING_BOARD_SIZE];
	int fd = 0;
	char* nameOfFile = "matrix";
	pthread_t allThreads[27];

	// if we don't have board file as argument, we will read from the input
	if (argc == 1) {
		readBoardFromInput(boardAsString, 0);
	} else { // read board from file
		nameOfFile = argv[1];
		fd = open(nameOfFile, O_RDONLY);
		if (fd == -1) {
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}

		// the input is now the file
		readBoardFromInput(boardAsString, fd);
		close(fd);
	}

	sudokuBoard = (sudokuboard*) calloc(1, sizeof(sudokuboard));

	// convert the board from string to int[81]
	convertStringBoardToIntBoard(boardAsString, STRING_BOARD_SIZE,
			sudokuBoard->board);

	// create 27 threads, one for each task
	for (i = 0; i < 27; i++) {
		taskNum[i] = i;
		returnValue = pthread_create(&allThreads[i], NULL, threadAct,
				(void*) &taskNum[i]);
		// if return value is not zero (there was a problem)
		if (returnValue) {
			free(sudokuBoard);
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}

	}

	// wait for the threads to complete their mission and end
	for (i = 0; i < 27; i++) {
		returnValue = pthread_join(allThreads[i], NULL);
		// if return value is not zero (there was a problem)
		if (returnValue) {
			free(sudokuBoard);
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	// check results in results array
	for (i = 0; i < 27; i++) {
		if (sudokuBoard->result[i] == 0) {
			goodAnswer = false;
			break;
		}
	}

	// free sudokuBoard memory
	free(sudokuBoard);

	// print if the board is legal or not
	if (goodAnswer)
		fprintf(stdout, "%s is legal\n", nameOfFile);
	else
		fprintf(stdout, "%s is not legal\n", nameOfFile);

	return 0;
}

// thread action, do the task on the sudoku board and update result in result array
void* threadAct(void* arg) {
	int taskNum = *(int*) arg;
	if (CheckRowsOrColsOrMatrix(taskNum, sudokuBoard->board) == true)
		sudokuBoard->result[taskNum] = 1;
	else
		sudokuBoard->result[taskNum] = 0;

	return NULL;
}

