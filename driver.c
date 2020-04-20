#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "graphPathAlg.h"
#include "graph.h"
#include "point2D.h"
#include "queuePoint2D.h"

/* Largest size of maze to be printed */
#define MAX_MAZE_PRINT 45

/* parameters to determine how to test student hasPath function */
#define TEST_HASPATH true
#define HASPATH_MIN_SIZE 8
#define HASPATH_MAX_SIZE 60
#define HASPATH_UPDATE_SIZE 5

/* parameters to determine how to test student findNearest function */
#define TEST_FINDNEAREST true
#define FINDNEAREST_MIN_SIZE 8
#define FINDNEAREST_MAX_SIZE 60
#define FINDNEAREST_UPDATE_SIZE 3

/* parameters to determine how to test student findLongest function */
#define TEST_FINDLONGEST true
#define FINDLONGEST_MIN_SIZE 4
#define FINDLONGEST_MAX_SIZE 9 /* Careful when changing this #.  Even just a size of 15 can sometimes take 3+ minutes to solve. */
#define FINDLONGEST_UPDATE_SIZE 1

/* These allow suppression of maze printing depending on whether the test succeeds */
#define PRINT_MAZE_ON_SUCCESS false
#define PRINT_MAZE_ON_UNKNOWN false
#define PRINT_MAZE_ON_FAILURE true

/* Suppress output of true positive/true negative/false positive/false negative and success counts */
#define SUPPRESS_OUTPUT_ON_SUCCESS false

void addDeadEnds( char** maze, int size );
void testHasPath( int minSize, int maxSize, int update );
void testFindNearestFinish( int minSize, int maxSize, int update );
void testFindLongestSimplePath( int minSize, int maxSize, int update, bool advanced );

void recordSolution( char** maze, int size, char *name, pathResult correctResult, pathResult testResult,
                    int *truePositive, int *trueNegative, int *falsePositive, int *falseNegative, bool* implemented, bool passesOtherConds, char *failureMsg );
void printResults( char* name, int truePositive, int trueNegative, int falsePositive, int falseNegative, bool implemented, bool passesOtherConds );

char** createBasicMaze( int size, bool path );
char** createMultipleFinishMaze( int size, int *shortestPathLength );
char** createSimplePathMaze( int size, int *longestPathLength, pathResult *presult );

char** mallocMaze( int size );
void fillMazeWithSymbol( char **maze, Point2D startPoint, Point2D endPoint, char c );
void createMazeBorder( char **maze, int start, int end );
void createPath( char **maze, int start, int end, bool path );
int createPathBetweenPoints( char **maze, Point2D startP, Point2D endP, Point2D tlCorner, Point2D brCorner, int probability, char c );
int createSimplePathBetweenPoints( char **maze, Point2D startP, Point2D endP, Point2D tlCorner, Point2D brCorner, int probability, char c, pathResult *presult );

int replaceZero( int dir );
int median( int a, int b, int c );
int min( int a, int b );
int max( int a, int b );
void printMaze( char **maze, int size );
void freeMaze( char **maze, int size );

