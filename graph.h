#ifndef _graph_h
#define _graph_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "hashTable.h"
#include "point2D.h"

typedef struct Point2D graphType;
typedef enum adjType{ MATRIX_TYPE, LIST_TYPE } adjType;

typedef struct AdjacencyList
{
    int vertexIndex;
    struct AdjacencyList* nextListEntry;
}  AdjacencyList;

typedef struct Vertex
{
    graphType data;
    bool visited;   /* store whether this vertex has been visited */
    int *distance;  /* distance from this vertex to all other vertices */

    /* if type is MATRIX_TYPE the successors are stored in the matrix at this index */
    int index;
    int currentSuccessorIndex;
    int currentPredecessorIndex;

    /* if type is LIST_TYPE the edges are stored in this node */
    AdjacencyList* successorList;
    AdjacencyList* predecessorList;
    AdjacencyList* currentSuccessorElement;
    AdjacencyList* currentPredecessorElement;
}  Vertex;

typedef struct Graph
{
    hashTable* vertexTable;
    Vertex* vertexArray;
    int numVertices;
    int capacity;
    adjType type;

    /* if type is MATRIX_TYPE the edges are stored in this matrix */
    bool** adjacencyMatrix;

}  Graph;


Graph* createGraph( int numVertex, adjType type );
void freeGraph( Graph* g );

void addVertex( Graph* g, graphType p );
bool isVertex( Graph* g, graphType p );
bool getVertexByIndex( Graph* g, int index, graphType* next );

void setEdge( Graph* g, graphType p1, graphType p2, bool value );
bool getEdge( Graph* g, graphType p1, graphType p2 );

void resetSuccessor( Graph* g, graphType start );
bool getSuccessor( Graph* g, graphType start, graphType* pnext );
bool getPredecessor( Graph* g, graphType start, graphType* pprev );

void setVisited( Graph* g, graphType p, bool value );
bool getVisited( Graph* g, graphType p );

void setDistance( Graph* g, graphType from, graphType to, int dist );
int getDistance( Graph* g, graphType from, graphType to );

#endif
