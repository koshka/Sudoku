#pragma once

class CSudokuDlg;

CSudokuDlg * g_pDlg = NULL;


/***********************************************************************************
* Created by Drakon
* drakon@dubki.ru
* 2007
***********************************************************************************/


typedef unsigned char BYTE;

/************************************

Все функции работают с массивом BYTE ** pData
Размер массива 9х9
Заполнен цифрами 1...9
Пустые клетки имеют значение 0.

Алгоритм:
1. Ищется пустая клетка с наименьшим числом кандидатов.
2. Туда подставляются возможные варианты и рекурсивно выполняется п. 1.

************************************/

/**************************************************************************************************************/
class CSudokuSet // простая реализация множества для проверки чисел 1...9
{
private:
	BYTE m_Data[10];

public:
	CSudokuSet() { for (int i = 0; i <= 9; i++) m_Data[i] = 0; }

	bool Set(BYTE n)
	{
		ASSERT (n >= 1 && n <= 9);
		if (m_Data[n] == 1) return false;
		m_Data[n] = 1;
		return true;
	}

	void Clear()
	{
		for (int i = 1; i <= 9; i++) m_Data[i] = 0;
	}
};
/**************************************************************************************************************/
bool CheckSudoku(BYTE ** pData) // проверка правильности заполнения
{
	CSudokuSet set;

	for (int iRow = 0; iRow < 9; iRow++)
	{
		set.Clear();
		for(int iCol = 0; iCol < 9; iCol++)
		{
			if (pData[iRow][iCol] == 0) continue;
			if (! set.Set(pData[iRow][iCol])) return false;
		}
	}

	for (int iCol = 0; iCol < 9; iCol++)
	{
		set.Clear();
		for(int iRow = 0; iRow < 9; iRow++)
		{
			if (pData[iRow][iCol] == 0) continue;
			if (! set.Set(pData[iRow][iCol])) return false;
		}
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			set.Clear();

			for (int iCol = 0; iCol < 3; iCol++)
				for(int iRow = 0; iRow < 3; iRow++)
				{
					if (pData[i*3 + iRow][j*3 + iCol] == 0) continue;
					if (! set.Set(pData[i*3 + iRow][j*3 + iCol])) return false;
				}
		}

	return true;
}
/**************************************************************************************************/
struct CPointSudoku 
{
	BYTE i;
	BYTE j;
	BYTE variances[10];
};
/**************************************************************************************************/
int SudokuFindMinPoint(CPointSudoku& p, BYTE ** pData) // ищет клетку с минимально возможным числом вариаций 1...9
{
	BYTE nVariances = 0, nMinVariances = 10, i, j, variance;
	bool bFree = false;
	BYTE variances[10];

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
		{
			if (pData[i][j] != 0) continue;

			nVariances = 0;
			bFree = true;
			memset(variances, 0, 10);

			for (variance = 1; variance <= 9; variance++)
			{
				pData[i][j] = variance;

				if (CheckSudoku(pData)) 
				{
					nVariances++;
					variances[variance] = 1;
				}
			}

			pData[i][j] = 0;

			if (nVariances == 0) continue;

			if (nVariances < nMinVariances)
			{
				p.i = i;
				p.j = j;
				// p.variances = nVariances;
				memcpy(p.variances, variances, 10);

				nMinVariances = nVariances;
			}
		}

	if (bFree && nMinVariances < 10) return 1; // нашли, куда можно поставить цифру
	if (bFree) return -1; // свободное место есть на поле, но поставить никуда нельзя
	return 0; // нет свободного места
}
/**************************************************************************************************/
void SudokuCopyArray(BYTE ** pSrc, BYTE ** pDst)
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			pDst[i][j] = pSrc[i][j];
}
/**************************************************************************************************/
BYTE ** SudokuCreateTempArray(BYTE ** pDataOrig)
{
	BYTE ** pData = new BYTE*[9];

	for (int i = 0; i < 9; i++)
		pData[i] = new BYTE[9];

	SudokuCopyArray(pDataOrig, pData);

	return pData;
}
/**************************************************************************************************/
void SudokuDeleteTempArray(BYTE ** pData)
{
	for (int i = 0; i < 9; i++)
		delete [] pData[i];

	delete [] pData;
}
/**************************************************************************************************/
/*
int SudokuSolveRecursive(BYTE ** pData)
{
	CPointSudoku pt;
	int iRet;

	// ищем клетку с наименьшим числом кандидатов:
	iRet = SudokuFindMinPoint(pt, pData); 

	if (iRet == 0) return 1; // решение найдено
	if (iRet == -1) return -1; // тупиковая комбинация

	BYTE ** pSaveData = SudokuCreateTempArray(pData);
	SudokuCopyArray(pData, pSaveData); // сохраняем то, что есть

	for (BYTE variance = 1; variance <= 9; variance++) // пытаемся ставить на найденную клетку цифры от 1 до 9
	{
		pData[pt.i][pt.j] = variance;
		
		if (! CheckSudoku(pData)) continue; // такую цифру поставить нельзя. берём следующую.
	
		iRet = SudokuSolveRecursive(pData); // запускаем функцию рекурсивно

		if (iRet == -1) // тупиковая комбинация
		{
			SudokuCopyArray(pSaveData, pData); // восстанавливаем массив и берём следующую цифру на то же место
			continue;
		}

		if (iRet == 1) // решение найдено
		{
			SudokuDeleteTempArray(pSaveData);
			return 1;
		}
	}

	SudokuDeleteTempArray(pSaveData);
	return -1;
}
*/
/**************************************************************************************************/
int SudokuSolve(BYTE ** pData, CSudokuDlg * pDlg)
{
	g_pDlg = pDlg;
//	return SudokuSolveRecursive(pData);
	return 0;
}
/**************************************************************************************************/
