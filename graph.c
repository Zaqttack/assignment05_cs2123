#include "graph.h"

void freeAdjList( AdjacencyList* pHead );
int getIndex( Graph* g, graphType p );

/* create
 * input: an int representing the maximum number of vertices needed in your graph, an adjType of either MATRIX_TYPE or LIST_TYPE
 * output: a pointer to a Graph (this is malloc-ed so must be freed eventually)
 *
 * Creates a new empty Graph and returns a pointer to it.  The maximum number of vertices is set to capacity.
 * The graph will use either an adjacency matrix or an adjacency list depending on the value given for adjType.
 */
Graph* createGraph( int capacity, adjType type )
{
    int i,j;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->vertexArray = (Vertex*)malloc(sizeof(Vertex)*capacity);
    g->vertexTable = createTable( capacity, FIBONACCI_HASH, false );
    g->type = type;

    if( g->type == MATRIX_TYPE ){
        g->adjacencyMatrix = (bool**)malloc(sizeof(bool*)*capacity);

        for( i=0; i<capacity; i++ )
            g->adjacencyMatrix[i] = (bool*)malloc(sizeof(bool)*capacity);

        for( i=0; i<capacity; i++ )
            for( j=0; j<capacity; j++ )
                g->adjacencyMatrix[i][j] = false;
    }


    g->numVertices = 0;
    g->capacity = capacity;
    return g;
}

/* freeGraph and freeAdjList
 * input: a pointer to a Graph
 * output: none
 *
 * frees the given Graph and all of it's Vertex elements
 */
void freeGraph( Graph* g )
{
    int i;

    if( g==NULL ){
        printf("ERROR - freeGraph - Attempt to free NULL Graph pointer\n");
        exit(-1);
    }

    if( g->type == MATRIX_TYPE ){
        for( i=0; i<g->capacity; i++ )
            free(g->adjacencyMatrix[i]);

        free(g->adjacencyMatrix);
    }

    /* Free adjacency lists */
    else if( g->type == LIST_TYPE ){
        for( i=0; i<g->numVertices; i++ ){
            freeAdjList( g->vertexArray[i].successorList );
            freeAdjList( g->vertexArray[i].predecessorList );
        }
    }

    for( i=0; i<g->numVertices; i++ ){
        free( g->vertexArray[i].distance );
    }

    /* Free vertex table */
    freeTable( g->vertexTable );

    /* Free vertex array */
    free( g->vertexArray );
    free( g );
}

void freeAdjList( AdjacencyList* pHead ){
    if( pHead==NULL )
        return;

    freeAdjList( pHead->nextListEntry );
    free( pHead );
}

/* addVertex
 * input: a pointer to a Graph, a graphType p
 * output: none
 *
 * adds a new vertex to the graph with its data set to p
 */
void addVertex( Graph* g, graphType p ){
    int i = getIndex( g, p );
    if( g->numVertices==g->capacity ){
        printf("ERROR - addVertex - Attempt to add more vertices than the capacity of the graph\n");
        exit(-1);
    }

    /* Check if vertex already in graph */
    if( i!=-1 ){
        printf("WARNING - addVertex - Attempt to add same graphType to graph multiple times\n");
        return;
    }

    g->vertexArray[g->numVertices].data = p;
    g->vertexArray[g->numVertices].index = g->numVertices;
    g->vertexArray[g->numVertices].visited = false;
    g->vertexArray[g->numVertices].distance = (int *)malloc(sizeof(int)*g->capacity);

    for( i=0; i<g->capacity; i++){
        g->vertexArray[g->numVertices].distance[i] = INT_MAX;
    }

    insertTable( g->vertexTable, getKey( p ), &g->vertexArray[g->numVertices] );

    if( g->type == MATRIX_TYPE ){
        g->vertexArray[g->numVertices].currentSuccessorIndex = -1;
        g->vertexArray[g->numVertices].currentPredecessorIndex = -1;
    }
    else if( g->type == LIST_TYPE ){
        g->vertexArray[g->numVertices].successorList = NULL;
        g->vertexArray[g->numVertices].currentSuccessorElement = NULL;
        g->vertexArray[g->numVertices].predecessorList = NULL;
        g->vertexArray[g->numVertices].currentPredecessorElement = NULL;
    }

    g->numVertices++;
}

/* isVertex
 * input: a pointer to a Graph, a graphType p
 * output: a bool
 *
 * returns true if the given graphType is contained in the graph
 */
bool isVertex( Graph* g, graphType p ){
    return getIndex( g, p )!=-1;
}

