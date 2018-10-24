#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <libgen.h>

#define INVALID_VALUE -1

#define MAX_ROWS 20000
#define MAX_LINE_SIZE 200
#define NUMBER_OF_VALUES 3 // TODO Rename

#define ROW_START "ATOM"

#define X_VALUE_START 31
#define Y_VALUE_START 39
#define Z_VALUE_START 47
#define VALUES_END 54
#define VALUE_LENGTH 8

float parseValue(char* line, int valueStart)
{
	char tempString[VALUE_LENGTH];
	strncpy(tempString, line + valueStart, VALUE_LENGTH);
	return strtof(tempString, NULL);
}

int readValues(FILE* file, float dataArray[][NUMBER_OF_VALUES], int maxSize)
{
	char line[MAX_LINE_SIZE];
	float xValue, yValue, zValue;
	int curLine = 0;

    while(fgets(line, MAX_LINE_SIZE, file) != NULL && curLine < maxSize)
	{

		if(strlen(line) < VALUES_END)
		{
			// Not enough data in row
			continue;
		}
		if(strncmp(line, ROW_START, sizeof(ROW_START) - 1) != 0)
		{
			// Row doesn't start with the right word
			continue;
		}
		xValue = parseValue(line, X_VALUE_START);
		if (xValue == 0 && errno != 0)
		{
			return INVALID_VALUE;
		}
		yValue = parseValue(line, Y_VALUE_START);
		if (yValue == 0 && errno != 0)
		{
			return INVALID_VALUE;
		}
		zValue = parseValue(line, Z_VALUE_START);
		if (zValue == 0 && errno != 0)
		{
			return INVALID_VALUE;
		}
		dataArray[curLine][0] = xValue; // TODO Constant for x, y, z cells
		dataArray[curLine][1] = yValue;
		dataArray[curLine][2] = zValue;
		curLine++;
	}

	return curLine;
	
}

float calculateDistance(float point1[NUMBER_OF_VALUES], float point2[NUMBER_OF_VALUES])
{
    float distance;
    float xDistance, yDistance, zDistance;
    xDistance = powf(point1[0] - point2[0], 2);
    yDistance = powf(point1[1] - point2[1], 2);
    zDistance = powf(point1[2] - point2[2], 2);
    distance = sqrtf(xDistance + yDistance + zDistance);
    return distance;
}

void calculateCenterOfGravity(float dataArray[][NUMBER_OF_VALUES], int rowNumber, float centerOfGravity[3])
{
	float xSum, ySum, zSum;
	int i;

	if (rowNumber == 0)
	{
		centerOfGravity[0] = 0;
		centerOfGravity[1] = 0;
		centerOfGravity[2] = 0;
		return;
	}

	xSum = 0;
	ySum = 0;
	zSum = 0;



	for(i = 0; i < rowNumber; i++)
	{
		xSum += dataArray[i][0];
		ySum += dataArray[i][1];
		zSum += dataArray[i][2];
	}

	centerOfGravity[0] = xSum / rowNumber;
	centerOfGravity[1] = ySum / rowNumber;
	centerOfGravity[2] = zSum / rowNumber;
}

float calculateTurnRadius(float dataArray[][NUMBER_OF_VALUES], int numOfRows, float centerOfGravity[NUMBER_OF_VALUES])
{
	float sum = 0;
	float distance;
	int i;

	if (numOfRows == 0)
	{
		return 0;
	}
	for(i = 0; i < numOfRows; i++)
	{
		distance = calculateDistance(dataArray[i], centerOfGravity);
		sum += pow(distance, 2);
	}
	return sqrtf(sum / numOfRows);
}

float calculateMaxDistance(float dataArray[][NUMBER_OF_VALUES], int numOfRows)
{
	int i;
	int j;
	float distance;
	float maxDistance = 0;

	for(i = 0; i < numOfRows; i++)
	{
		for(j = i + 1; j < numOfRows; j++)
		{
			distance = calculateDistance(dataArray[i], dataArray[j]);
			if (distance > maxDistance)
			{
				maxDistance = distance;
			}
		}
	}
	return maxDistance;
}
int main(int argc, char *argv[])
{
	// TODO Read data file from args
	// TODO Allow more than one data file
	int i;
	float dataArray[MAX_ROWS][NUMBER_OF_VALUES];
	char* dataFilePath;
	FILE* file;
	int ret;
	int rowNumber;
	float centerOfGravity[3];
	float turnRadius;
	float maxDistance;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <pdb1> <pdb2> ...\n", basename(argv[0]));
		exit(1);
	}

	for(i = 1; i < argc; i++)
	{
		dataFilePath = argv[i];
		file = fopen(dataFilePath, "r");
		if(!file)
		{
			fprintf(stderr, "Error opening file: %s\n", dataFilePath);
			return 1;
		}

		ret = readValues(file, dataArray, MAX_ROWS);

		if (ret < 0)
		{
			// TODO Error handling and usage
			return -ret;
		}

		rowNumber = ret;

		printf("PDB FILE %s, %d atoms were read\n", dataFilePath, rowNumber);

		calculateCenterOfGravity(dataArray, rowNumber, centerOfGravity);
		printf("Cg = %.3f %.3f %.3f\n", centerOfGravity[0], centerOfGravity[1], centerOfGravity[2]);

		turnRadius = calculateTurnRadius(dataArray, rowNumber, centerOfGravity);
		printf("Rg = %.3f\n", turnRadius);

		maxDistance = calculateMaxDistance(dataArray, rowNumber);
		printf("Dmax = %.3f\n", maxDistance);
	}
	return 0;
	
}
