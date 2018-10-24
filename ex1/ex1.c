#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <libgen.h>

#define OPEN_FILE_ERROR -1
#define INVALID_VALUE -2

#define MAX_ROWS 20000
#define NUMBER_OF_VALUES 3 // TODO Rename

#define ROW_START "ATOM"

#define X_VALUE_START 31
#define X_VALUE_END 38
#define Y_VALUE_START 39
#define Y_VALUE_END 46
#define Z_VALUE_START 47
#define Z_VALUE_END 54

float parseValue(char* line, int valueStart, int valueEnd)
{
	return strtof(line + valueStart, &line + valueEnd);
}

int readValues(FILE* file, float dataArray[][NUMBER_OF_VALUES], int maxSize)
{
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int i;
	float xValue, yValue, zValue;
	int curLine = 0;

	
	while((read = getline(&line, &len, file)) != -1)
	{
		if(len < Z_VALUE_END)
		{
			// Not enough data in row
			continue;
		}
		if(strncmp(line, ROW_START, sizeof(ROW_START) - 1))
		{
			// Row doesn't start with the right word
			continue;
		}
		xValue = parseValue(line, X_VALUE_START, X_VALUE_END);
		if (xValue == 0 && errno != 0)
		{
			return INVALID_VALUE;
		}
		yValue = parseValue(line, Y_VALUE_START, Y_VALUE_END);
		if (yValue == 0 && errno != 0)
		{
			return INVALID_VALUE;
		}
		zValue = parseValue(line, Z_VALUE_START, Z_VALUE_END);
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
		if(xSum > 1000) {
			printf("%d %.3f %.3f %.3f\n", i, xSum, ySum, zSum);
			return;
		}
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
	return sqrt(sum / numOfRows);
}
float calculateDistance(float point1[NUMBER_OF_VALUES], float point2[NUMBER_OF_VALUES])
{
	float distance;
	float xDistance, yDistance, zDistance;
	xDistance = pow(point1[0] - point2[0], 2);
	yDistance = pow(point1[1] - point2[1], 2);
	zDistance = pow(point1[2] - point2[2], 2);
	distance = sqrt(xDistance + yDistance + zDistance);
	return distance;
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
	float maximalDistance;

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
		printf("Rg = %f\n", turnRadius);
	}
	return 0;
	
}
