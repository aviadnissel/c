#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

#define MAX_LINE_SIZE 200

struct Cell
{
    long value;
    struct Cell* prevCell;
    int isInitialized;
};

struct Sequence
{
    char* name;
    char* sequence;
};

void cleanup(struct Cell** table, size_t rows)
{
    int i;
    if(!table)
    {
        // table isn't allocated
        return;
    }
    for(i = 0; i < rows; i++)
    {
        if(table[i])
        {
            free(table[i]);
        }
    }
    free(table);
}

void initializeTable(long gapScore, struct Cell** table, size_t rows, size_t columns)
{
    int i;
    for(i = 0; i < rows; i++)
    {
        table[i][0].value = gapScore * i;
        table[i][0].isInitialized = 1;
    }
    for(i = 1; i < columns; i++)
    {
        table[0][i].value = gapScore * i;
        table[0][i].isInitialized = 1;
    }
}

void calculateValue(char* str1, char* str2, struct Cell** table,
        size_t str1Index, size_t str2Index, long matchScore, long mismatchScore, long gapScore)
{
    long matchValue;
    long firstStrGapValue;
    long secondStrGapValue;

    // Those vars are necessary because the table has another row and column
    size_t tableRow = str1Index + 1;
    size_t tableColumn = str2Index + 1;

    if( str1Index < 0 || str2Index < 0 || table[tableRow][tableColumn].isInitialized)
    {
        // str1Index or str2Index can't be negative, because we initialized the edges at the start
        // but, just in case, we check it here
        return;
    }

    calculateValue(str1, str2, table, str1Index - 1, str2Index - 1, matchScore, mismatchScore, gapScore);
    calculateValue(str1, str2, table, str1Index, str2Index - 1, matchScore, mismatchScore, gapScore);
    calculateValue(str1, str2, table, str1Index - 1, str2Index, matchScore, mismatchScore, gapScore);

    matchValue = table[tableRow - 1][tableColumn - 1].value;
    if(str1[str1Index] == str2[str2Index])
    {
        matchValue += matchScore;
    }
    else
    {
        matchValue += mismatchScore;
    }

    firstStrGapValue = table[tableRow][tableColumn - 1].value + gapScore;
    secondStrGapValue = table[tableRow - 1][tableColumn].value + gapScore;

    if(matchValue > firstStrGapValue)
    {
        if(matchValue > secondStrGapValue){
            // matchValue > firstStrGapValue, secondStrGapValue
            table[tableRow][tableColumn].value = matchValue;
            table[tableRow][tableColumn].prevCell = &table[tableRow - 1][tableColumn - 1];
        }
        else
        {
            // secondStrGapValue > matchValue > firstStrGapValue
            table[tableRow][tableColumn].value = secondStrGapValue;
            table[tableRow][tableColumn].prevCell = &table[tableRow - 1][tableColumn]; // TODO check those cells
        }
    }
    else
    {
        if (firstStrGapValue > secondStrGapValue)
        {
            // firstStrGapValue > matchValue, secondStrGapValue
            table[tableRow][tableColumn].value = firstStrGapValue;
            table[tableRow][tableColumn].prevCell = &table[tableRow ][tableColumn - 1];
        }
        else
        {
            // secondStrGapValue > firstStrGapValue > secondStrGapValue
            table[tableRow][tableColumn].value = secondStrGapValue;
            table[tableRow][tableColumn].prevCell = &table[tableRow - 1][tableColumn];
        }
    }
    table[tableRow][tableColumn].isInitialized = 1;
}

