/*
 * csim.c: a cache simulator that takes a valgrind memory trace
 * as input, simulates the hit/miss behavior of a cache memory
 * on this trace, and outputs the total number of hits, misses,
 * and evictions.
 *
 * Name: Eamon Niknafs
 * Email: en@bu.edu
 */
#include "cachelab.h"


/* Define type for cache line */
typedef struct {
    short valid;
    unsigned long long tag;
    unsigned long long block;
} line_t;

/* Define type for cache set */
typedef line_t* set_t;

/* Define type for cache statistics */
typedef struct {
    unsigned int hits = 0;
    unsigned int misses = 0;
    unsigned int evictions = 0;
    unsigned long long int lru_count = 1;
} stats_t;

/* Define type for entire cache */
typedef struct {
    set_t sets;
    stats_t stats;
} cache_t;


/* Define global variables */
cache_t cache;
struct args {
    int verbose = 0;    //verbose flag
    int idx_bits = 0;   //number of set index bits
    int assoc = 0;      //associativity
    int blocks = 0;     //number of block bits
    char* t_addr = 0;     //address to tracefile
};


int main()
{
    printSummary(0, 0, 0);
    return 0;
}
