#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

#define MAX_LINE_SIZE 200

struct Cell
{
    int value;
    int prevCellRow; // TODO coordinates or pointer?
    int prevCellColumn;
    int isInitialized;
};

struct Sequence
{
    char* name;
    char* sequence;
};

void cleanup(struct Cell** table, int rows)
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
void printTable(struct Cell** table, int rows, int columns)
{
    // TODO Delete this function
    int i, j;
    for(i = 0; i < rows; i++)
    {
        for(j = 0; j < columns; j++)
        {
            printf("(%2d %2d - %2d) ", i, j, table[i][j].value);
        }
        printf("\n");
    }
}

void initializeTable(int gapScore, struct Cell** table, int rows, int columns)
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
        int str1Index, int str2Index, int matchScore, int mismatchScore, int gapScore)
{
    int matchValue;
    int firstStrGapValue;
    int secondStrGapValue;

    // Those vars are necessary because the table has another row and column
    int tableRow = str1Index + 1;
    int tableColumn = str2Index + 1;

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
            table[tableRow][tableColumn].prevCellRow = tableRow - 1;
            table[tableRow][tableColumn].prevCellColumn = tableColumn - 1;
        }
        else
        {
            // secondStrGapValue > matchValue > firstStrGapValue
            table[tableRow][tableColumn].value = secondStrGapValue;
            table[tableRow][tableColumn].prevCellRow = tableRow - 1;
            table[tableRow][tableColumn].prevCellColumn = tableColumn;
        }
    }
    else
    {
        if (firstStrGapValue > secondStrGapValue)
        {
            // firstStrGapValue > matchValue, secondStrGapValue
            table[tableRow][tableColumn].value = firstStrGapValue;
            table[tableRow][tableColumn].prevCellRow = tableRow - 1;
            table[tableRow][tableColumn].prevCellColumn = tableColumn;
        }
        else
        {
            // secondStrGapValue > firstStrGapValue > secondStrGapValue
            table[tableRow][tableColumn].value = secondStrGapValue;
            table[tableRow][tableColumn].prevCellRow = tableRow - 1;
            table[tableRow][tableColumn].prevCellColumn = tableColumn;
        }
    }
    table[tableRow][tableColumn].isInitialized = 1;
}

int readSequences(FILE* file, struct Sequence** sequencesPtr)
{
    struct Sequence* sequences = NULL;
    char line[MAX_LINE_SIZE];
    int sequencesNumber;
    char *newlineIndex;
    char* sequenceString = NULL;
    struct Sequence *newSequence;

    // TODO Check malloc/realloc results

    sequencesNumber = 0;
    while(fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        if ((newlineIndex = strchr(line, '\r')) != NULL)
        {
            *newlineIndex = '\0';
        }
        if ((newlineIndex = strchr(line, '\n')) != NULL)
        {
            *newlineIndex = '\0';
        }
        if (line[0] == '>')
        {
            if(!sequences)
            {
                sequences = malloc(sizeof(struct Sequence)); // TODO const
            }
            else
            {
                sequences = realloc(sequences, sizeof(sequences) + sizeof(struct Sequence));
            }
            newSequence = malloc(sizeof(struct Sequence));
            newSequence->name = malloc(strlen(line));
            strcpy(newSequence->name, line + 1);

            newSequence->sequence = sequenceString;
            sequences[sequencesNumber] = *newSequence;
            sequenceString = NULL;
            sequencesNumber++;
        }
        else
        {
            if (!sequenceString)
            {
                sequenceString = line;
            }
            else
            {
                sequenceString = realloc(sequenceString, strlen(sequenceString) + strlen(line) + 1);
                strcat(sequenceString, line);
            }
        }
    }
    *sequencesPtr = sequences;
    return sequencesNumber;
}

int main(int argc, char *argv[]) {

    FILE* file;
    char line[MAX_LINE_SIZE];
    struct Sequence* sequences;
    int sequencesNumber;

    char* str1;
    char* str2;

    int matchScore = 1;
    int mismatchScore = -1;
    int gapScore = -1;

    int str1Len;
    int str2Len;

    int rows;
    int columns;

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
            for(i = 0; i < rows; i++)
            {
                scoreTable[i] = malloc(sizeof(struct Cell) * columns);
                if(!scoreTable[i])
                {
                    cleanup(scoreTable, rows);
                    return 1;
                }
                for(j = 0; j < columns; j++)
                {
                    scoreTable[i][j].isInitialized = 0;
                }
            }

            initializeTable(gapScore, scoreTable, rows, columns);

            calculateValue(str1, str2, scoreTable, str1Len - 1, str2Len - 1, matchScore, mismatchScore, gapScore);

            printf("Score for alignment of sequence %s to sequence %s is %d\n", sequences[i].name, sequences[j].name, scoreTable[str1Len][str2Len].value);

            cleanup(scoreTable, str2Len + 1);
        }
    }

}
