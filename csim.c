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
    unsigned long long int lru;
} line_t;

/* Define type for cache set */
typedef line_t *set_t;

/* Define type for entire cache */
typedef set_t *cache_t;

/* Define type for cache statistics */
typedef struct {
    unsigned int hits = 0;
    unsigned int misses = 0;
    unsigned int evictions = 0;
    unsigned long long int lru_count = 1;
} stats_t;

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
stats_t stats;
int num_sets;
int block_size;


/*
 * init: initializes cache by allocating memory and setting 0 for values
 */
void init() {
    cache = (set_t*)malloc(num_sets * sizeof(set_t));
    for (set = 0; set < num_sets; set++) {
        cache[set] = (line_t*)malloc(args.assoc * sizeof(line_t));
        for (line = 0; line < args.assoc; line++) {
            cache[set][line].valid = 0;
            cache[set][line].tag = 0;
            cache[set][line].lru = 0;
        }
    }
}

/*
 * runTraceSim: runs the cache simulator with the trace file provided in args
 */
 //help from: http://blough.ece.gatech.edu/3055/read_trace.c
void runTraceSim(char* t_addr) {
    FILE trace = fopen(t_addr, "r");
    char operation;
    unsigned long long addr;
    int size;

    while (fscanf(trace, " %c %llx,%d", &operation, &addr, &size) != 0) {
        switch(operation) {
            case 'L':
                getData(addr);
                break;
            case 'S':
                getData(addr);
                break;
            case 'M':
                getData(addr);
                getData(addr);
                break;
            default: break;
        }
    }
    fclose(trace);
}

/*
 * getData: fetch data from cache at address addr.
 *      - If present, add to stats.hits, else add to stats.misses
 *      - Call evict() if required
 */
void getData(unsigned long long addr) {
    unsigned long long tag = addr >> (block_bits + idx_bits);
    set_t set = cache[(addr >> args.idx_bits) & (num_sets-1)];

    for (int line = 0; line < args.assoc; ++line) {
        if (set[line].valid) {
            if (set[line].tag == tag) {
                set[line].lru = stats.lru_count++;
                stats.hits++;
                return;
            }
        }
    }

    stats.misses++;
    evict(set, tag);
}

/*
 * evict: evicts line in set_t set that matches tag.
 *      - Completes eviction
 *      - Increases stats.evictions
 */
void evict(set_t set, unsigned long long tag) {
    unsigned int evict_at = 0
    for (int line = 0; line < args.assoc; ++line) {
        if (ULONG_MAX > set[line].lru) {
            evict_at = line;
        }
    }

    if (set[evict_at].valid) {
        stats.evictions++;
    }

    set[evict_at].valid = 1;
    set[evict_at].tag = tag;
    set[evict_at].lru = stats.lru_count++;

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

    init(); //initializes cache, allocates memory, and assigns NULL values

    runTraceSim(args.t_addr); //runs the cache simulator
    
    printSummary(stats.hits, stats.misses, stats.evictions);
    return 0;
}
