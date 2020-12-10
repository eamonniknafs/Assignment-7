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
    int idx_bits = 0;   //number of set index bits (S = 2^s is the number of sets)
    int assoc = 0;      //associativity (number of lines per set)
    int block_bits = 0; //number of block bits (B = 2^b is the block size)
    char* t_addr = 0;   //address to tracefile
} args_t;


/* Define global variables */
cache_t cache;
args_t args;
int num_sets;
int block_size;


/*
 * help: prints info for command line usage
 */
void help(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


int main(int argc, char *argv[])
{
    /* Gets args from command line */
    //help from: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    int arg;
    while ((arg = getopt (argc, argv, "s:E:b:t:vh")) != -1){
        switch (arg){
        case 's':
            args.idx_bits = atoi(optarg); //number of set index bits (S = 2^s is the number of sets)
            break;
        case 'E':
            args.assoc = atoi(optarg); //associativity (number of lines per set)
            break;
        case 'b':
            args.block_bits = atoi(optarg); //number of block bits (B = 2^b is the block size)
            break;
        case 't':
            args.t_addr = optarg; //address to tracefile
            break;
        case 'v':
            args.verbose = 1; //verbose flag
            break;
        case 'h':
            help(argv); //help
            exit(0);
        default:
            help(argv); //no args (help)
            exit(1);
        }
    }

    /* Ensures all required args are set */
    if (args.s == 0 || args.E == 0 || args.b == 0 || args.t_addr == 0) {
        printf("%s: Missing required command line argument\n", argv[0]);
        help(argv);
        exit(1);
    }

    num_sets = (1 << idx_bits);
    block_size = (1 << block_bits);
    
    printSummary(0, 0, 0);
    return 0;
}
