#include "main.h"

// Hauptfunktion. Programmaufrufspunkt.
int main(int argc, char * argv[])
{
	double ** ppMatrix = ReadMatrix();
	double ** ppTmpMatrix = AllocateMatrixMemory(iColumns + iRows, iRows);

	iColumns += iRows;
	for(int i = (iColumns - iRows); i < iColumns; i++) {
		for(int g = 0; g < iRows; g++) {
			if(g == i - iColumns + iRows) {
				ppTmpMatrix[i][g] = 1.0f;

			} else {
				ppTmpMatrix[i][g] = 0.0f;
			}
		}
	}
	CopyMatrix(ppTmpMatrix, ppMatrix, iRows, iColumns - iRows);
	ppMatrix = ppTmpMatrix;

	if(ppMatrix == NULL) {
		printf("Error while reading matrix. Terminate programm. / Fehler beim Einlesen der Matrix. Beende Programm.\n");
		//system("pause");
		WaitForKey();
		return 0;
	}

	printf("Eingelesen - ");
	PrintMatrix(ppMatrix, false);

	POSITION posPivot = StepOne(ppMatrix);
	if(posPivot.iColumn == -1) {
		printf("Fettig. Nix zu tun.\n");
		//system("pause");
		WaitForKey();
		return 0;
	}

	StepTwo(ppMatrix, &posPivot);
	printf("\nSchritt 1 & 2 - ");
	PrintMatrix(ppMatrix, false);
	StepThree(ppMatrix, posPivot);
	printf("\nSchritt 3 - ");
	PrintMatrix(ppMatrix, false);
	StepFour(ppMatrix, posPivot);
	printf("\nSchritt 4 - ");
	PrintMatrix(ppMatrix, false);
	POSITION_ARRAY pivots = StepFive(ppMatrix, posPivot);
	printf("\nSchritt 5 - ");
	PrintMatrix(ppMatrix, false);
	AddPosition(&pivots, posPivot);
	StepSix(ppMatrix, pivots);
	printf("\nFertig - ");
	PrintMatrix(ppMatrix, false);
	PrintMatrix(ppMatrix, true);
	//system("pause");
	WaitForKey();
	FreeMatrixMemory(ppMatrix, iColumns);
	return 0;
}

double ** ReadMatrix()
{
#ifdef _USE_WINDOWS_CODE
	printf("Wollen Sie die Matrix aus einer Datei laden oder \x81 \bber den Bildschirm eingeben? [Datei/Bildschirm]\n");
#else
	printf("Wollen Sie die Matrix aus einer Datei laden oder über den Bildschirm eingeben? [Datei/Bildschirm]\n");
#endif
	printf("  ");
	char answer[100];
	scanf("%s", answer);
	fflush(stdin);

	double ** ppMatrix;
	if(strcmp(answer, "Datei") == 0 || strcmp(answer, "D") == 0) {
		ppMatrix = ReadMatrixFromFile();
	} else {
		ppMatrix = ReadMatrixFromInput();
	}

	printf("Wollen Sie die Elementarmatrizen in eine Datei schreiben oder auf dem Bildschirm ausgeben? [Datei/Bildschirm]\n");
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
#ifdef _USE_WINDOWS_CODE
			printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht \x94 \bffnen: %s", answer);
#else
			printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht öffnen: %s", answer);
#endif
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
#ifdef _USE_WINDOWS_CODE
		printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht \x94 \bffnen: %s", answer);
#else
		printf("Fatal Error: Cannot open file: / Fataler Fehler: Kann Datei nicht öffnen: %s", answer);
#endif
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
			for(int l = 0; l < 1000; l++) {
				var[l] = '\0';
			}
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

void PrintMatrix(double ** ppMatrix, bool bPrintSMatrix)
{
	printf("Print %sMatrix\n", bPrintSMatrix ? "S-" : "");
	for(int i = 0; i < iRows; i++) {
		for(int g = (bPrintSMatrix ? iColumns - iRows : 0); g < (bPrintSMatrix ? iColumns : iColumns - iRows); g++) {
			printf("%lf", ppMatrix[g][i]);
			printf(";"); // Trennzeichen ausgeben
		}
		printf("\n");
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
			if(ppMatrix[i][g] > 0.00000001l || ppMatrix[i][g] < -0.00000001l) {
				res.iRow = g;
				res.iColumn = i;
				return res;
			} else {
				ppMatrix[i][g] = 0.0f;
			}
		}
	}
	return res;
}

