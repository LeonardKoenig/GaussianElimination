#include "background.h"

// In iColumns und iRows steht die Spalten- bzw. Zeilenanzahl der aktuell bearbeiteten Matrix
int iColumns, iRows;

// In iColumnsOffset und iRowsOffset steht die Spalten- bzw. Zeilenanzahl drin, die von der urspünglichen Matrix bereits entfernt wurde. Dies ist wichtig für die Ausgabe der korrekten Elementarmatrizen während der Rekursion.
int iColumnsOffset = 0;
int iRowsOffset = 0;

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

	//fprintf(fOut, "Füge Elementarmatrix an: P[%d,%d]\n", posPivot->iRow + 1 + iRowsOffset, 1 + iRowsOffset);

	posPivot->iRow = 0;
}

int StepThree(double ** ppMatrix, POSITION posPivot)
{
	double fElement = ppMatrix[posPivot.iColumn][0];

	if(fElement == 0) {
		return -1;
	}

	for(int i = 0; i < iColumns; i++) {
		ppMatrix[i][0] *= (1.0f / fElement);
	}
	//fprintf(fOut, "Füge Elementarmatrix an: M[%d](%lf)\n", 1 + iRowsOffset, 1.0f / fElement);
	return 0;
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
		//fprintf(fOut, "Füge Elementarmatrix an: G[%d,%d](%lf)\n", 1 + i + iRowsOffset, 1 + iRowsOffset, -scale);
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
		iRowsOffset -= (iRowsSave - iRows);
		iRows = iRowsSave;
		iRowsOffset -= (iColumnsSave - iColumns);
		iColumns = iColumnsSave;
		return pivots; // Fertig
	}
	
	StepTwo(ppSmallerMatrix, &posNewPivot);
	StepThree(ppSmallerMatrix, posNewPivot);
	StepFour(ppSmallerMatrix, posNewPivot);
	pivots = StepFive(ppSmallerMatrix, posNewPivot);

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
			//fprintf(fOut, "Füge Elementarmatrix an: G[%d,%d](%lf)\n", 1 + i + iRowsOffset, 1 + iRowsOffset, -scale);
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

void CopyMatrix(double ** ppBuf, double ** ppIn, int iRowsToCopy, int iColumnsToCopy)
{
	for(int i = 0; i < iRowsToCopy; i++) {
		for(int g = 0; g < iColumnsToCopy; g++) {
			ppBuf[g][i] = ppIn[g][i];
		}
	}
}

void SetMatrixDimensions(int iNewRows, int iNewColumns)
{
	iRows = iNewRows;
	iColumns = iNewColumns;
}

void DivideOutputAndSMatrix(double ** ppWorkMatrix, GAUSSIAN_ELIMINATION_OUTPUT * pOutputBuffer)
{
	// Treppennormalform
	for(int i = 0; i < iColumns - iRows; i++) {
		for(int g = 0; g < iRows; g++) {
			pOutputBuffer->ppReducedEcholonForm[i][g] = ppWorkMatrix[i][g];
		}
	}
	// S-Matrix
	for(int i = iColumns - iRows; i < iColumns; i++) {
		for(int g = 0; g < iRows; g++) {
			pOutputBuffer->ppSMatrix[i - (iColumns - iRows)][g] = ppWorkMatrix[i][g];
		}
	}
}
