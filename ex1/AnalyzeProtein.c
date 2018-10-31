/**
 * @file AnalyzeProtein.c
 * @author Aviad Nissel <aviad.nissel@mail.huji.ac.il>
 * 
 * This program reads protein atoms from a pdb file,
 * calculates important values and prints them back. 
 */


/* --- Includes --- */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <libgen.h>


/* -- Constants --- */

/** Maximum number of rows in the pdb file. */
#define MAX_ROWS 20000

/** Maximum number of characters in a line. */
#define MAX_LINE_SIZE 200

/** How many coordinates are there (x, y, z). */
#define COORD_NUM 3

/** X coordinate position. */
#define X_POS 0

/** Y coordinate position. */
#define Y_POS 1

/** Z coordinate position. */
#define Z_POS 2

/** How every row that matters for calculations should start. */
#define ROW_START "ATOM  "

/** Where the x value starts in the line. */
#define X_VALUE_START 30

/** Where the y value starts in the line. */
#define Y_VALUE_START 38

/** Where the z value starts in the line. */
#define Z_VALUE_START 46

/** Where the values end. */
#define VALUES_END 53

/** Minimum line length. */
#define MINIMUM_LINE_LENGTH 61

/** The number of characters in each value. */
#define VALUE_LENGTH 8

/** An error number if the line is too short. */
#define ERRSHORT -1;


/* --- Functions --- */

/**
 * Parses a value of VALUE_LENGTH from given line.
 * 
 * @param line the line to parse from.
 * @param valueStart the position of the value.
 * @return The parsed value. In case of error, returns 0 and sets errno.
 */
float parseValue(char* line, int valueStart)
{
	char* endPtr;
	char tempString[VALUE_LENGTH + 1];
	float value;
	memset(tempString, '\0', VALUE_LENGTH);
	strncpy(tempString, line + valueStart, VALUE_LENGTH);
	value = strtof(tempString, &endPtr);
	if (endPtr == tempString)
	{
		errno = EINVAL; 
		fprintf(stderr, "Error in coordinate conversion %s!\n", tempString);
	}
	return value;
}

/**
 * Reads the values from the given file into the given array.
 * 
 * @param file the file to read from.
 * @param dataArray the output array.
 * @param maxLines the maximum number of lines to read.
 * @return positive integer - how many lines were read.
 *         negative integer - an error code.
 */
int readValues(FILE* file, float dataArray[][COORD_NUM], int maxLines)
{
	char line[MAX_LINE_SIZE];
	float xValue, yValue, zValue;
	int curLine = 0;

	while(fgets(line, MAX_LINE_SIZE, file) != NULL && curLine < maxLines)
	{

		if(strncmp(line, ROW_START, sizeof(ROW_START) - 1) != 0)
		{
			// Row doesn't start with the right word
			continue;
		}
		if(strlen(line) < MINIMUM_LINE_LENGTH)
		{
			fprintf(stderr, "ATOM line is too short %ld characters\n", strlen(line));
			return ERRSHORT;
		}
		xValue = parseValue(line, X_VALUE_START);
		if (xValue == 0 && errno != 0)
		{
			return -errno;
		}
		yValue = parseValue(line, Y_VALUE_START);
		if (yValue == 0 && errno != 0)
		{
			return -errno;
		}
		zValue = parseValue(line, Z_VALUE_START);
		if (zValue == 0 && errno != 0)
		{
			return -errno;
		}
		dataArray[curLine][X_POS] = xValue;
		dataArray[curLine][Y_POS] = yValue;
		dataArray[curLine][Z_POS] = zValue;
		curLine++;
	}

	return curLine;
	
}

/**
 * Calculates the distance between two give points.
 * 
 * @param point1 the first point.
 * @param point2 the second point.
 * @return The distance between the points.
 */
