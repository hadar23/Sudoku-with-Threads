all: ThreadsQ1 ThreadsQ2

ThreadsQ1:
	gcc -pthread -o ThreadsQ1 ThreadsOption1.c CheckSudokuParts.c utils.c
ThreadsQ2:
	gcc -pthread -o ThreadsQ2 ThreadsOption2.c CheckSudokuParts.c utils.c
clean:
	rm -f ThreadsQ1 ThreadsQ2
