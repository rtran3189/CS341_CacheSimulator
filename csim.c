// Richard Tran
// rtran00

#include <unistd.h> 
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "cachelab.h" // For printSummary function
#include <getopt.h> //   Used to parse command line options
#include <stdlib.h> //   Allows usage of atoi function which converts string to int

//Struct that builds the cache
typedef struct
{
    int s; // Number of set index bits         2^s
    int S; // Number of sets                   S=2^s
    int b; // Number of block bits             2^b
    int B; // Number of blocks                 B=2^b
    int E; // Number of Lines per set
    

    int numberOfHits; //        Number of hits
    int numberOfMisses;//       Number of misses
    int numberOfEvictions;//    Number of evictions
    int isVerbose;//            Verbose flag
} cacheSettings;

typedef struct 
{
    int valid;
    unsigned long long tag; 
    char *block;
    int numberOfUses;
} setLine;

typedef struct 
{
    setLine *lines;
} cacheSet;

typedef struct 
{
    cacheSet *sets;
} cache;

// Creates the cache.
cache createCache(long long S, int E) 
{
    cache currentCache;	
    cacheSet set;
    setLine line;
    currentCache.sets = (cacheSet *) malloc(sizeof(cacheSet) * S);

    // Loops to create the set of the cache.
    int i = 0;
    for (i = 0; i < S; i++) 
    {
        set.lines =  (setLine *) malloc(sizeof(setLine) * E);
        currentCache.sets[i] = set;
        // Loops to create the lines of the cache.
        int j = 0;
        for (j = 0; j < E; j++) 
        {
            line.valid = 0; 
            line.tag = 0; 
            set.lines[j] = line;
            line.numberOfUses = 0;
        }
    } 
    return currentCache;
}
int isVerbose = 0;

// Prints how to use.
void printUsage()
{
    printf("Usage: ./csim [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("-s: number of set index(2^s sets)\n");
    printf("-E: number of lines per set\n");
    printf("-b: number of block offset bits\n");
    printf("-t: trace file name\n");
}

// Checks if there is a hit at location.
int checkForHit(setLine line, unsigned long long tag)
{
    if(line.valid)
    {
        if(line.tag == tag)
        {
            return 1;
        }
    }
    return 0;
}

// Checks if the set is full of valid information.
int checkFull(cacheSet set, cacheSettings property)
{
    int i = 0;
    for(i = 0; i<property.E; i++)
    {
        if(set.lines[i].valid == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Finds the next available line to put information in.
int findIndex(cacheSet set, cacheSettings property)
{
    int i = 0;
    for(i = 0; i<property.E; i++)
    {
        if(set.lines[i].valid == 0)
        {
            return i;
        }
    }
    // This will never return -1.
    return -1;
}

// Finds the next line to evict.
int findEvict(cacheSet set, cacheSettings property)
{
    int min = set.lines[0].numberOfUses;
    int i = 0;
    int index = 0;
    for(i = 0; i < property.E ; i++)
    {
        if(min>set.lines[i].numberOfUses)
        {
            index = i;
            min = set.lines[i].numberOfUses;
        }
    }
    return index;
}

// Finds the maximum used.
int findMax(cacheSet set, cacheSettings property)
{
    int max = set.lines[0].numberOfUses;
    int i = 0;
    int index = 0;
    for(i = 0; i < property.E ; i++)
    {
        if(set.lines[i].numberOfUses>max)
        {
            index = i;
            max = set.lines[i].numberOfUses;
        }
    }
    return index;
}

// The actual caching function.
cacheSettings runCache(cache currentCache,cacheSettings property, unsigned long long address)
{
    // Compute the size of the tag, the 64 represents 64-bit.
    int tagSize = 64 - (property.b + property.s);
    unsigned long long tag = address >> (property.s + property.b);

    // Uses tagSize to compute set index
    unsigned long long temp = address << (tagSize);
    unsigned long long setIndex = temp >> (tagSize + property.b);
    cacheSet set = currentCache.sets[setIndex];

    // Loops through all lines of the set.
    int i = 0;
    int hit = 0;
    for (i = 0; i<property.E; i++)
    {
        setLine currentLine = set.lines[i];
        
        // Checks for hit and increments hit number, finds the max, and updates number of uses counter.
        if(checkForHit(currentLine, tag) == 1)
        {
            property.numberOfHits+=1;
            int max = 0;
            hit = 1;
            max = findMax(set, property);
            currentCache.sets[setIndex].lines[i].numberOfUses = currentCache.sets[setIndex].lines[max].numberOfUses+1;
        }
    }

    // If cache isn't full AND it is a miss, then update miss counter and place information into it.
    if(hit == 0 && checkFull(set, property) == 1)
    {
        property.numberOfMisses+=1;
        int index = 0;
        index = findIndex(set, property);
        set.lines[index].tag = tag;
        set.lines[index].valid = 1;
        int max = 0;
        max = findMax(set, property);
        currentCache.sets[setIndex].lines[index].numberOfUses = currentCache.sets[setIndex].lines[max].numberOfUses+1;
    }

    // Eviction
    else if(hit == 0)
    {
        property.numberOfMisses+=1;
        property.numberOfEvictions+=1;
        int evictIndex = 0;

        // Finds the eviction line
        evictIndex = findEvict(set, property);
        set.lines[evictIndex].tag = tag;
        int max = 0;
        max = findMax(set, property);
        currentCache.sets[setIndex].lines[evictIndex].numberOfUses = currentCache.sets[setIndex].lines[max].numberOfUses+1;
    }
    return property;
}

// Main
int main(int argc, char** argv)
{
    cache currentCache;
    cacheSettings property;
    char* trace;
    char input;
    property.isVerbose = 0;
    
    while( (input=getopt(argc,argv,"s:E:b:t:vh")) != -1)
    {
        switch(input){
        case 's':
            property.s = atoi(optarg);
            break;
        case 'E':
            property.E = atoi(optarg);
            break;
        case 'b':
            property.b = atoi(optarg);
            break;
        case 't':
            trace = optarg;
            break;
        case 'v':
            property.isVerbose = 1;
            break;
        case 'h':
            printUsage();
            exit(0);
        default:
            printUsage();
            exit(-1);
        }
    }
    // Computes S and B
    property.S = pow(2.0,property.s);
    property.B = pow(2.0,property.b);

    // Initialization
    currentCache = createCache(property.S, property.E);
    property.numberOfMisses = 0;
    property.numberOfHits = 0;
    property.numberOfEvictions = 0;
    //input file
    FILE *tmp;
    //wrap the trace into tmp
    char command;
    unsigned long long address;
    int size;
    tmp = fopen(trace, "r");
    while(fscanf(tmp, " %c %llx,%d", &command, &address, &size) == 3)
    {
        switch(command)
        {
            case 'I':
                break;
            case 'L':
                property = runCache(currentCache, property, address);
                break;
            case 'S':
                property = runCache(currentCache, property, address);
                break;
            case 'M':
                property = runCache(currentCache, property, address);
                property = runCache(currentCache, property, address);	
                break;
            default:
                break;
        }
    }
    
    printSummary(property.numberOfHits, property.numberOfMisses, property.numberOfEvictions);
    fclose(tmp);
    return 0;
}