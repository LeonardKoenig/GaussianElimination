#include "main.h"

// Hauptfunktion. Programmaufrufspunkt.
int main(int argc, char argv[])
{
	double ** ppMatrix = ReadMatrix();
	if(ppMatrix == NULL) {
		printf("Error while reading matrix. Terminate programm. / Fehler beim Einlesen der Matrix. Beende Programm.\n");
		system("pause");
		return 0;
	}

	printf("Eingelesen - ");
	PrintMatrix(ppMatrix);

	POSITION posPivot = StepOne(ppMatrix);
	if(posPivot.iColumn == -1) {
		printf("Fettig. Nix zu tun.\n");
		system("pause");
		return 0;
	}


	StepTwo(ppMatrix, posPivot);
	printf("\nSchritt 1 & 2 - ");
	PrintMatrix(ppMatrix);
	StepThree(ppMatrix, posPivot);
	printf("\nSchritt 3 - ");
	PrintMatrix(ppMatrix);
	StepFour(ppMatrix, posPivot);
	printf("\nSchritt 4 - ");
	PrintMatrix(ppMatrix);
	POSITION_ARRAY pivots = StepFive(ppMatrix, posPivot);
	printf("\nSchritt 5 - ");
	PrintMatrix(ppMatrix);
	AddPosition(&pivots, posPivot);
	StepSix(ppMatrix, pivots);
	printf("\nFertig - ");
	PrintMatrix(ppMatrix);
	system("pause");
	FreeMatrixMemory(ppMatrix, iColumns);
	return 0;
}

double ** ReadMatrix()
{
	printf("Wollen Sie die Matrix aus einer Datei laden oder \x81 \bber den Bildschirm eingeben? [Datei/Bildschirm]\n");
	printf("  ");
	char answer[100];
	scanf("%s", answer);
	fflush(stdin);

	double ** ppMatrix;
	if(strcmp(answer, "Datei") == 0) {
		ppMatrix = ReadMatrixFromFile();
	} else {
		ppMatrix = ReadMatrixFromInput();
	}

	printf("Wollen Sie die Elementarmatrizen in eine Datei schreiben oder auf dem Bildschirm ausgeben? [Datei/Bildschrim]\n");
	printf("  ");
	char answer2[100];
	scanf("%s", answer2);
	fflush(stdin);

	if(strcmp(answer2, "Datei") == 0) {
		printf("Bitte geben Sie den Dateinamen an: ");
		char answer3[200];
		fgets(answer3, 100, stdin);
		answer3[strlen(answer3) - 1] = '\0';

		fOut = fopen(answer3, "w");

		if(fOut == NULL) {
			printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht \x94 \bffnen: %s", answer);
		}
	} else {
		fOut = stdout;
	}

	return ppMatrix;
}

double ** ReadMatrixFromFile()
{
	printf("Bitte geben Sie den Dateinamen an: ");
	char answer[200];
	fgets(answer, 100, stdin);
	answer[strlen(answer) - 1] = '\0';
	FILE * fFile = fopen(answer, "r");

	// stream überprüfen
	if(fFile == NULL) {
		printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht \x94 \bffnen: %s", answer);
		return NULL;
	}

	fscanf(fFile, "%d;%d\n", &iColumns, &iRows);
	
	double ** ppMatrix = AllocateMatrixMemory(iColumns, iRows);

	char str[1000];

	for(int i = 0; i < iRows; i++) {
		fgets(str, 1000, fFile);

		char var[1000];

		int pos = 0;
		for(int g = 0; g < iColumns; g++) {
			for(int h = 0; pos < strlen(str); h++) {
				if(str[pos] == ';' || str[pos] == '\n') {
					pos++;
					break;
				}
				var[h] = str[pos];
				pos++;
			}
			ppMatrix[g][i] = atof(var);
		}
	}

	fclose(fFile);

	return ppMatrix;
}