int readSequences(FILE* file, struct Sequence** sequencesPtr)
{
    struct Sequence* sequences = NULL;
    struct Sequence* newSequences = NULL;
    char line[MAX_LINE_SIZE];
    int sequencesNumber;
    char *newlineIndex;
    char* sequenceString = NULL;

    // TODO Check malloc/realloc results

    sequencesNumber = 0;
    while(fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        if ((newlineIndex = strchr(line, '\r')) != NULL || (newlineIndex = strchr(line, '\n')) != NULL)
        {
            *newlineIndex = '\0';
        }
        if (line[0] == '>') // TODO const
        {
            if (sequencesNumber > 0)
            {
                sequences[sequencesNumber - 1].sequence = malloc(strlen(sequenceString) + 1);
                strcpy(sequences[sequencesNumber - 1].sequence, sequenceString);
                free(sequenceString);
                newSequences = realloc(sequences, sizeof(struct Sequence) * (sequencesNumber + 1));
                if (!newSequences)
                {
                    // TODO What now?
                    return -1;
                }
                sequences = newSequences;
            }
            else
            {
                sequences = malloc(sizeof(struct Sequence)); // TODO const
            }
            sequences[sequencesNumber].name = malloc(strlen(line) + 1);
            strcpy(sequences[sequencesNumber].name, line + 1);
            sequencesNumber++;

            sequenceString = NULL;
        }
        else
        {
            if (!sequenceString)
            {
                sequenceString = malloc(strlen(line) + 1);
                strcpy(sequenceString, line);

            }
            else
            {
                sequenceString = realloc(sequenceString, strlen(sequenceString) + strlen(line) + 1);
                strcat(sequenceString, line);
            }
        }
    }
    if(!sequences)
    {
        return 0;
    }
    sequences[sequencesNumber - 1].sequence = malloc(strlen(sequenceString) + 1);
    strcpy(sequences[sequencesNumber - 1].sequence, sequenceString);
    free(sequenceString);
    *sequencesPtr = sequences;
    return sequencesNumber;
}


long parseValue(char* arg)
{
    char* endPtr;
    long value;
    value = strtol(arg, &endPtr, 10);
    if (endPtr == arg)
    {
        errno = EINVAL;
        fprintf(stderr, "Invalid number %s!\n", arg); // TODO What error message?
    }
    return value;
}

int main(int argc, char *argv[]) {

    FILE* file;
    char line[MAX_LINE_SIZE];
    struct Sequence* sequences;
    int sequencesNumber;

    char* str1;
    char* str2;

    long matchScore;
    long mismatchScore;
    long gapScore;

    size_t str1Len;
    size_t str2Len;

    size_t rows;
    size_t columns;

    int row;
    int column;

    struct Cell** scoreTable;

    int i, j;

    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <path_to_sequences_file> <m> <s> <g>\n", basename(argv[0]));
        exit(1);
    }

    file = fopen(argv[1], "r");

    if(!file)
    {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return errno;
    }

    matchScore = parseValue(argv[2]);
    if (errno)
    {
        return errno;
    }
    mismatchScore = parseValue(argv[3]);
    if (errno)
    {
        return errno;
    }
    gapScore = parseValue(argv[4]);
    if (errno)
    {
        return errno;
    }

    // TODO Get m, s, g from argv
    sequencesNumber = readSequences(file, &sequences);

    for(i = 0; i < sequencesNumber; i++)
    {
        for(j = i + 1; j < sequencesNumber; j++)
        {
            str1 = sequences[i].sequence;
            str2 = sequences[j].sequence;
            // TODO put a loop over all combinations
            str1Len = strlen(str1);
            rows = str1Len + 1;
            str2Len = strlen(str2);
            columns = str2Len + 2;

            // TODO move to function
            scoreTable = malloc(sizeof(struct Cell*) * rows); // TODO make sizeof a var?
            if(!scoreTable)
            {
                cleanup(scoreTable, rows);
                return 1;
            }
            for(row = 0; row < rows; row++)
            {
                scoreTable[row] = malloc(sizeof(struct Cell) * columns);
                if(!scoreTable[row])
                {
                    cleanup(scoreTable, rows);
                    return 1;
                }
                for(column = 0; column < columns; column++)
                {
                    scoreTable[row][column].isInitialized = 0;
                }
            }

            initializeTable(gapScore, scoreTable, rows, columns);

            calculateValue(str1, str2, scoreTable, str1Len - 1, str2Len - 1, matchScore, mismatchScore, gapScore);

            printf("Score for alignment of %s to %s is %ld\n",
                    sequences[i].name, sequences[j].name, scoreTable[str1Len][str2Len].value);

            cleanup(scoreTable, str2Len + 1);
        }
    }

}
