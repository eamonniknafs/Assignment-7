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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* Define type for cache line */
typedef struct {
    short valid;
    unsigned long long tag;
    unsigned long long block;
    unsigned long long int lru;
} line_t;

/* Define type for cache set */
typedef line_t *set_t;

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

/* Define type for args */
typedef struct {
    int verbose = 0;    //verbose flag
    int idx_bits = 0;   //number of set index bits
    int assoc = 0;      //associativity
    int blocks = 0;     //number of block bits
    char* t_addr = 0;     //address to tracefile
} args_t;


/* Define global variables */
cache_t cache;
args_t args;


int main(int argc, char **argv)
{
    /* Gets args from command line */
    //help from: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    while ((arg = getopt (argc, argv, "s:E:b:t:vh")) != -1){
        switch (arg){
        case 's':
            args.idx_bits = atoi(optarg);
            break;
        case 'E':
            args.assoc = atoi(optarg);
            break;
        case 'b':
            args.blocks = atoi(optarg);
            break;
        case 't':
            args.t_addr = optarg;
            break;
        case 'v':
            args.verbose = 1;
            break;
        case 'h':
            help(argv);
            exit(0);
        default:
            help(argv);
            exit(1);
        }
    }
    printSummary(0, 0, 0);
    return 0;
}