void StepTwo(double ** ppMatrix, POSITION * posPivot)
{
	if(posPivot->iRow == 0) {
		return;
	}

	// Zeilen posPivot und 0 tauschen
	for(int i = 0; i < iColumns; i++) {
		double tmp = ppMatrix[i][0];
		ppMatrix[i][0] = ppMatrix[i][posPivot->iRow];
		ppMatrix[i][posPivot->iRow] = tmp;
	}

	fprintf(fOut, "Füge Elementarmatrix an: P[%d,%d]\n", posPivot->iRow + 1 + iRowsOffset, 1 + iRowsOffset);

	posPivot->iRow = 0;
}

void StepThree(double ** ppMatrix, POSITION posPivot)
{
	double fElement = ppMatrix[posPivot.iColumn][0];

	if(fElement == 0) {
		printf("Fat(al) error... Error in previous steps. Terminate programm. / Fett(al)er Fehler... Fehler in vorherigen Schritten. Beende Programm.\n");
		//system("pause");
		WaitForKey();
		exit(-1);
	}

	for(int i = 0; i < iColumns; i++) {
		ppMatrix[i][0] *= (1.0f / fElement);
	}
	fprintf(fOut, "Füge Elementarmatrix an: M[%d](%lf)\n", 1 + iRowsOffset, 1.0f / fElement);
}

void StepFour(double ** ppMatrix, POSITION posPivot)
{
	for(int i = 1; i < iRows; i++) {
		double scale = ppMatrix[posPivot.iColumn][i];

		if(scale < 0.00000001l && scale > -0.00000001l) {
			continue; // mit nächster Zeile weitermachen
		}

		for(int g = 0; g < iColumns; g++) {
			ppMatrix[g][i] += - ppMatrix[g][0] * scale;
		}
		fprintf(fOut, "Füge Elementarmatrix an: G[%d,%d](%lf)\n", 1 + i + iRowsOffset, 1 + iRowsOffset, -scale);
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
	
	double ** ppSmallerMatrix = AllocateMatrixMemory(iColumns - posPivot.iColumn - 1, iRows - 1);

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
//		iRowsOffset -= (iRowsSave - iRows);
//		iRows = iRowsSave;
//		iRowsOffset -= (iColumnsSave - iColumns);
//		iColumns = iColumnsSave;
		return pivots; // Fertig
	}
	
	StepTwo(ppSmallerMatrix, &posNewPivot);
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
		pivots.pPositions[i].iColumn += posPivot.iColumn + 1; // Auf neue Grösse anpassen
		pivots.pPositions[i].iRow += 1;
	}

	// Rücksetzen
	iRowsOffset -= (iRowsSave - iRows);
	iRows = iRowsSave;
	iColumnsOffset -= (iColumnsSave - iColumns);
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

			if(scale < 0.00000001l && scale > -0.00000001l) {
				continue; // mit nächster Zeile weitermachen
			}

			for(int h = 0; h < iColumns; h++) {
				ppMatrix[h][g] += - ppMatrix[h][pivots.pPositions[i].iRow] * scale;
			}
			fprintf(fOut, "Füge Elementarmatrix an: G[%d,%d](%lf)\n", 1 + i + iRowsOffset, 1 + iRowsOffset, -scale);
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

void WaitForKey()
{
#ifdef _USE_WINDOWS_CODE
	system("pause");
#else
	system("read -n1 -r");
#endif
}

void CopyMatrix(double ** ppBuf, double ** ppIn, int iRowsToCopy, int iColumnsToCopy)
{
	for(int i = 0; i < iRowsToCopy; i++) {
		for(int g = 0; g < iColumnsToCopy; g++) {
			ppBuf[i][g] = ppIn[i][g];
		}
	}
}
