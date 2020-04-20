#include "graph.h"
#include "graphPathAlg.h"

/* hasPath
 * input: a **char pointer to a maze, an int that represents the side length of the maze
 * output: pathResult
 *
 * Detects whether a path exists from 'S' to 'F' in the graph ('X' marks impassable regions)
 */
pathResult hasPath( char** maze, int mazeSize ){
    //TODO: Complete this algorithm

    return PATH_UNKNOWN;
}

/* findNearestFinish
 * input: a **char pointer to a maze, an int that represents the side lengths of the maze, a pointer to an int
 * output: pathResult
 *
 * The maze contains one 'S' and multiple 'F's (1 or more).  'X' marks impassable regions.
 * Find the length of the shortest path to any 'F' from 'S' and return it in spDist.
 * If no 'F' is reachable set spDist to INT_MAX.
 */
pathResult findNearestFinish( char** maze, int mazeSize, int *spDist ){
    //TODO: Complete this algorithm

    (*spDist) = INT_MAX;
    return PATH_UNKNOWN;
}

/* findLongestSimplePath
 * input: a **char pointer to a maze, an int that represents the side lengths of the maze, a pointer to an int
 * output: pathResult
 *
 * The maze contains one 'S' and one 'F'.  'X' marks impassable regions.
 * Find the length of the longest simple path to 'F' from 'S' and return it in lpDist (simple paths are those that do not visit any vertex twice).
 * If 'F' is not reachable set lpDist to -1.
 */
pathResult findLongestSimplePath( char** maze, int mazeSize, int *lpDist ){
    //TODO: Complete this algorithm

    (*lpDist) = -1;
    return PATH_UNKNOWN;
}

