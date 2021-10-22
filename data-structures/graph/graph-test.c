/**
   graph-test.c

   Tests of graphs with generic integer vertices and generic contiguous
   weights.

   The following command line arguments can be used to customize tests:
   graph-test
      [0, size_t width / 2] : n for 2**n vertices in smallest graph
      [0, size_t width / 2] : n for 2**n vertices in largest graph
      [0, 1] : small graph test on/off
      [0, 1] : non-random graph test on/off
      [0, 1] : random graph test on/off

   usage examples: 
   ./graph-test
   ./graph-test 10 14
   ./graph-test 0 10 0 1 0
   ./graph-test 14 14 0 0 1

   graph-test can be run with any subset of command line arguments in the
   above-defined order. If the (i + 1)th argument is specified then the ith
   argument must be specified for i >= 0. Default values are used for the
   unspecified arguments according to the C_ARGS_DEF array.

   The implementation of tests does not use stdint.h and is portable under
   C89/C90 and C99 with the only requirement that width of size_t is even
   and less than 2040.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "graph.h"
#include "stack.h"
#include "utilities-mem.h"
#include "utilities-mod.h"
#include "utilities-lim.h"

/**
   Generate random numbers in a portable way for test purposes only; rand()
   in the Linux C Library uses the same generator as random(), which may not
   be the case on older rand() implementations, and on current
   implementations on different systems.
*/
#define RGENS_SEED() do{srand(time(NULL));}while (0)
#define RANDOM() (rand()) /* [0, RAND_MAX] */
#define DRAND() ((double)rand() / RAND_MAX) /* [0.0, 1.0] */

#define TOLU(i) ((unsigned long int)(i)) /* printing size_t under C89/C90 */

/* input handling */
const char *C_USAGE =
  "graph-test \n"
  "[0, size_t width / 2] : n for 2**n vertices in smallest graph \n"
  "[0, size_t width / 2] : n for 2**n vertices in largest graph \n"
  "[0, 1] : small graph test on/off \n"
  "[0, 1] : non-random graph test on/off \n"
  "[0, 1] : random graph test on/off \n";
const int C_ARGC_MAX = 6;
const size_t C_ARGS_DEF[5] = {0u, 10u, 1u, 1u, 1u};
const size_t C_FULL_BIT = UINT_WIDTH_FROM_MAX((size_t)-1);

/* small graph tests */
const size_t C_NUM_VTS = 5u;
const size_t C_NUM_ES = 4u;
const unsigned char C_UCHAR_U[4] = {0u, 0u, 0u, 1u};
const unsigned char C_UCHAR_V[4] = {1u, 2u, 3u, 3u};
const unsigned char C_UCHAR_WTS[4] = {4u, 3u, 2u, 1u};
const unsigned long C_ULONG_U[4] = {0u, 0u, 0u, 1u};
const unsigned long C_ULONG_V[4] = {1u, 2u, 3u, 3u};
const unsigned long C_ULONG_WTS[4] = {4u, 3u, 2u, 1u};
const double C_DOUBLE_WTS[4] = {4.0, 3.0, 2.0, 1.0};

const size_t C_FN_COUNT = 4u;
size_t (* const C_READ[4])(const void *) ={
  graph_read_ushort,
  graph_read_uint,
  graph_read_ulong,
  graph_read_sz};
void (* const C_WRITE[4])(void *, size_t) ={
  graph_write_ushort,
  graph_write_uint,
  graph_write_ulong,
  graph_write_sz};
void (* const C_INCR[4])(void *) ={
  graph_incr_ushort,
  graph_incr_uint,
  graph_incr_ulong,
  graph_incr_sz};
stack_t *(* const C_AT[4])(stack_t * const *, const void *) ={
  graph_at_ushort,
  graph_at_uint,
  graph_at_ulong,
  graph_at_sz};
int (* const C_CMPAT[4])(const void *, const void *, const void *) ={
  graph_cmpat_ushort,
  graph_cmpat_uint,
  graph_cmpat_ulong,
  graph_cmpat_sz};
void (* const C_ASSIGN[4])(void *, const void *, const void *) ={
  graph_assign_ushort,
  graph_assign_uint,
  graph_assign_ulong,
  graph_assign_sz};
const size_t C_VT_SIZES[4] = {
  sizeof(unsigned short),
  sizeof(unsigned int),
  sizeof(unsigned long),
  sizeof(size_t)};
