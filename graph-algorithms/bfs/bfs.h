/**
   bfs.h

   Declarations of accessible functions for running the BFS algorithm on
   graphs with vertices indexed from 0.
*/

#ifndef BFS_H  
#define BFS_H

#include <stdlib.h>
#include "graph.h"

/**
   Computes and copies to dist the lowest # of edges from start to each 
   reached vertex, and provides the previous vertex in prev, with SIZE_MAX
   in prev for unreached vertices. Assumes start is valid and there is
   at least one vertex.
*/
void bfs(const adj_lst_t *a, size_t start, size_t *dist, size_t *prev);

#endif