float calculateDistance(float point1[COORD_NUM], float point2[COORD_NUM])
{
	float distance;
	float xDistance, yDistance, zDistance;
	xDistance = powf(point1[X_POS] - point2[X_POS], 2);
	yDistance = powf(point1[Y_POS] - point2[Y_POS], 2);
	zDistance = powf(point1[Z_POS] - point2[Z_POS], 2);
	distance = sqrtf(xDistance + yDistance + zDistance);
	return distance;
}

/**
 * Calcualtes the center of gravity of the given points.
 *
 * @param dataArray an array of points, each represented as (x, y, z).
 * @param numOfPoints how many points are in the given array.
 * @param centerOfGravity the output array, representing (x, y, z).
 */
void calculateCenterOfGravity(float dataArray[][COORD_NUM], int numOfPoints, float centerOfGravity[3])
{
	float xSum, ySum, zSum;
	int i;

	if (numOfPoints == 0)
	{
		centerOfGravity[X_POS] = 0.0f;
		centerOfGravity[Y_POS] = 0.0f;
		centerOfGravity[Z_POS] = 0.0f;
		return;
	}

	xSum = 0.0f;
	ySum = 0.0f;
	zSum = 0.0f;

	for(i = 0; i < numOfPoints; i++)
	{
		xSum += dataArray[i][X_POS];
		ySum += dataArray[i][Y_POS];
		zSum += dataArray[i][Z_POS];
	}

	centerOfGravity[X_POS] = xSum / numOfPoints;
	centerOfGravity[Y_POS] = ySum / numOfPoints;
	centerOfGravity[Z_POS] = zSum / numOfPoints;
}

/**
 * Calcualtes the turn radius of the given points.
 *
 * @param dataArray an array of points, each represented as (x, y, z).
 * @param numOfPoints how many points are in the given array.
 * @param centerOfGravity the center of gravity of the points.
 * @return The turn radius.
 */
float calculateTurnRadius(float dataArray[][COORD_NUM], int numOfPoints, float centerOfGravity[COORD_NUM])
{
	float sum = 0;
	float distance;
	int i;

	if (numOfPoints == 0)
	{
		return 0;
	}
	for(i = 0; i < numOfPoints; i++)
	{
		distance = calculateDistance(dataArray[i], centerOfGravity);
		sum += pow(distance, 2);
	}
	return sqrtf(sum / numOfPoints);
}

/**
 * Calcualtes the maximum distance between all given points.
 *
 * @param dataArray an array of points, each represented as (x, y, z).
 * @param numOfPoints how many points are in the given array.
 * @return The maxium distance between the points.
 */
float calculateMaxDistance(float dataArray[][COORD_NUM], int numOfRows)
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


/* --- Main --- */

/**
 * The main function.
 *
 * @param argc the number of arguments.
 * @param argv An array of strings, the arguments of the program.
 * @return 0 in a succesful execution, 1 in case of file error, 
 *        other in case of a different error.
 */
int main(int argc, char *argv[])
{
	int i;
	float dataArray[MAX_ROWS][COORD_NUM];
	char* dataFilePath;
	FILE* file;
	int ret;
	int numOfRows;
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
			return errno;
		}

		ret = readValues(file, dataArray, MAX_ROWS);

		if (ret < 0)
		{
			return -ret;
		}
		if (ret == 0)
		{
			fprintf(stderr, "Error - 0 atoms were found in the file %s\n", dataFilePath);
			return 1;
		}

		numOfRows = ret;

		printf("PDB file %s, %d atoms were read\n", dataFilePath, numOfRows);

		calculateCenterOfGravity(dataArray, numOfRows, centerOfGravity);
		printf("Cg = %.3f %.3f %.3f\n", centerOfGravity[X_POS], centerOfGravity[Y_POS], centerOfGravity[Z_POS]);

		turnRadius = calculateTurnRadius(dataArray, numOfRows, centerOfGravity);
		printf("Rg = %.3f\n", turnRadius);

		maxDistance = calculateMaxDistance(dataArray, numOfRows);
		printf("Dmax = %.3f\n", maxDistance);
	}
	return 0;
	
}