const char *C_VT_TYPES[4] = {"ushort", "uint  ", "ulong ", "sz    "};
const double C_PROB_ONE = 1.0;
const double C_PROB_HALF = 0.5;
const double C_PROB_ZERO = 0.0;

void print_uchar(const void *a);
void print_ulong(const void *a);
void print_double(const void *a);
void print_adj_lst(const adj_lst_t *a,
		   void (*print_vt)(const void *),
		   void (*print_wt)(const void *));
size_t sum_vts(const adj_lst_t *a, size_t i);
void print_test_result(int res);

/** 
   Test on small graphs.
*/

/**
   Initialize small graphs with unsigned char vertices and 
   unsigned char, unsigned long, and double weights.
*/

void uchar_uchar_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned char),
		  sizeof(unsigned char),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned char *)C_UCHAR_U;
  g->v = (unsigned char *)C_UCHAR_V;
  g->wts = (unsigned char *)C_UCHAR_WTS;
}

void uchar_ulong_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned char),
		  sizeof(unsigned long),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned char *)C_UCHAR_U;
  g->v = (unsigned char *)C_UCHAR_V;
  g->wts = (unsigned long *)C_ULONG_WTS;
}

void uchar_double_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned char),
		  sizeof(double),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned char *)C_UCHAR_U;
  g->v = (unsigned char *)C_UCHAR_V;
  g->wts = (double *)C_DOUBLE_WTS;
}

/**
   Initialize small graphs with unsigned long vertices and 
   unsigned char, unsigned long, and double weights.
*/

void ulong_uchar_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned long),
		  sizeof(unsigned char),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned long *)C_ULONG_U;
  g->v = (unsigned long *)C_ULONG_V;
  g->wts = (unsigned char *)C_UCHAR_WTS;
}

void ulong_ulong_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned long),
		  sizeof(unsigned long),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned long *)C_ULONG_U;
  g->v = (unsigned long *)C_ULONG_V;
  g->wts = (unsigned long *)C_ULONG_WTS;
}

void ulong_double_graph_init(graph_t *g, const graph_vto_t *vto){
  graph_base_init(g,
		  C_NUM_VTS,
		  sizeof(unsigned long),
		  sizeof(double),
		  vto);
  g->num_es = C_NUM_ES;
  g->u = (unsigned long *)C_ULONG_U;
  g->v = (unsigned long *)C_ULONG_V;
  g->wts = (double *)C_DOUBLE_WTS;
}

