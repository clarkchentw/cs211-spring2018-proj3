#include <stdio.h>
#include <stdlib.h>

// Define boolean value
#define TRUE 1
#define FALSE 0
typedef struct mazeSearchStruct
{
        struct mazeSearchStruct* next; // pointer for next
        int x, y; // x y position
} mazeSearch;

typedef struct mazeStruct
{
	mazeSearch* hd;
	mazeSearch* solution;
	char arr[32][32];  /* allows for a maze of size 30x30 plus outer walls */
 	int visited[32][32]; // TRUE or FALSE
	int xsize, ysize;
	int xstart, ystart;
	int xend, yend;
} maze;

void setupMaze(int argc, char **argv, maze* mazeBoard){
	int xpos, ypos;
	int i,j;

	FILE *src;

	/* verify the proper number of command line arguments were given */
	if(argc != 2) 
	{
		fprintf( stderr, "Usage: %s <input file name>\n\n", argv[0]);
		exit(-1);
	}
	
	/* Try to open the input file. */
	if ( ( src = fopen( argv[1], "r" )) == NULL )
	{
		fprintf ( stderr, "Can't open input file: %s\n", argv[1] );
		exit(-1);
	}

	/* read in the size, starting and ending positions in the maze */
	fscanf (src, "%d %d", &mazeBoard->xsize, &mazeBoard->ysize);
	fscanf (src, "%d %d", &mazeBoard->xstart, &mazeBoard->ystart);
	fscanf (src, "%d %d", &mazeBoard->xend, &mazeBoard->yend);
	/* print them out to verify the input */
	printf ("size: %d, %d\n", mazeBoard->xsize, mazeBoard->ysize);
	printf ("start: %d, %d\n", mazeBoard->xstart, mazeBoard->ystart);
	printf ("end: %d, %d\n", mazeBoard->xend, mazeBoard->yend);

	/* initialize the maze to empty */
	for (i = 0; i < mazeBoard->xsize+2; i++){
		for (j = 0; j < mazeBoard->ysize+2; j++){
			mazeBoard->arr[i][j] = '.';
			mazeBoard->visited[i][j] = FALSE; //Reset all to unvisited
		}
	}
	/* mark the borders of the maze with *'s */

	for (i=0; i < mazeBoard->xsize+2; i++) {
		mazeBoard->arr[i][0] = '*';
		mazeBoard->visited[i][0] = TRUE;
		mazeBoard->arr[i][mazeBoard->ysize+1] = '*';
		mazeBoard->visited[i][mazeBoard->ysize+1] = TRUE;
	}
	for (i=0; i < mazeBoard->ysize+2; i++)
	{
		mazeBoard->arr[0][i] = '*';
		mazeBoard->visited[0][i] = TRUE;
		mazeBoard->arr[mazeBoard->xsize+1][i] = '*';
		mazeBoard->visited[mazeBoard->xsize+1][i] = TRUE;
	}

	/* mark the starting and ending positions in the maze */
	mazeBoard->arr[mazeBoard->xstart][mazeBoard->ystart] = 's';
 	mazeBoard->arr[mazeBoard->xend][mazeBoard->yend] = 'e';

	/* mark the blocked positions in the maze with *'s */
	while (fscanf (src, "%d %d", &xpos, &ypos) != EOF)
	{
		mazeBoard->arr[xpos][ypos] = '*';
		mazeBoard->visited[xpos][ypos] = TRUE;
	}
}

void printMaze(maze* mazeBoard){
	int i, j; // for the loop
	/* print out the initial maze */
	for (i = 0; i < mazeBoard->xsize+2; i++){
		for (j = 0; j < mazeBoard->ysize+2; j++){
			printf ("%c", mazeBoard->arr[i][j]);
		}
		printf("\n");
	}
}

void printMazeVisited(maze* mazeBoard){
        int i, j; // for the loop
	char indicator;
        /* print out the initial maze */
	printf("\nThis is Maze Visit History (Debug):\n");
        for (i = 0; i < mazeBoard->xsize+2; i++){
                for (j = 0; j < mazeBoard->ysize+2; j++){
			if (mazeBoard->visited[i][j] == TRUE){
				indicator = '!';
			}
			else{
				indicator = '.';
			}
                        printf ("%c", indicator);
                }
                printf("\n");
        }
}

void markVisited(maze* mazeBoard, int x, int y){
	mazeBoard->visited[x][y] = TRUE;
}

void searchInit(maze* mazeBoard, int x, int y){
	// Check if the hd has been init or not
	if (mazeBoard->hd == NULL){
		mazeBoard->hd = (mazeSearch*) malloc(sizeof(mazeSearch));
		if (x < mazeBoard->xstart || x > mazeBoard->xend){
			printf("invalid x of value %d\n", x);
			return;
		}
		
		if (y < mazeBoard->ystart || y > mazeBoard->yend){
			printf("invalid y of value %d\n", y);
			return;
        	}
		mazeBoard->hd->x = x;
		mazeBoard->hd->y = y;
	}
	else{
		printf("hd is already initialized\n");
	}
}

void searchPush(maze* mazeBoard, int x, int y){
	mazeSearch* tmp = (mazeSearch*) malloc (sizeof(mazeSearch));
	tmp->x = x;
	tmp->y = y;
	tmp->next = mazeBoard->hd;
	mazeBoard->hd = tmp;
	markVisited(mazeBoard, x, y);
}

