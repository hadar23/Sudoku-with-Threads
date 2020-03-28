
#ifndef THREADSOPTION2_H_
#define THREADSOPTION2_H_

typedef struct {
	int missions[27];
	int currentMission;
	int numOfCompletedMissions;
} sudokutasks;

#define N 10

void* threadAct(void* arg);

#endif /* THREADSOPTION2_H_ */