int main( int argc, char *argv[] )
{
    clock_t start, end;
    srand(time(0));

    /* Test basic path finding capability */
    if( TEST_HASPATH ){
        start = clock();
        testHasPath( HASPATH_MIN_SIZE, HASPATH_MAX_SIZE, HASPATH_UPDATE_SIZE );
        end = clock();
        printf( "hasPath testing took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );
    }

    /* Test function for finding nearest finish to graph */
    if( TEST_FINDNEAREST ){
        start = clock();
        testFindNearestFinish( FINDNEAREST_MIN_SIZE, FINDNEAREST_MAX_SIZE, FINDNEAREST_UPDATE_SIZE );
        end = clock();
        printf( "findNearestFinish testing took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );
    }

    /* Test function for finding longest simple (i.e., no revisiting vertices) path to the finish */
    if( TEST_FINDLONGEST ){
        start = clock();
        testFindLongestSimplePath( FINDLONGEST_MIN_SIZE, FINDLONGEST_MAX_SIZE, FINDLONGEST_UPDATE_SIZE, true );
        end = clock();
        printf( "findLongestSimplePath testing took %lf seconds\n\n", (double)(end - start)/ CLOCKS_PER_SEC );
    }

    return 0;
}

/************************ TESTING FUNCTIONS ************************/

/* Test hasPath */
void testHasPath( int minSize, int maxSize, int update )
{
    int size, falsePositive=0, falseNegative=0, truePositive=0, trueNegative=0;
    bool implemented = true;
    char **maze;

    /* Test basic path finding capability */
    for( size=minSize; size<=maxSize; size+=update )
    {
        maze = createBasicMaze( size, PATH_FOUND );
        recordSolution( maze, size, "hasPath", PATH_FOUND, hasPath( maze, size ), &truePositive, &trueNegative, &falsePositive, &falseNegative, &implemented, true, NULL );
        freeMaze( maze, size );
    }
    for( size=minSize; size<=maxSize; size+=update )
    {
        maze = createBasicMaze( size, PATH_IMPOSSIBLE );
        recordSolution( maze, size, "hasPath", PATH_IMPOSSIBLE, hasPath( maze, size ), &truePositive, &trueNegative, &falsePositive, &falseNegative, &implemented, true, NULL );
        freeMaze( maze, size );
    }
    printResults( "hasPath", truePositive, trueNegative, falsePositive, falseNegative, implemented, true );
    if( falsePositive==0 && falseNegative==0 && implemented )
            printf( "All test cases succeeded.  Good job!\n" );
}

/* Test findNearestFinish */
void testFindNearestFinish( int minSize, int maxSize, int update )
{
    int size, falsePositive=0, falseNegative=0, truePositive=0, trueNegative=0;
    int correctSPLength, testSPLength, numSPsCorrect=0, numSPsIncorrect=0;
    pathResult testResult;
    bool implemented = true;
    char **maze;
    char failureMsg[100];

    for( size=minSize; size<=maxSize; size+=update )
    {
        maze = createMultipleFinishMaze( size, &correctSPLength );
        testResult = findNearestFinish( maze, size, &testSPLength );

        if( correctSPLength==testSPLength )
            numSPsCorrect++;
        else if( (PRINT_MAZE_ON_FAILURE && implemented) || (PRINT_MAZE_ON_UNKNOWN && !implemented) ){
            sprintf( failureMsg, "FAILURE - findNearestFinish - Shortest path has length = %d but your function returned %d\n", correctSPLength, testSPLength);
            numSPsIncorrect++;
        }
        else
            numSPsIncorrect++;

        recordSolution( maze, size, "findNearestFinish", PATH_FOUND, testResult, &truePositive, &trueNegative, &falsePositive, &falseNegative, &implemented, correctSPLength==testSPLength, failureMsg );
        freeMaze( maze, size );
    }
    for( size=minSize; size<=maxSize; size+=update )
    {
        maze = createBasicMaze( size, PATH_IMPOSSIBLE );
        recordSolution( maze, size, "findNearestFinish", PATH_IMPOSSIBLE, findNearestFinish( maze, size, &testSPLength ), &truePositive, &trueNegative, &falsePositive, &falseNegative, &implemented, true, NULL );
        freeMaze( maze, size );
    }

    printResults( "findNearestFinish", truePositive, trueNegative, falsePositive, falseNegative, implemented, (numSPsIncorrect==0) );
    if( implemented && ( !SUPPRESS_OUTPUT_ON_SUCCESS || numSPsIncorrect!=0 ) )
        printf( "Correctly identified the shortest path length in %d out of %d test cases\n", numSPsCorrect, numSPsIncorrect+numSPsCorrect );
    if( falsePositive==0 && falseNegative==0 && (numSPsIncorrect==0) && implemented )
        printf( "All test cases succeeded.  Good job!\n" );
}

/* Test findLongestSimplePath */
void testFindLongestSimplePath( int minSize, int maxSize, int update, bool advanced  )
{
    int i, size, falsePositive=0, falseNegative=0, truePositive=0, trueNegative=0;
    int correctLPLength, testLPLength, numLPsCorrect=0, numLPsIncorrect=0;
    pathResult testResult, correctResult;
    bool implemented = true;
    char **maze;
    char failureMsg[100];

    for( size=minSize; size<=maxSize; size+=update )
    {
        for( i=0; i<=min((size-3)*(size-3),10); i++ )/* Run test size case multiple times */
        {
            maze = createSimplePathMaze( size, &correctLPLength, &correctResult );
            if( advanced&&rand()%2 ) /* randomly complicate the maze */
                addDeadEnds( maze, size );

            testResult = findLongestSimplePath( maze, size, &testLPLength );

            if( correctLPLength==testLPLength )
                numLPsCorrect++;
            else if( (PRINT_MAZE_ON_FAILURE && implemented) || (PRINT_MAZE_ON_UNKNOWN && !implemented) ){
                sprintf( failureMsg, "FAILURE - findLongestSimplePath - Longest simple path has length = %d but your function returned %d\n", correctLPLength, testLPLength);
                numLPsIncorrect++;
            }
            else
                numLPsIncorrect++;

            recordSolution( maze, size, "findLongestSimplePath", correctResult, testResult, &truePositive, &trueNegative, &falsePositive, &falseNegative, &implemented, correctLPLength==testLPLength, failureMsg );
            freeMaze( maze, size );
        }
    }

    printResults( "findLongestSimplePath", truePositive, trueNegative, falsePositive, falseNegative, implemented, (numLPsIncorrect==0) );
    if( implemented && ( !SUPPRESS_OUTPUT_ON_SUCCESS || numLPsIncorrect!=0 ) )
        printf( "Correctly identified the longest simple path length in %d out of %d test cases\n", numLPsCorrect, numLPsIncorrect+numLPsCorrect );
    if( falsePositive==0 && falseNegative==0 && (numLPsIncorrect==0) && implemented )
        printf( "All test cases succeeded.  Good job!\n" );
}

/************************ REPORTING/PRINTING FUNCTIONS ************************/

/* Determines the type of error that occurred and updates parameters accordingly */
void recordSolution( char** maze, int size, char *name, pathResult correctResult, pathResult testResult,
                     int *truePositive, int *trueNegative, int *falsePositive, int *falseNegative, bool* implemented, bool passesOtherConds, char *failureMsg )
{
    if( correctResult==PATH_FOUND && testResult==PATH_FOUND ){
        if( PRINT_MAZE_ON_SUCCESS || (!passesOtherConds&&PRINT_MAZE_ON_FAILURE&&implemented) ){
            printf("SUCCESS - %s - Path found for maze of size %d (true positive)\n", name, size);
            if( !passesOtherConds ) printf("%s", failureMsg);
            if( size<=MAX_MAZE_PRINT ) printMaze( maze, size );
            else printf("Maze too large to display\n");
        }
        (*truePositive)++;
    }
    else if( correctResult==PATH_FOUND && testResult==PATH_IMPOSSIBLE ){
        if( PRINT_MAZE_ON_FAILURE ){
            printf("FAILURE - %s - Path not found for maze of size %d (false negative)\n", name, size);
            if( !passesOtherConds ) printf("%s", failureMsg);
            if( size<=MAX_MAZE_PRINT ) printMaze( maze, size );
            else printf("Maze too large to display\n");
        }
        (*falseNegative)++;
    }
    else if( correctResult==PATH_IMPOSSIBLE && testResult==PATH_IMPOSSIBLE ){
        if( PRINT_MAZE_ON_SUCCESS || (!passesOtherConds&&PRINT_MAZE_ON_FAILURE&&implemented) ){
            printf("SUCCESS - %s - Path not found for maze of size %d (true negative)\n", name, size);
            if( !passesOtherConds ) printf("%s", failureMsg);
            if( size<=MAX_MAZE_PRINT ) printMaze( maze, size );
            else printf("Maze too large to display\n");
        }
        (*trueNegative)++;
    }
    else if( correctResult==PATH_IMPOSSIBLE && testResult==PATH_FOUND ){
        if( PRINT_MAZE_ON_FAILURE ){
            printf("FAILURE - %s - Path found for maze of size %d\n (false positive)", name, size);
            if( !passesOtherConds ) printf("%s", failureMsg);
            if( size<=MAX_MAZE_PRINT ) printMaze( maze, size );
            else printf("Maze too large to display\n");
        }
        (*falsePositive)++;
    }
    else if( testResult==PATH_UNKNOWN ){
        if( PRINT_MAZE_ON_UNKNOWN ){
            if( !passesOtherConds ) printf("%s", failureMsg);
            if( size<=MAX_MAZE_PRINT ) printMaze( maze, size );
            else printf("Maze too large to display\n");
        }
        (*implemented) = false;
    }
}

void printResults( char* name, int truePositive, int trueNegative, int falsePositive, int falseNegative, bool implemented, bool passesOtherConds ){
    printf( "%s Summary of Results:\n", name );
    printf( "------------------------------------------------\n" );
    if( implemented && ( !SUPPRESS_OUTPUT_ON_SUCCESS || falsePositive+falseNegative!=0 ) ){
        printf( "True Positives:  %5d, True Negatives:  %5d\n", truePositive, trueNegative );
        printf( "False Positives: %5d, False Negatives: %5d\n", falsePositive, falseNegative );
    }
    else if( !implemented && !PRINT_MAZE_ON_UNKNOWN ){
        printf("NOT IMPLEMENTED - %s - This function is not yet implemented\n", name);
    }
}

/************************ MAZE CREATION/PRINTING FUNCTIONS ************************/

//Create and fill in a randomly generated maze with length and width = size
char** createBasicMaze( int size, bool path )
{
    int i, dist, numRandPoints;
    Point2D startPoint, endPoint, tlCorner, brCorner, randomPoint, prevPoint;

    startPoint.x = 2;
    startPoint.y = rand()%(size-4) + 2;
    endPoint.x = size-3;
    endPoint.y = rand()%(size-4) + 2;

    tlCorner.x = 1;
    tlCorner.y = 1;
    brCorner.x = size-2;
    brCorner.y = size-2;

    numRandPoints = (rand()%size)/2;

    char** maze = mallocMaze( size );
    fillMazeWithSymbol( maze, createPoint(0, 0), createPoint(size-1, size-1), 'X' );
    fillMazeWithSymbol( maze, createPoint(startPoint.x-2, startPoint.y-2), createPoint(startPoint.x+2, startPoint.y+2), ' ' );
    fillMazeWithSymbol( maze, createPoint(endPoint.x-2, endPoint.y-2), createPoint(endPoint.x+2, endPoint.y+2), ' ' );

    /* create a trail of random points to connect startPoint to endPoint */
    prevPoint = startPoint;
    for(i=0; i<numRandPoints; i++){
        randomPoint.x = rand()%(size-4) + 2;
        randomPoint.y = rand()%(size-4) + 2;

        createPathBetweenPoints( maze, prevPoint, randomPoint, tlCorner, brCorner, 70, ' ' );
        prevPoint = randomPoint;
    }
    createPathBetweenPoints( maze, prevPoint, endPoint, tlCorner, brCorner, 70, ' ' );

    dist = abs(startPoint.x - endPoint.x)-2;
    dist = rand()%dist+1;
    if(!path)
        createPathBetweenPoints( maze, createPoint(startPoint.x+dist, tlCorner.y), createPoint(endPoint.x-dist, brCorner.y),
                                createPoint(startPoint.x+1, tlCorner.y), createPoint(endPoint.x-1, brCorner.y), 100, 'X' );

    createMazeBorder( maze, 0, size-1 );

    maze[startPoint.x][startPoint.y] = 'S';
    maze[endPoint.x][endPoint.y] = 'F';

    return maze;
}

//Create and fill in a randomly generated maze with length and width = size
char** createMultipleFinishMaze( int size, int *shortestPathLength )
{
    int i, curDist, numRandPoints;
    Point2D startPoint, endPoint, endPointDownUp, tlCorner, brCorner, randomPoint;
    Queue* q = createQueue();

    startPoint.x = size/2+1;
    startPoint.y = size-4;
    endPointDownUp.x = size/2+1;
    endPointDownUp.y = size-2;

    tlCorner.x = 1;
    tlCorner.y = 1;
    brCorner.x = size-2;
    brCorner.y = size-2;

    numRandPoints = (rand()%size)/4;

    char** maze = mallocMaze( size );
    fillMazeWithSymbol( maze, createPoint(0, 0), createPoint(size-1, size-1), 'X' );

    /* create a trail of points to connect startPoint to endPointDownUp */
    curDist = 0;
    curDist += createPathBetweenPoints( maze, startPoint, createPoint( brCorner.x, startPoint.y ), tlCorner, brCorner, 100, ' ' );
    curDist += createPathBetweenPoints( maze, createPoint( brCorner.x, startPoint.y ), createPoint( brCorner.x, endPointDownUp.y ), tlCorner, brCorner, 100, ' ' );
    curDist += createPathBetweenPoints( maze, createPoint( brCorner.x, endPointDownUp.y ), endPointDownUp, tlCorner, brCorner, 100, ' ' );
    *shortestPathLength = curDist;
    enqueue( q, endPointDownUp );

    for( i=0; i<numRandPoints; i++){
        randomPoint = createPoint( rand()%(size-2)+1, rand()%(size/2)+1 );
        if( !equals(startPoint,randomPoint) ){ /* don't overwrite start point */
            enqueue( q, randomPoint );
            curDist = createPathBetweenPoints( maze, startPoint, randomPoint, tlCorner, brCorner, 100, ' ' );
            if( curDist<*shortestPathLength )
                *shortestPathLength = curDist;
        }
    }

    createMazeBorder( maze, 0, size-1 );

    maze[startPoint.x][startPoint.y] = 'S';
    while( !isEmptyQueue(q) ){
        endPoint = dequeue( q );
        maze[endPoint.x][endPoint.y] = 'F';
    }
    freeQueue(q);

    return maze;
}

//Create and fill in a randomly generated maze with length and width = size
char** createSimplePathMaze( int size, int *longestPathLength, pathResult *presult )
{
    Point2D startPoint, endPoint, tlCorner, brCorner;

    startPoint.x = 1;
    startPoint.y = 1;
    endPoint.x = size-2;
    endPoint.y = size-2;

    tlCorner.x = 1;
    tlCorner.y = 1;
    brCorner.x = size-2;
    brCorner.y = size-2;

    char** maze = mallocMaze( size );
    fillMazeWithSymbol( maze, createPoint(0, 0), createPoint(size-1, size-1), 'X' );

    /* create a simple path to connect startPoint to endPoint */
    *longestPathLength = createSimplePathBetweenPoints( maze, startPoint, endPoint, tlCorner, brCorner, 70, ' ', presult );

    createMazeBorder( maze, 0, size-1 );

    maze[startPoint.x][startPoint.y] = 'S';
    maze[endPoint.x][endPoint.y] = 'F';

    return maze;
}

void addDeadEnds( char** maze, int size ){
    int i, j;

    for( i=1; i<size-1; i++ )
        for( j=1; j<size-1; j++ ){
            if( maze[i][j]=='X' )
                if( (maze[i-1][j]=='X' && maze[i+1][j]=='X' && maze[i][j-1]=='X') ||
                    (maze[i-1][j]=='X' && maze[i+1][j]=='X' && maze[i][j+1]=='X') ||
                    (maze[i-1][j]=='X' && maze[i][j-1]=='X' && maze[i][j+1]=='X') ||
                    (maze[i+1][j]=='X' && maze[i][j-1]=='X' && maze[i][j+1]=='X') )
                    maze[i][j] = ' ';
        }
}

//Create space for a maze with length and width = size
char** mallocMaze( int size )
{
    int i;
    char **maze = (char**)malloc(size*sizeof(char*));

    for( i=0; i<size; i++ )
        maze[i] = (char*)malloc(size*sizeof(char));

    return maze;
}

//Fill from the startPoint to the endPoint of the maze with symbol c
void fillMazeWithSymbol( char **maze, Point2D startPoint, Point2D endPoint, char c )
{
    int i, j;
    int xdir = (startPoint.x <= endPoint.x) ? 1: -1;
    int ydir = (startPoint.y <= endPoint.y) ? 1: -1;

    for( i=startPoint.x; i<=endPoint.x; i+=xdir )
        for( j=startPoint.y; j<=endPoint.y; j+=ydir )
            maze[i][j] = c;
}

//Create a border of 'X' around the maze
void createMazeBorder( char **maze, int start, int end )
{
    int i,j;

    for( i=start; i<=end; i++)
        for( j=start; j<=end; j++)
            if( i==start || i==end || j==start || j==end)
                maze[i][j] = 'X';
}

int createPathBetweenPoints( char **maze, Point2D startP, Point2D endP, Point2D tlCorner, Point2D brCorner, int probability, char c )
{
    int xdir, ydir;
    int numSteps = 0;
    Point2D curP = startP;

    if( median( tlCorner.x, startP.x, brCorner.x )!=startP.x || median( tlCorner.y, startP.y, brCorner.y )!=startP.y ||
        median( tlCorner.x, endP.x, brCorner.x )!=endP.x || median( tlCorner.y, endP.y, brCorner.y )!=endP.y ){
        printf("ERROR - createPathBetweenPoints - createPathBetweenPoints unable to link startP to endP\n");
        exit(-1);
    }


    while( !equals( curP, endP ) )
    {

        xdir = (endP.x-curP.x)!=0 ? (endP.x-curP.x)/abs(endP.x-curP.x) : 0;
        ydir = (endP.y-curP.y)!=0 ? (endP.y-curP.y)/abs(endP.y-curP.y) : 0;

        if( rand()%100>=probability )
            xdir = replaceZero(xdir) ;
        if( rand()%100>=probability )
            ydir = replaceZero(ydir) ;

        if( xdir!=0 && median( tlCorner.x, curP.x+xdir, brCorner.x ) == curP.x+xdir ){
            curP.x += xdir;
            numSteps++;
        }

        maze[curP.x][curP.y] = c;

        if( ydir!=0 && median( tlCorner.y, curP.y+ydir, brCorner.y ) == curP.y+ydir ){
            curP.y += ydir;
            numSteps++;
        }

        maze[curP.x][curP.y] = c;
    }
    return numSteps;
}

int createSimplePathBetweenPoints( char **maze, Point2D startP, Point2D endP, Point2D tlCorner, Point2D brCorner, int probability, char c, pathResult *presult )
{
    int xdir, ydir;
    int numSteps = 0;
    Point2D curP = startP;

    if( median( tlCorner.x, startP.x, brCorner.x )!=startP.x || median( tlCorner.y, startP.y, brCorner.y )!=startP.y ||
        median( tlCorner.x, endP.x, brCorner.x )!=endP.x || median( tlCorner.y, endP.y, brCorner.y )!=endP.y ){
        printf("ERROR - createSimplePathBetweenPoints - createPathBetweenPoints unable to link startP to endP\n");
        exit(-1);
    }

    maze[startP.x][startP.y] = ' ';

    while( abs(curP.x-endP.x)+abs(curP.y-endP.y)>1 )
    {
        if( (maze[curP.x-1][curP.y] == c || curP.x-1<tlCorner.x) && (maze[curP.x+1][curP.y] == c || curP.x+1>brCorner.x) &&
            (maze[curP.x][curP.y-1] == c|| curP.y-1<tlCorner.y) && (maze[curP.x][curP.y+1] == c || curP.y+1>brCorner.y) )
            {
                *presult = PATH_IMPOSSIBLE;
                return -1;
            }

        xdir = (endP.x-curP.x)!=0 ? (endP.x-curP.x)/abs(endP.x-curP.x) : 0;
        ydir = (endP.y-curP.y)!=0 ? (endP.y-curP.y)/abs(endP.y-curP.y) : 0;

        if( rand()%100>=probability )
            xdir = replaceZero(xdir) ;
        if( rand()%100>=probability )
            ydir = replaceZero(ydir) ;

        if( xdir!=0 && median( tlCorner.x, curP.x+xdir, brCorner.x ) == curP.x+xdir && maze[curP.x+xdir][curP.y]!=c ){
            curP.x += xdir;
            numSteps++;
        }

        maze[curP.x][curP.y] = c;

        if( abs(curP.x-endP.x)+abs(curP.y-endP.y)<=1 )
            break;

        if( ydir!=0 && median( tlCorner.y, curP.y+ydir, brCorner.y ) == curP.y+ydir && maze[curP.x][curP.y+ydir]!=c ){
            curP.y += ydir;
            numSteps++;
        }

        maze[curP.x][curP.y] = c;
    }
    maze[endP.x][endP.y] = c;
    *presult = PATH_FOUND;
    return numSteps+abs(curP.x-endP.x)+abs(curP.y-endP.y);
}

int replaceZero( int dir ){
    if( dir==0 )
        dir = rand()%2 ? 1 : -1;
    return -dir;
}

int median( int a, int b, int c ){
        return a+b+c-min(min(a,b),c)-max(max(a,b),c);
}

int min( int a, int b ){
    if( a<=b )
        return a;
    else
        return b;
}

int max( int a, int b ){
    if( a>=b )
        return a;
    else
        return b;
}

void printMaze( char **maze, int size )
{
    int i, j;

    for( i=0; i<size; i++ ){
        for( j=0; j<size; j++ )
            printf("%c", maze[i][j]);
        printf("\n");
    }
    printf("\n");
}

void freeMaze( char **maze, int size )
{
    int i;

    for( i=0; i<size; i++ )
        free(maze[i]);
    free( maze );
}