/**
   Runs a test of adj_lst_{init, dir_build, undir_build, free} on 
   small graphs.
*/
void run_small_graph_test(){
  graph_t g;
  graph_vto_t vto_uchar;
  graph_vto_t vto_ulong;
  adj_lst_t a;
  vto_uchar.read = graph_read_uchar;
  vto_uchar.write = graph_write_uchar;
  vto_uchar.incr = graph_incr_uchar;
  vto_uchar.at = graph_at_uchar;
  vto_uchar.cmpat = graph_cmpat_uchar;
  vto_uchar.assign = graph_assign_uchar;
  vto_ulong.read = graph_read_ulong;
  vto_ulong.write = graph_write_ulong;
  vto_ulong.incr = graph_incr_ulong;
  vto_ulong.at = graph_at_ulong;
  vto_ulong.cmpat = graph_cmpat_ulong;
  vto_ulong.assign = graph_assign_ulong;
  /* unsigned char vertices, unsigned char weights */
  uchar_uchar_graph_init(&g, &vto_uchar);
  printf("uchar vertices, uchar weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_uchar);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_uchar);
  adj_lst_free(&a);
  /* unsigned char vertices, unsigned long weights */
  uchar_ulong_graph_init(&g, &vto_uchar);
  printf("uchar vertices, ulong weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_ulong);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_ulong);
  adj_lst_free(&a);
  /* unsigned char vertices, double weights */
  uchar_double_graph_init(&g, &vto_uchar);
  printf("uchar vertices, double weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_double);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_uchar, print_double);
  adj_lst_free(&a);

  /* unsigned long vertices, unsigned char weights */
  ulong_uchar_graph_init(&g, &vto_ulong);
  printf("ulong vertices, uchar weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_uchar);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_uchar);
  adj_lst_free(&a);
  /* unsigned long vertices, unsigned long weights */
  ulong_ulong_graph_init(&g, &vto_ulong);
  printf("ulong vertices, ulong weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_ulong);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_ulong);
  adj_lst_free(&a);
  /* unsigned long vertices, double weights */
  ulong_double_graph_init(&g, &vto_ulong);
  printf("ulong vertices, double weights\n");
  printf("\tdirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_dir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_double);
  adj_lst_free(&a);
  printf("\tundirected\n");
  adj_lst_base_init(&a, &g);
  adj_lst_undir_build(&a, &g);
  print_adj_lst(&a, print_ulong, print_double);
  adj_lst_free(&a);
}

/**
   Test on non-random graphs.
*/

/**
   Initialize and free unweighted graphs. Each graph is i) a DAG with source
   0 and num_vts(num_vts - 1) / 2 edges in the directed form, and ii)
   complete in the undirected form. num_vts is >= 1.
*/

void complete_graph_init(graph_t *g,
			 size_t num_vts,
			 size_t vt_size,
			 const graph_vto_t *vto){
  size_t i, j;
  size_t num_es = mul_sz_perror(num_vts, num_vts - 1) >> 1;
  char *up = NULL;
  char *vp = NULL;
  graph_base_init(g,
		  num_vts,
		  vt_size,
		  0,
		  vto);
  g->num_es = num_es;
  g->u = malloc_perror(g->num_es, vt_size);
  g->v = malloc_perror(g->num_es, vt_size);
  up = g->u;
  vp = g->v;
  for (i = 0; i < num_vts - 1; i++){
    for (j = i + 1; j < num_vts; j++){
      g->vto->write(up, i);
      g->vto->write(vp, j);
      up += vt_size;
      vp += vt_size;
    }
  }
}

void complete_graph_free(graph_t *g){
  free(g->u);
  free(g->v);
  g->u = NULL;
  g->v = NULL;
}

/**
   Runs a adj_lst_undir_build test on complete unweighted graphs across
   integer types for vertices.
*/
void run_adj_lst_undir_build_test(size_t log_start, size_t log_end){
  size_t i, j;
  size_t num_vts;
  graph_t g;
  graph_vto_t vto;
  adj_lst_t a;
  clock_t t;
  printf("Test adj_lst_undir_build on complete unweighted graphs across"
	 " vertex types\n");
  printf("\tn vertices, n(n - 1)/2 edges represented by n(n - 1) "
	 "directed edges\n");
  for (i = log_start; i <= log_end; i++){
    num_vts = pow_two_perror(i);
    printf("\t\tvertices: %lu\n", TOLU(num_vts));
    for (j = 0; j < C_FN_COUNT; j++){
      vto.read = C_READ[j];
      vto.write = C_WRITE[j];
      vto.incr = C_INCR[j];
      vto.at = C_AT[j];
      vto.cmpat = C_CMPAT[j];
      vto.assign = C_ASSIGN[j];
      complete_graph_init(&g, pow_two_perror(i), C_VT_SIZES[j], &vto);
      adj_lst_base_init(&a, &g);
      t = clock();
      adj_lst_undir_build(&a, &g);
      t = clock() - t;
      adj_lst_free(&a);
      complete_graph_free(&g);
      printf("\t\t\t%s build time:      %.6f seconds\n",
	     C_VT_TYPES[j], (float)t / CLOCKS_PER_SEC);
    }
  }
}

/**
   Test on random graphs.
*/

typedef struct{
  double p;
} bern_arg_t;

int bern(void *arg){
  bern_arg_t *b = arg;
  if (b->p >= C_PROB_ONE) return 1;
  if (b->p <= C_PROB_ZERO) return 0;
  if (b->p > DRAND()) return 1;
  return 0;
}

/**
   Test adj_lst_add_dir_edge and adj_lst_add_undir_edge.
*/

void add_edge_helper(size_t log_start,
		     size_t log_end,
		     void (*build)(adj_lst_t *,
				   const graph_t *),
		     void (*add_edge)(adj_lst_t *,
				      size_t,
				      size_t,
				      const void *,
				      int (*)(void *),
				      void *));

void run_adj_lst_add_dir_edge_test(size_t log_start, size_t log_end){
  printf("Test adj_lst_add_dir_edge on DAGs \n");
  printf("\tn vertices, 0 as source, n(n - 1)/2 directed edges \n");
  add_edge_helper(log_start,
		  log_end,
		  adj_lst_dir_build,
		  adj_lst_add_dir_edge);
}

void run_adj_lst_add_undir_edge_test(size_t log_start, size_t log_end){
  printf("Test adj_lst_add_undir_edge on complete graphs \n");
  printf("\tn vertices, n(n - 1)/2 edges represented by n(n - 1) "
	 "directed edges \n");
  add_edge_helper(log_start,
		  log_end,
		  adj_lst_undir_build,
		  adj_lst_add_undir_edge);
}

void add_edge_helper(size_t log_start,
		     size_t log_end,
		     void (*build)(adj_lst_t *,
				   const graph_t *),
		     void (*add_edge)(adj_lst_t *,
				      size_t,
				      size_t,
				      const void *,
				      int (*)(void *),
				      void *)){
  int res = 1;
  size_t i, j, k, l;
  size_t num_vts;
  bern_arg_t b;
  graph_t g_blt, g_bld;
  graph_vto_t vto;
  adj_lst_t a_blt, a_bld;
  clock_t t;
  b.p = C_PROB_ONE;
  for (i = log_start; i <= log_end; i++){
    num_vts = pow_two_perror(i);
    printf("\t\tvertices: %lu\n", TOLU(num_vts));
    for (j = 0; j < C_FN_COUNT; j++){
      vto.read = C_READ[j];
      vto.write = C_WRITE[j];
      vto.incr = C_INCR[j];
      vto.at = C_AT[j];
      vto.cmpat = C_CMPAT[j];
      complete_graph_init(&g_blt, num_vts, C_VT_SIZES[j], &vto);
      graph_base_init(&g_bld,
		      num_vts,
		      C_VT_SIZES[j],
		      0,
		      &vto);
      adj_lst_base_init(&a_blt, &g_blt);
      adj_lst_base_init(&a_bld, &g_bld);
      build(&a_blt, &g_blt);
      t = clock();
      for (k = 0; k < num_vts - 1; k++){
	for (l = k + 1; l < num_vts; l++){
	  add_edge(&a_bld, k, l, NULL, bern, &b);
	}
      }
      t = clock() - t;
      for (k = 0; k < num_vts; k++){
	res *= (a_blt.vt_wts[k]->num_elts == a_bld.vt_wts[k]->num_elts);
	res *= (sum_vts(&a_blt, k) == sum_vts(&a_bld, k));
      }
      res *= (a_blt.num_vts == a_bld.num_vts);
      res *= (a_blt.num_es == a_bld.num_es);
      complete_graph_free(&g_blt);
      adj_lst_free(&a_blt);
      adj_lst_free(&a_bld);
      printf("\t\t\t%s build time:      %.6f seconds\n",
	     C_VT_TYPES[j], (float)t / CLOCKS_PER_SEC);
    }
  }
  printf("\t\tcorrectness across all builds --> ");
  print_test_result(res);
}

/** 
   Test adj_lst_rand_dir and adj_lst_rand_undir.
*/

void rand_build_helper(size_t log_start,
		       size_t log_end,
		       double prob,
		       void (*rand_build)(adj_lst_t *,
					  int (*)(void *),
					  void *));

void run_adj_lst_rand_dir_test(size_t log_start, size_t log_end){
  printf("Test adj_lst_rand_dir on the number of edges in expectation\n");
  printf("\tn vertices, E[# of directed edges] = n(n - 1) * (%.1f * 1)\n",
	 C_PROB_HALF);
  rand_build_helper(log_start, log_end, C_PROB_HALF, adj_lst_rand_dir);
}

void run_adj_lst_rand_undir_test(size_t log_start, size_t log_end){
  printf("Test adj_lst_rand_undir on the number of edges in expectation\n");
  printf("\tn vertices, E[# of directed edges] = n(n - 1)/2 * (%.1f * 2)\n",
	 C_PROB_HALF);
  rand_build_helper(log_start, log_end, C_PROB_HALF, adj_lst_rand_undir);
}

void rand_build_helper(size_t log_start,
		       size_t log_end,
		       double prob,
		       void (*rand_build)(adj_lst_t *,
					  int (*)(void *),
					  void *)){
  size_t i, j;
  size_t num_vts;
  graph_t g;
  graph_vto_t vto;
  adj_lst_t a;
  bern_arg_t b;
  b.p = prob;
  for (i = log_start; i <= log_end; i++){
    num_vts = pow_two_perror(i);
    printf("\t\tvertices: %lu, expected directed edges: %.1f\n",
	   TOLU(num_vts), prob * num_vts * (num_vts - 1));
    for (j = 0; j < C_FN_COUNT; j++){
      vto.read = C_READ[j];
      vto.write = C_WRITE[j];
      vto.incr = C_INCR[j];
      vto.at = C_AT[j];
      vto.cmpat = C_CMPAT[j];
      vto.assign = C_ASSIGN[j];
      graph_base_init(&g, num_vts, C_VT_SIZES[j], 0, &vto);
      adj_lst_base_init(&a, &g);
      rand_build(&a, bern, &b);
      printf("\t\t\t%s directed edges:   %lu\n",
	     C_VT_TYPES[j],
	     TOLU(a.num_es));
      adj_lst_free(&a);
    }
  }
}

/**
   Auxiliary functions.
*/

/**
   Sums the vertices in the ith stack in an adjacency list. Wraps around and
   does not check for overflow.
*/
size_t sum_vts(const adj_lst_t *a, size_t i){
  char *p = NULL, *p_start = NULL, *p_end = NULL;
  size_t ret = 0;
  p_start = a->vt_wts[i]->elts;
  p_end = p_start + a->vt_wts[i]->num_elts * a->pair_size;
  for (p = p_start; p != p_end; p += a->pair_size){
    ret += a->vto->read(p);
  }
  return ret;
}

/**
   Printing functions.
*/

void print_uchar(const void *a){
  printf("%u ", *(const unsigned char *)a);
}

void print_ulong(const void *a){
  printf("%lu ", *(const unsigned long *)a);
}

void print_double(const void *a){
  printf("%.2f ", *(const double *)a);
}
  
void print_adj_lst(const adj_lst_t *a,
		   void (*print_vt)(const void *),
		   void (*print_wt)(const void *)){
  size_t i;
  char *p = NULL, *p_start = NULL, *p_end = NULL;
  printf("\t\tvertices: \n");
  for (i = 0; i < a->num_vts; i++){
    printf("\t\t%lu : ", TOLU(i));
    p_start = a->vt_wts[i]->elts;
    p_end = p_start + a->vt_wts[i]->num_elts * a->pair_size;
    for (p = p_start; p != p_end; p += a->pair_size){
      print_vt(p);
    }
    printf("\n");
  }
  if (a->wt_size > 0 && print_wt != NULL){
    printf("\t\tweights: \n");
    for (i = 0; i < a->num_vts; i++){
      printf("\t\t%lu : ", TOLU(i));
      p_start = a->vt_wts[i]->elts;
      p_end = p_start + a->vt_wts[i]->num_elts * a->pair_size;
      for (p = p_start; p != p_end; p += a->pair_size){
	print_wt(p + a->wt_offset);
      }
      printf("\n");
    }
  }
}

void print_test_result(int res){
  if (res){
    printf("SUCCESS\n");
  }else{
    printf("FAILURE\n");
  }
}

int main(int argc, char *argv[]){
  int i;
  size_t *args = NULL;
  RGENS_SEED();
  if (argc > C_ARGC_MAX){
    printf("USAGE:\n%s", C_USAGE);
    exit(EXIT_FAILURE);
  }
  args = malloc_perror(C_ARGC_MAX - 1, sizeof(size_t));
  memcpy(args, C_ARGS_DEF, (C_ARGC_MAX - 1) * sizeof(size_t));
  for (i = 1; i < argc; i++){
    args[i - 1] = atoi(argv[i]);
  }
  if (args[0] > C_FULL_BIT / 2 ||
      args[1] > C_FULL_BIT / 2 ||
      args[1] < args[0] ||
      args[2] > 1 ||
      args[3] > 1 ||
      args[4] > 1){
    printf("USAGE:\n%s", C_USAGE);
    exit(EXIT_FAILURE);
  }
  if (args[2]) run_small_graph_test();
  if (args[3]) run_adj_lst_undir_build_test(args[0], args[1]);
  if (args[4]){
    run_adj_lst_add_dir_edge_test(args[0], args[1]);
    run_adj_lst_add_undir_edge_test(args[0], args[1]);
    run_adj_lst_rand_dir_test(args[0], args[1]);
    run_adj_lst_rand_undir_test(args[0], args[1]);
  }
  free(args);
  args = NULL;
  return 0;
}
