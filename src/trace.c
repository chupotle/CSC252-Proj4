#include "trace.h"
/*
   Thomas Jeffries
   Timothy Chu
 */
char ** traceGet(char * filename, int lineCount) {
        char ** instructions = malloc(sizeof(char*) * lineCount * 2);
        FILE *trace;
        trace = fopen(filename, "r");
        char * LvS = malloc(sizeof(char*) * 100);
        char * address = malloc(sizeof(char*) * 100);
        int i;
        for(i = 0; i < lineCount; i++) {
                instructions[i] = malloc(sizeof(char*));
                fscanf(trace, "%s %s", LvS, address);
                instructions[i] = strdup(strcat(LvS, address));
        }
        fclose(trace);
        return instructions;
}

int traceLines(char* filename) {
        FILE *trace;
        trace = fopen(filename, "r");
        char c;
        int numLines = 0;
        while((c = fgetc(trace)) != EOF) {
                if(c == '\n') {
                        numLines++;
                }
        }
        return numLines;
}
