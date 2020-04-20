#ifndef _graphPathAlg_h
#define _graphPathAlg_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "graph.h"
#include "point2D.h"
#include "stackPoint2D.h"
#include "queuePoint2D.h"
#include "priorityQueuePoint2D.h"

typedef enum pathResult{ PATH_IMPOSSIBLE, PATH_FOUND, PATH_UNKNOWN } pathResult;

typedef struct MazeData
{
    /* Optional TODO - put your data for finding paths through the maze in here to help you pass it between helper functions */
    Graph *g;

}  MazeData;

pathResult hasPath( char** maze, int mazeSize );
pathResult findNearestFinish( char** maze, int mazeSize, int *spDist );
pathResult findLongestSimplePath( char** maze, int mazeSize, int *lpDist );

#endif
