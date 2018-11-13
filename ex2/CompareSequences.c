#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

#define MAX_LINE_SIZE 200
#define SEQUENCE_NAME_LINE_START '>'

struct Cell
{
	long value;
	int isInitialized;
};

struct Sequence
{
	char* name;
	char* sequence;
};

void cleanupTable(struct Cell **table, size_t rows)
{
	size_t i;
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

void cleanupSequences(struct Sequence* sequences, int numOfSequences)
{
	int i;
	for(i = 0; i < numOfSequences; i++)
	{
		free(sequences[i].sequence);
		free(sequences[i].name);
	}
	free(sequences);
}

struct Cell** createEmptyScoreTable(size_t rows, size_t columns)
{
	struct Cell** scoreTable;
	size_t row;
	size_t column;
	scoreTable = malloc(sizeof(struct Cell*) * rows);
	if(!scoreTable)
	{
		cleanupTable(scoreTable, rows);
		return NULL;
	}
	for(row = 0; row < rows; row++)
	{
		scoreTable[row] = malloc(sizeof(struct Cell) * columns);
		if(!scoreTable[row])
		{
			cleanupTable(scoreTable, rows);
			return NULL;
		}
		for(column = 0; column < columns; column++)
		{
			scoreTable[row][column].isInitialized = 0;
		}
	}
	return scoreTable;
}

void initializeTable(long gapScore, struct Cell** table, size_t rows, size_t columns)
{
	size_t i;
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

	if(table[tableRow][tableColumn].isInitialized)
	{
		return;
	}

	// string indexes at this point are always larger than 0,
	// because we assume the table is initialized by initializeTable

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
		}
		else
		{
			// secondStrGapValue > matchValue > firstStrGapValue
			table[tableRow][tableColumn].value = secondStrGapValue;
		}
	}
	else
	{
		if (firstStrGapValue > secondStrGapValue)
		{
			// firstStrGapValue > matchValue, secondStrGapValue
			table[tableRow][tableColumn].value = firstStrGapValue;
		}
		else
		{
			// secondStrGapValue > firstStrGapValue > secondStrGapValue
			table[tableRow][tableColumn].value = secondStrGapValue;
		}
	}
	table[tableRow][tableColumn].isInitialized = 1;
}

int readSequences(FILE* file, struct Sequence** sequencesPtr)
{
	struct Sequence* sequences = NULL;
	struct Sequence* newSequences = NULL;
	char line[MAX_LINE_SIZE];
	int numOfSequences;
	char *newlineIndex;
	char* sequenceString = NULL;

	numOfSequences = 0;
	while(fgets(line, MAX_LINE_SIZE, file) != NULL)
	{
		if ((newlineIndex = strchr(line, '\r')) != NULL || (newlineIndex = strchr(line, '\n')) != NULL)
		{
			*newlineIndex = '\0';
		}
		if (line[0] == SEQUENCE_NAME_LINE_START)
		{
			if (numOfSequences > 0)
			{
				sequences[numOfSequences - 1].sequence = malloc(strlen(sequenceString) + 1);
				strcpy(sequences[numOfSequences - 1].sequence, sequenceString);
				free(sequenceString);
				newSequences = realloc(sequences, sizeof(struct Sequence) * (numOfSequences + 1));
				if (!newSequences)
				{
					return -ENOMEM;
				}
				sequences = newSequences;
			}
			else
			{
				sequences = malloc(sizeof(struct Sequence));
				if(!sequences)
				{
					return -ENOMEM;
				}
			}
			sequences[numOfSequences].name = malloc(strlen(line) + 1);
			strcpy(sequences[numOfSequences].name, line + 1);
			numOfSequences++;

			sequenceString = NULL;
		}
		else
		{
			if (!sequenceString)
			{
				sequenceString = malloc(strlen(line) + 1);
				if(!sequenceString)
				{
					return -ENOMEM;
				}
				strcpy(sequenceString, line);

			}
			else
			{
				sequenceString = realloc(sequenceString, strlen(sequenceString) + strlen(line) + 1);
				if(!sequenceString)
				{
					return -ENOMEM;
				}
				strcat(sequenceString, line);
			}
		}
	}
	if(!sequences)
	{
		*sequencesPtr = NULL;
		return 0;
	}
	sequences[numOfSequences - 1].sequence = malloc(strlen(sequenceString) + 1);
	strcpy(sequences[numOfSequences - 1].sequence, sequenceString);
	free(sequenceString);
	*sequencesPtr = sequences;
	return numOfSequences;
}


long parseValue(char* arg)
{
	char* endPtr;
	long value;
	value = strtol(arg, &endPtr, 10);
	if (endPtr == arg)
	{
		errno = EINVAL;
		fprintf(stderr, "ERROR: invalid number %s!\n", arg);
	}
	return value;
}

int main(int argc, char *argv[]) {

	FILE* file;
	struct Sequence* sequences;
	int numOfSequences;

	char* str1;
	char* str2;

	long matchScore;
	long mismatchScore;
	long gapScore;

	size_t str1Len;
	size_t str2Len;

	size_t rows;
	size_t columns;

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
		fprintf(stderr, "ERROR opening file: %s\n", argv[1]);
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

	numOfSequences = readSequences(file, &sequences);

	fclose(file);

	if(numOfSequences < 0)
	{
		fprintf(stderr, "ERROR while reading sequences: %d\n", -numOfSequences);
		return -numOfSequences;
	}

	for(i = 0; i < numOfSequences; i++)
	{
		for(j = i + 1; j < numOfSequences; j++)
		{
			str1 = sequences[i].sequence;
			str2 = sequences[j].sequence;

			str1Len = strlen(str1);
			str2Len = strlen(str2);
			rows = str1Len + 1;
			columns = str2Len + 1;

			scoreTable = createEmptyScoreTable(rows, columns);

			if(!scoreTable)
			{
				fprintf(stderr, "ERROR while creating table\n");
				return 1;
			}
			initializeTable(gapScore, scoreTable, rows, columns);

			calculateValue(str1, str2, scoreTable, str1Len - 1, str2Len - 1, matchScore, mismatchScore, gapScore);

			printf("Score for alignment of %s to %s is %ld\n",
					sequences[i].name, sequences[j].name, scoreTable[str1Len][str2Len].value);

			cleanupTable(scoreTable, rows);
		}
	}
	cleanupSequences(sequences, numOfSequences);
}
