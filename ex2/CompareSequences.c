#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Cell
{
    int value;
    int prevCellRow; // TODO coordinates or pointer?
    int prevCellColumn;
    int isInitialized;
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

int main() {
    // TODO All of these should be inputs
    char* str1 = "GCATGCU";
    char* str2 = "GATTACA";
    char* seq1Name = "seq1";
    char* seq2Name = "seq2";
    int matchScore = 1;
    int mismatchScore = -1;
    int gapScore = -1;

    int str1Len;
    int str2Len;

    struct Cell** scoreTable;

    int i, j;

    // TODO put a loop over all combinations
    str1Len = strlen(str1);
    str2Len = strlen(str2);

    // TODO move to function
    scoreTable = malloc(sizeof(struct Cell*) * (str1Len + 1)); // TODO make sizeof a var?
    if(!scoreTable)
    {
        cleanup(scoreTable, str1Len + 1);
        return 1;
    }
    for(i = 0; i < str1Len + 1; i++)
    {
        scoreTable[i] = malloc(sizeof(struct Cell) * (str2Len + 1));
        if(!scoreTable[i])
        {
            cleanup(scoreTable, str1Len + 1);
            return 1;
        }
        for(j = 0; j < str2Len + 1; j++)
        {
            scoreTable[i][j].isInitialized = 0;
        }
    }

    initializeTable(gapScore, scoreTable, str1Len + 1, str2Len + 1);

    calculateValue(str1, str2, scoreTable, str1Len - 1, str2Len - 1, matchScore, mismatchScore, gapScore);

    printf("Score for alignment of sequence %s to sequence %s is %d\n", seq1Name, seq2Name, scoreTable[str1Len][str2Len].value);

    cleanup(scoreTable, str2Len + 1);
}