double ** ReadMatrixFromInput()
{
	printf("Bitte geben Sie nun, per Komma und Leerzeichen getrennt, die Spalten- und Zeilenanzahl an: ");
	scanf("%d, %d", &iColumns, &iRows);
	
	double ** ppMatrix = AllocateMatrixMemory(iColumns, iRows);

	for(int i = 0; i < iColumns; i++) {
		for(int g = 0; g < iRows; g++) {
			printf("Bitte geben Sie nun das %d. Element der %d. Spalte an: ", g + 1, i + 1);
			scanf("%lf", &ppMatrix[i][g]);
		}
	}

	printf("\n");

	return ppMatrix;
}

void PrintMatrix(double ** ppMatrix)
{
	printf("Print Matrix\n");
	for(int i = 0; i < iRows; i++) {
		for(int g = 0; g < iColumns; g++) {
			printf("%lf", ppMatrix[g][i]);
			if(g == iColumns - 1) { // Letzte Spalte 
				printf("\n"); // Auf neue Zeile wechseln
			} else {
				printf(";"); // Trennzeichen ausgeben
			}
		}
	}
	printf("\n");
}

void AddPosition(POSITION_ARRAY * posArray, POSITION posPositionToAdd)
{
	POSITION * tmp = posArray->pPositions;
	posArray->pPositions = (POSITION *)malloc(sizeof(POSITION) * (posArray->iCount + 1));
	for(int i = 0; i < posArray->iCount; i++) {
		posArray->pPositions[i] = tmp[i];
	}
	posArray->pPositions[posArray->iCount] = posPositionToAdd;
	posArray->iCount ++;
}

POSITION StepOne(double ** ppMatrix)
{
	POSITION res = {-1,-1};
	for(int i = 0; i < iColumns; i++) {
		for(int g = 0; g < iRows; g++) {
			if(ppMatrix[i][g] != 0.0f) {
				res.iRow = g;
				res.iColumn = i;
				return res;
			}
		}
	}
	return res;
}

void StepTwo(double ** ppMatrix, POSITION posPivot)
{
	if(posPivot.iColumn == 0) {
		return;
	}
	double * pTemp = ppMatrix[posPivot.iColumn];

	ppMatrix[posPivot.iColumn] = ppMatrix[0];
	ppMatrix[0] = pTemp;

	fprintf(fOut, "Füge Elementarmatrix an: T[%d,%d]\n", posPivot.iColumn + iColumnsOffset, 1 + iRowsOffset);
}

void StepThree(double ** ppMatrix, POSITION posPivot)
{
	double fElement = ppMatrix[posPivot.iColumn][0];

	if(fElement == 0) {
		printf("Big error... Error in previous steps. Terminate programm. / Grosser Fehler... Fehler in vorherigen Schritten. Beende Programm.\n");
		system("Pause");
		exit(-1);
	}

	for(int i = 0; i < iColumns; i++) {
		ppMatrix[i][0] *= (1.0f / fElement);
	}
	fprintf(fOut, "Füge Elementarmatrix an: S[%d](%lf)\n", 1 + iRowsOffset, 1.0f / fElement);
}

void StepFour(double ** ppMatrix, POSITION posPivot)
{
	for(int i = 1; i < iRows; i++) {
		double scale = ppMatrix[posPivot.iColumn][i];

		if(scale == 0.0f) {
			continue; // mit nächster Zeile weitermachen
		}

		for(int g = 0; g < iColumns; g++) {
			ppMatrix[g][i] += - ppMatrix[g][0] * scale;
		}
		fprintf(fOut, "Füge Elementarmatrix an: A[%d,%d](%lf)\n", i + iRowsOffset, 1 + iRowsOffset, -scale);
	}
}