/* getVertexByIndex
 * input: a pointer to a Graph, an int, a pointer to a graphType
 * output: a bool
 *
 * finds index-th vertex added to the graph and inserts into next.  It returns true if this search was successful and false otherwise.
 */
bool getVertexByIndex( Graph* g, int index, graphType* next ){
    if( index>=g->numVertices )
        return false;
    *next = g->vertexArray[index].data;
    return true;
}

/* getIndex
 * input: a pointer to a Graph, a graphType
 * output: a bool
 *
 * finds index-th vertex added to the graph and inserts into next.  It returns true if this search was successful and false otherwise.
 */
int getIndex( Graph* g, graphType p ){
    Vertex* v = (Vertex *)searchTable( g->vertexTable, getKey(p) );
    if( v==NULL )
        return -1;
    return v->index;
}

/* setEdge
 * input: a pointer to a Graph, two graphType variables, a bool
 * output: none
 *
 * Sets the specified edge in the graph to the bool value
 */
void setEdge( Graph* g, graphType p1, graphType p2, bool value )
{
    int i = getIndex( g, p1 );
    int j = getIndex( g, p2 );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        addVertex( g, p1 );
        i = getIndex( g, p1 );
    }
    if( j==-1 ){
        addVertex( g, p2 );
        j = getIndex( g, p2 );
    }

    if( g->type == MATRIX_TYPE )
        g->adjacencyMatrix[i][j] = value;
    else if( g->type == LIST_TYPE && value==true ){
        AdjacencyList* sucListEntry  = (AdjacencyList*)malloc(sizeof(AdjacencyList));
        AdjacencyList* prevListEntry = (AdjacencyList*)malloc(sizeof(AdjacencyList));

        /* add new element to front of successorList */
        sucListEntry->nextListEntry = g->vertexArray[i].successorList;
        g->vertexArray[i].successorList = sucListEntry;
        sucListEntry->vertexIndex = j;

        /* add new element to front of predecessorList */
        prevListEntry->nextListEntry = g->vertexArray[j].predecessorList;
        g->vertexArray[j].predecessorList = prevListEntry;
        prevListEntry->vertexIndex = i;
    }
    else if( g->type == LIST_TYPE && getEdge(g, p1, p2)==true ){
        printf("WARNING - setEdge - Removing edges is not currently implemented for LIST_TYPE.  Use MATRIX_TYPE.\n");
    }
}

/* getEdge
 * input: a pointer to a Graph, two graphType variables
 * output: a bool
 *
 * Returns whether the specified edge is in the graph
 */
bool getEdge( Graph* g, graphType p1, graphType p2 )
{
    int i = getIndex( g, p1 );
    int j = getIndex( g, p2 );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        addVertex( g, p1 );
        i = getIndex( g, p1 );
    }
    if( j==-1 ){
        addVertex( g, p2 );
        j = getIndex( g, p2 );
    }

    if( g->type == MATRIX_TYPE )
        return g->adjacencyMatrix[i][j];
    else if( g->type == LIST_TYPE ){
        AdjacencyList* listEntry = g->vertexArray[i].successorList;
        while( listEntry!=NULL && !equals(g->vertexArray[listEntry->vertexIndex].data,p2) )
            listEntry = listEntry->nextListEntry;
        return listEntry!=NULL;
    }
    else{
        printf("ERROR - getEdge - Invalid graph type\n");
        exit(-1);
    }
}

/* getSuccessor
 * input: a pointer to a Graph, a graphType 'start', a pointer to a graphType 'pnext'
 * output: a bool and the next successor of start is stored at 'pnext'
 *
 * Finds the next node reachable from 'start'.  It stores the destination of this edge into location pointed to by 'pnext'.
 * If a successor exists it returns true and false otherwise.
 */
bool getSuccessor( Graph* g, graphType start, graphType* pnext )
{
    int i = getIndex( g, start );
    int j;


    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        addVertex( g, start );
        i = getIndex( g, start );
    }
    else if( pnext==NULL ){
        printf("ERROR - getSuccessor - Attempt to call getSuccessor with NULL pnext\n");
        exit(-1);
    }

    Vertex* v = &g->vertexArray[i];

    if( g->type == MATRIX_TYPE ){
        j = v->currentSuccessorIndex;
        for( j=j+1; j<g->numVertices; j++ ){
            if( g->adjacencyMatrix[i][j]==true ){
                *pnext = g->vertexArray[j].data;
                v->currentSuccessorIndex = j;
                return true;
            }
        }
        v->currentSuccessorIndex = -1;
        return false;
    }
    else if( g->type == LIST_TYPE ){
        if( v->currentSuccessorElement == NULL && v->successorList == NULL )
            return false;
        else if( v->currentSuccessorElement == NULL )
            v->currentSuccessorElement = v->successorList;
        else
            v->currentSuccessorElement = v->currentSuccessorElement->nextListEntry;

        if( v->currentSuccessorElement == NULL )
            return false;

        *pnext = g->vertexArray[ v->currentSuccessorElement->vertexIndex ].data;
        return true;
    }
    else{
        printf("ERROR - Invalid graph type\n");
        exit(-1);
    }
}

