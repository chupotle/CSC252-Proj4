#include "cache.h"
#include "trace.h"

int write_xactions = 0;
int read_xactions = 0;

/*
   Thomas Jeffries
   Timothy Chu
 */

/*
   Print help message to user
 */

uint32_t logg2(uint32_t i){
        uint32_t r = 0;
        while (i >>= 1)
        {
                r++;
        }
        return r;
}

void printHelp(const char *prog){
        printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n", prog);
        printf("options:\n");
        printf("-h: print out help text\n");
        printf("-s <cache size>: set the total size of the cache in KB\n");
        printf("-w <ways>: set the number of ways in each set\n");
        printf("-l <line size>: set the size of each cache line in bytes\n");
        printf("-t <trace>: use <trace> as the input file for memory traces\n");
}

uint32_t getIndex(int num, int tagNum, int offsetNum){
        uint32_t ret = (num << tagNum);
        return (uint32_t) (ret >> (tagNum + offsetNum));
}

uint32_t getTag(int num, int tagNum){
        return (uint32_t) (num >> (32-tagNum));
}

/*
   Main function. Feed to options to set the cache

   Options:
   -h : print out help message
   -s : set L1 cache Size (KB)
   -w : set L1 cache ways
   -l : set L1 cache line size
 */
int main(int argc, char *argv[]){
        int i,j,k;
        uint32_t size = 32; //total size of L1$ (KB)
        uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
        uint32_t line = 32; //line size (B)

        // hit and miss counts
        int totalHits = 0;
        int totalMisses = 0;

        char *filename;

        //strings to compare
        const char helpString[] = "-h";
        const char sizeString[] = "-s";
        const char waysString[] = "-w";
        const char lineString[] = "-l";
        const char traceString[] = "-t";

        if (argc == 1) {
                // No arguments passed, show help
                printHelp(argv[0]);
                return 1;
        }
        //parse command line
        for (i = 1; i < argc; i++) {
                //check for help
                if (!strcmp(helpString, argv[i])) {
                        //print out help text and terminate
                        printHelp(argv[0]);
                        return 1; //return 1 for help termination
                }
                //check for size
                else if (!strcmp(sizeString, argv[i])) {
                        //take next string and convert to int
                        i++; //increment i so that it skips data string in the next loop iteration
                        //check next string's first char. If not digit, fail
                        if (isdigit(argv[i][0])) {
                                size = atoi(argv[i]);
                        } else {
                                printf("Incorrect formatting of size value\n");
                                return -1; //input failure
                        }
                }
                //check for ways
                else if (!strcmp(waysString, argv[i])) {
                        //take next string and convert to int
                        i++; //increment i so that it skips data string in the next loop iteration
                        //check next string's first char. If not digit, fail
                        if (isdigit(argv[i][0])) {
                                ways = atoi(argv[i]);
                        } else {
                                printf("Incorrect formatting of ways value\n");
                                return -1; //input failure
                        }
                }
                //check for line size
                else if (!strcmp(lineString, argv[i])) {
                        //take next string and convert to int
                        i++; //increment i so that it skips data string in the next loop iteration
                        //check next string's first char. If not digit, fail
                        if (isdigit(argv[i][0])) {
                                line = atoi(argv[i]);
                        } else {
                                printf("Incorrect formatting of line size value\n");
                                return -1; //input failure
                        }
                } else if (!strcmp(traceString, argv[i])) {
                        i++;
                        filename = argv[i];
                }
                //unrecognized input
                else {
                        printf("Unrecognized argument. Exiting.\n");
                        return -1;
                }
        }

        int bitsOffset,sets,numSet,bitIndex,bitsTag;
        bitsOffset = logg2(line);
        sets = line * ways;
        numSet = (size * 1024)/sets;
        bitIndex = logg2(numSet);
        bitsTag = 32 - (bitsOffset + bitIndex);

        char outputFileName[100];
        FILE *output;
        strcpy(outputFileName, filename);
        strcat(outputFileName, ".simulated");
        output = fopen(outputFileName, "w");

        /* TODO: Probably should intitalize the cache */

        int cacheWidth = (ways*3) + 1;
        int cache[numSet][cacheWidth];
        for(i = 0; i < numSet; i++)
                for(j = 0; j < cacheWidth; j++) {
                        cache[i][j] = 0;
                        if(j == (ways*3))
                                cache[i][j] = 2;
                }

        printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, numSet, line);
        printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", bitsTag, bitIndex, bitsOffset);

        int cacheSize,bitsTagOffsetted,missCounter,instructionCount;
        cacheSize = numSet * ways;
        int fullyAssociativeCache[cacheSize];
        for(i = 0; i < cacheSize; i++)
                fullyAssociativeCache[i] = -1;

        bitsTagOffsetted = 32 - bitsOffset;
        missCounter = 0;

        /* TODO: Now we read the trace file line by line */

        instructionCount = traceLines(filename);
        char ** traceInstr;
        traceInstr = traceGet(filename, instructionCount);



        table* hashtables = malloc(sizeof(table) * numSet);
        for(i = 0; i < numSet; i++)
                hashtables[i] = *ht_create((double) instructionCount/(double) numSet);

        /* TODO: Now we simulate the cache */

        char * currentInstr = malloc(sizeof(char*));
        int tag;
        int index;
        int data;
        char accessType;
        int address;
        int prevWay = 0;
        char* cacheAccess = "capacity";
        char outputLine[100];
        int isTrue = 0;
        int isFullyAssoc = 0;

        for(i = 0; i < instructionCount; i++) {
                cacheAccess = "capacity";
                currentInstr = traceInstr[i];
                accessType = currentInstr[0];
                currentInstr[0] = '0';
                memmove (currentInstr, currentInstr+1, strlen(currentInstr+1) + 1);
                address = (int) strtol(currentInstr, NULL, 0);

                tag = getTag(address, bitsTag);
                data = getTag(address, bitsTagOffsetted);
                index = getIndex(address, bitsTag, bitsOffset);

                isTrue = 0;

                if(ht_contains(&hashtables[index], tag) == 0) {
                        cacheAccess = "compulsory";
                        isTrue = 0;
                }
                else {
                        isTrue = 0;
                        for(j = 2; j < cacheWidth; j+=3)
                                if((cache[index][j] == tag) && (cache[index][j-2] == 1)) {
                                        cacheAccess = "hit";
                                        isTrue = 1;
                                        totalHits++;

                                        if(accessType == 's')
                                                cache[index][j-1] = 1;
                                        break;
                                }
                        if(isTrue == 0)
                                for(j = 0; j < cacheSize; j++)
                                        if(fullyAssociativeCache[j] == data) {
                                                cacheAccess = "conflict";
                                                break;
                                        }
                }
                if(isTrue == 0) {
                        read_xactions++;
                        prevWay = cache[index][ways*3];
                        if(cache[index][prevWay-1] == 1)
                                write_xactions++;

                        cache[index][prevWay] = tag;
                        cache[index][prevWay-2] = 1;

                        if(accessType == 's')
                                cache[index][prevWay-1] = 1;
                        else
                                cache[index][prevWay-1] = 0;

                        prevWay = (prevWay+3) % (ways*3);
                        cache[index][ways*3] = prevWay;
                        totalMisses++;

                        hashtables[index] = *ht_set(&hashtables[index], tag);
                }

                isFullyAssoc = 0;

                for(j = 0; j < cacheSize; j++)
                        if(data == fullyAssociativeCache[j]) {
                                isFullyAssoc = 1;
                                break;
                        }

                if(isFullyAssoc == 0) {
                        fullyAssociativeCache[missCounter] = data;
                        missCounter = (missCounter + 1) % cacheSize;
                }


                sprintf(outputLine, "%c 0x%08x %s\n", accessType, address, cacheAccess);
                fputs(outputLine, output);
        }

        /* Print results */
        printf("Miss Rate: %8lf%%\n",
               ((double) totalMisses) / ((double) totalMisses +
                                         (double) totalHits) * 100.0);
        printf("Read Transactions: %d\n", read_xactions);
        printf("Write Transactions: %d\n", write_xactions);

        /* TODO: Now we output the file */
        fclose(output);
        /* TODO: Cleanup */
}