void searchSolutionPush(maze* mazeBoard, int x, int y){
        mazeSearch* tmp = (mazeSearch*) malloc (sizeof(mazeSearch));
        tmp->x = x;
        tmp->y = y;
        tmp->next = mazeBoard->solution;
        mazeBoard->solution = tmp;
}

void searchPop(maze* mazeBoard){
	if (mazeBoard->hd == NULL){
		printf("Can't pop, hd is NULL\n");
		return;
	}
	mazeSearch* tmp = (mazeSearch*) malloc (sizeof(mazeSearch));
	tmp = mazeBoard->hd;
	mazeBoard->hd = mazeBoard->hd->next;
	free(tmp);
}

int isEmpty(mazeSearch* hd){
	if (hd == NULL){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

char lookAround(maze* mazeBoard, int x, int y){
	// Priority Right(r) Down(d) Left(l) Up(u), then not found(?)
	if (mazeBoard->visited[x][y+1] == FALSE){
		return 'r';
	}
	if (mazeBoard->visited[x+1][y] == FALSE){
		return 'd';
	}
	if (mazeBoard->visited[x][y-1] == FALSE){
		return 'l';
	}
	if (mazeBoard->visited[x-1][y] == FALSE){
		return 'u';
	}
	return '?';
}

void stepByStepDebug(maze* mazeBoard, char nextDirection, int x, int y){
	printMaze(mazeBoard);
	printMazeVisited(mazeBoard);
	printf("nextDirection: %c\n", nextDirection);
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("Current: %c", mazeBoard->arr[mazeBoard->hd->x][mazeBoard->hd->y]);
	getchar(); 
}


void prepareSolution(maze* mazeBoard){
	mazeSearch* tmp = (mazeSearch*) malloc (sizeof(mazeSearch));
	while (TRUE){
		if (mazeBoard->hd == NULL){
			break;
		}
		searchSolutionPush(mazeBoard, mazeBoard->hd->x, mazeBoard->hd->y);
		searchPop(mazeBoard);
	}
	printf("Solution Generated\n");
	printf("Start Playing the Solution by holding the [ENTER] key\n");
}


// Return status Code: 200 for Found, 404 for not found, System error 500
int searching(maze* mazeBoard){
	char nextDirection;
	while (TRUE) 
	{
		if (isEmpty(mazeBoard->hd)){
			break;
		}
		if (mazeBoard->arr[mazeBoard->hd->x][mazeBoard->hd->y] == 'e'){
			break;
		}
		nextDirection = lookAround(mazeBoard, mazeBoard->hd->x, mazeBoard->hd->y);
		//stepByStepDebug(mazeBoard, nextDirection, mazeBoard->hd->x, mazeBoard->hd->y);
		switch(nextDirection){
			case 'r':	searchPush(mazeBoard, mazeBoard->hd->x, mazeBoard->hd->y+1);	break;
			case 'd':	searchPush(mazeBoard, mazeBoard->hd->x+1, mazeBoard->hd->y);	break;
			case 'l':	searchPush(mazeBoard, mazeBoard->hd->x, mazeBoard->hd->y-1);	break;
			case 'u':	searchPush(mazeBoard,  mazeBoard->hd->x-1, mazeBoard->hd->y);	break;
			case '?':	searchPop(mazeBoard);	break;
		}
	}
	if (isEmpty(mazeBoard->hd)){
		return 404;
	}
	else if (mazeBoard->hd != NULL && // Prevent reading null pointer
		mazeBoard->arr[mazeBoard->hd->x][mazeBoard->hd->y] == 'e'){
		return 200;
	}
	else{
		return 500; // Shouldn't happen, but just in case
	}
}

void recursivePlayer(maze* mazeBoard, mazeSearch* hd){
	if (hd != NULL){
		getchar();
		if (mazeBoard->arr[hd->x][hd->y] != 's' || mazeBoard->arr[hd->x][hd->y] != 'e')
			mazeBoard->arr[hd->x][hd->y] = '+';
		int i;
		for (i = 0; i < 40; i++){
			printf("\n");
		}
		printMaze(mazeBoard);
		printf("Continue playing by pressing the [ENTER] Key\n");
		getchar();
		recursivePlayer(mazeBoard, hd->next);
	}
}

void playMazeSolution(maze* mazeBoard){
	recursivePlayer(mazeBoard, mazeBoard->solution);
}

int main (int argc, char **argv)
{
	maze mazeBoard;
	setupMaze(argc, argv, &mazeBoard); 
	//printMaze(&mazeBoard);
	//printMazeVisited(&mazeBoard);
	
	//push the start position's coordinates on the stack
	searchInit(&mazeBoard, mazeBoard.xstart, mazeBoard.ystart);
	//mark the start position as visited
	markVisited(&mazeBoard, mazeBoard.xstart, mazeBoard.ystart);
	

	printMaze(&mazeBoard);
	//printMazeVisited(&mazeBoard);

	int result = searching(&mazeBoard);
	if (result == 200){
		prepareSolution(&mazeBoard);
		playMazeSolution(&mazeBoard);
	}
	else if (result == 404){
		printf("Result Not Found...\n");
	}
	else{
		printf("Error: %d", result);
	}
}