/* getPredecessor
 * input: a pointer to a Graph, a graphType 'start', a pointer to a graphType 'pprev'
 * output: a bool and the next node with an edge going to start is stored at 'pprev'
 *
 * Finds the next node which has an edge going to 'start'.  It stores the destination of this edge into location pointed to by 'pprev'.
 * If a predecessor exists it returns true and false otherwise.
 */
bool getPredecessor( Graph* g, graphType start, graphType* pprev ){
    int i;
    int j = getIndex( g, start );


    /* Add vertices if they're not already in graph */
    if( j==-1 ){
        addVertex( g, start );
        j = getIndex( g, start );
    }
    else if( pprev==NULL ){
        printf("ERROR - getPredecessor - Attempt to call getSuccessor with NULL pprev\n");
        exit(-1);
    }

    Vertex* v = &g->vertexArray[j];

    if( g->type == MATRIX_TYPE ){
        i = v->currentPredecessorIndex;
        for( i=i+1; i<g->numVertices; i++ ){
            if( g->adjacencyMatrix[i][j]==true ){
                *pprev = g->vertexArray[i].data;
                v->currentPredecessorIndex= i;
                return true;
            }
        }
        v->currentPredecessorIndex = -1;
        return false;
    }
    else if( g->type == LIST_TYPE ){
        if( v->currentPredecessorElement == NULL && v->predecessorList == NULL )
            return false;
        else if( v->currentPredecessorElement == NULL )
            v->currentPredecessorElement = v->predecessorList;
        else
            v->currentPredecessorElement = v->currentPredecessorElement->nextListEntry;

        if( v->currentPredecessorElement == NULL )
            return false;

        *pprev = g->vertexArray[ v->currentPredecessorElement->vertexIndex ].data;
        return true;
    }
    else{
        printf("ERROR - Invalid graph type\n");
        exit(-1);
    }
}

/* setVisited
 * input: a pointer to a Graph, a graphType, a bool value
 * output: none
 *
 * Sets the visited status of the vertex with the given graphType to 'value'
 */
void setVisited( Graph* g, graphType p, bool value ){
    int i = getIndex( g, p );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - setVisited - Attempt to set visited for a graphType not in graph \n");
        addVertex( g, p );
        i = getIndex( g, p );
    }

    g->vertexArray[i].visited = value;
}

/* getVisited
 * input: a pointer to a Graph, a graphType
 * output: none
 *
 * Gets the visited status of the vertex with the given graphType
 */
bool getVisited( Graph* g, graphType p ){
    int i = getIndex( g, p );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getVisited - Attempt to get visited for a graphType not in graph \n");
        addVertex( g, p );
        i = getIndex( g, p );
    }

    return g->vertexArray[i].visited;
}

/* setDistance
 * input: a pointer to a Graph, two graphTypes, an int
 * output: none
 *
 * Sets the distance from the vertex 'from' to the vertex 'to' equal to 'dist'.
 */
void setDistance( Graph* g, graphType from, graphType to, int dist ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getVisited - Attempt to set distance for a graphType not in graph \n");
        addVertex( g, from );
        i = getIndex( g, from );
    }
    if( j==-1 ){
        printf("WARNING - getVisited - Attempt to set distance for a graphType not in graph \n");
        addVertex( g, to );
        j = getIndex( g, to );
    }

    g->vertexArray[i].distance[j] = dist;
}

/* getDistance
 * input: a pointer to a Graph, two graphTypes
 * output: an int
 *
 * Returns the distance from the vertex 'from' to the vertex 'to'.
 */
int getDistance( Graph* g, graphType from, graphType to ){
    int i = getIndex( g, from );
    int j = getIndex( g, to );

    /* Add vertices if they're not already in graph */
    if( i==-1 ){
        printf("WARNING - getDistance - Attempt to get distance for a graphType not in graph \n");
        addVertex( g, from );
        i = getIndex( g, from );
    }
    if( j==-1 ){
        printf("WARNING - getDistance - Attempt to get distance for a graphType not in graph \n");
        addVertex( g, to );
        j = getIndex( g, to );
    }

    return g->vertexArray[i].distance[j];
}
