
#include "ThreadsOption2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include "utils.h"
#include "CheckSudokuParts.h"

// global sudoku board and results array
sudokutasks* sudokuTasks;

int board[81];
int resultOfBoard;

pthread_mutex_t tasksMutex;
pthread_mutex_t resultMutex;
pthread_cond_t condition;

int main(int argc, char **argv) {
	bool goodAnswer = true;
	int i, returnValue, tempNumOfCompletedTasks = 0;
	char boardAsString[STRING_BOARD_SIZE];
	int fd = 0;
	char* nameOfFile = "matrix";
	pthread_t allThreads[N];

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

	sudokuTasks = (sudokutasks*) calloc(1, sizeof(sudokutasks));

	// 1 means legal, 0 means not legal.  we assume the board is legal.
	// if not, some thread will change to 0 (not legal)
	resultOfBoard = 1;

	// Reseting
	sudokuTasks->numOfCompletedMissions = 0;
	sudokuTasks->currentMission = 0;
	for (i = 0; i < 27; i++) {
		sudokuTasks->missions[i] = i;
	}

	// convert the board from string to int[81]
	convertStringBoardToIntBoard(boardAsString, STRING_BOARD_SIZE, board);

	// Initializing
	pthread_mutex_init(&tasksMutex, NULL);
	pthread_mutex_init(&resultMutex, NULL);
	pthread_cond_init(&condition, NULL);

	// lock mutex before creating threads
	pthread_mutex_lock(&tasksMutex);

	// create 27 threads, one for each task
	for (i = 0; i < N; i++) {
		returnValue = pthread_create(&allThreads[i], NULL, threadAct,
				(void*) NULL);
		// if return value is not zero (there was a problem)
		if (returnValue) {
			free(sudokuTasks);
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}

	}

	// wait until all tasks completed
	while (tempNumOfCompletedTasks < 27) {
		pthread_cond_wait(&condition, &tasksMutex);
		tempNumOfCompletedTasks = sudokuTasks->numOfCompletedMissions;
	}
	// unlock mutex
	pthread_mutex_unlock(&tasksMutex);

	// check result
	if (resultOfBoard == 0) {
		goodAnswer = false;
	}

	// free sudokuBoard memory
	free(sudokuTasks);

	// print if the board is legal or not
	if (goodAnswer)
		fprintf(stdout, "%s is legal\n", nameOfFile);
	else
		fprintf(stdout, "%s is not legal\n", nameOfFile);

	// "for the good order", we want to make sure all threads are gone
	// wait for the threads to complete their mission and end
	for (i = 0; i < N; i++) {
		returnValue = pthread_join(allThreads[i], NULL);
		// if return value is not zero (there was a problem)
		if (returnValue) {
			free(sudokuTasks);
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	// destroy mutexes and condition
	pthread_mutex_destroy(&tasksMutex);
	pthread_mutex_destroy(&resultMutex);
	pthread_cond_destroy(&condition);

	return 0;
}

// thread action, do the task on the sudoku board and update result in result array
void* threadAct(void* arg) {
	int taskNum = 0;
	bool resultOfTask = false;

	for (;;) {
		pthread_mutex_lock(&tasksMutex);
		if (sudokuTasks->currentMission >= 27) {
			pthread_cond_signal(&condition);
			pthread_mutex_unlock(&tasksMutex);
			break;
		}
		// get the task number
		taskNum = sudokuTasks->missions[sudokuTasks->currentMission++];
		pthread_mutex_unlock(&tasksMutex);

		// do the task and get the result
		resultOfTask = CheckRowsOrColsOrMatrix(taskNum, board);

		// if result of board is 1 (true) and result of task is false, we need to update result of board
		if (resultOfBoard == 1 && resultOfTask == false) {
			pthread_mutex_lock(&resultMutex);
			resultOfBoard = 0;
			pthread_mutex_unlock(&resultMutex);
		}
		// else, if resultOfBoard is already 0 (false), we do nothing
		// else, if resultOfTask is true, we do nothing

		// signal condition when ending task
		pthread_mutex_lock(&tasksMutex);
		sudokuTasks->numOfCompletedMissions++;
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&tasksMutex);
	}

	return NULL;
}

