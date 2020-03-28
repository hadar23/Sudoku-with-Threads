
#ifndef THREADSOPTION1_H_
#define THREADSOPTION1_H_

typedef struct {
	int board[81];
	char result[27];
} sudokuboard;

void* threadAct(void* arg);

#endif /* THREADSOPTION1_H_ */