POSITION_ARRAY StepFive(double ** ppMatrix, POSITION posPivot)
{
	POSITION_ARRAY pivots = {NULL, 0};
	int iRowsSave = iRows;
	int iColumnsSave = iColumns;

	if(iRows - 1 == 0) {
		return pivots; // Fertig
	}

	if(iColumns - posPivot.iColumn == 0) {
		return pivots; // Fertig
	}
	
	double ** ppSmallerMatrix = AllocateMatrixMemory(iColumns, iRows);

	for(int i = posPivot.iColumn + 1; i < iColumns; i++) {
		for(int g = 1; g < iRows; g++) {
			ppSmallerMatrix[i - posPivot.iColumn - 1][g - 1] = ppMatrix[i][g];
		}
	}

	iRows--; 
	iRowsOffset++;
	iColumns -= posPivot.iColumn + 1;
	iColumnsOffset += posPivot.iColumn + 1;

	POSITION posNewPivot = StepOne(ppSmallerMatrix);
	if(posNewPivot.iColumn == -1) {
		// Rücksetzen
		iRowsOffset -= (iRowsSave - iRows);
		iRows = iRowsSave;
		iRowsOffset -= (iColumnsSave - iColumns);
		iColumns = iColumnsSave;
		return pivots; // Fertig
	}
	StepTwo(ppSmallerMatrix, posNewPivot);
	//printf("Schritt 1 & 2 - ");
	//PrintMatrix(ppSmallerMatrix);
	StepThree(ppSmallerMatrix, posNewPivot);
	//printf("Schritt 3 - ");
	//PrintMatrix(ppSmallerMatrix);
	StepFour(ppSmallerMatrix, posNewPivot);
	//printf("Schritt 4 - ");
	//PrintMatrix(ppSmallerMatrix);
	pivots = StepFive(ppSmallerMatrix, posNewPivot);
	//printf("Schritt 5 - ");
	//PrintMatrix(ppSmallerMatrix);

	AddPosition(&pivots, posNewPivot);

	for(int i = 0; i < pivots.iCount; i++) {
		pivots.pPositions[i].iColumn += posPivot.iColumn + 1; // Auf neue Größe anpassen
		pivots.pPositions[i].iRow += 1;
	}

	// Rücksetzen
	iRowsOffset -= (iRowsSave - iRows);
	iRows = iRowsSave;
	iRowsOffset -= (iColumnsSave - iColumns);
	iColumns = iColumnsSave;

	// Eingruppieren der neuen Matrix
	for(int i = posPivot.iColumn + 1; i < iColumns; i++) {
		for(int g = 1; g < iRows; g++) {
			ppMatrix[i][g] = ppSmallerMatrix[i - posPivot.iColumn - 1][g - 1];
		}
	}

	// Freigeben des Speichers
	FreeMatrixMemory(ppSmallerMatrix, iColumns - posPivot.iColumn - 1);

	return pivots;
}

void StepSix(double ** ppMatrix, POSITION_ARRAY pivots)
{
	for(int i = 0; i < pivots.iCount; i++) {
		for(int g = pivots.pPositions[i].iRow - 1; g >= 0; g--) { // von "unten" (der Matrix) nach "oben" (der Matrix) zählen
			double scale = ppMatrix[pivots.pPositions[i].iColumn][g];

			if(scale == 0.0f) {
				continue; // mit nächster Zeile weitermachen
			}

			for(int h = 0; h < iColumns; h++) {
				ppMatrix[h][g] += - ppMatrix[h][pivots.pPositions[i].iRow] * scale;
			}
			fprintf(fOut, "Füge Elementarmatrix an: A[%d,%d](%lf)\n", i + iRowsOffset, 1 + iRowsOffset, -scale);
		}
	}
}

double ** AllocateMatrixMemory(int iMatrixColumns, int iMatrixRows)
{
	double ** ppMatrix;
	ppMatrix = (double **)malloc(sizeof(double *) * iMatrixColumns);

	for(int i = 0; i < iMatrixColumns; i++) {
		ppMatrix[i] = (double *)malloc(sizeof(double) * iMatrixRows);
	}
	return ppMatrix;
}

void FreeMatrixMemory(double ** ppMatrix, int iMatrixColumns)
{
	// Freigeben des Speichers
	for(int i = 0; i < iMatrixColumns; i++) {
		free(ppMatrix[i]);
	}
	free(ppMatrix);
}
