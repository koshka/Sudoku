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

��� ������� �������� � �������� BYTE ** pData
������ ������� 9�9
�������� ������� 1...9
������ ������ ����� �������� 0.

��������:
1. ������ ������ ������ � ���������� ������ ����������.
2. ���� ������������� ��������� �������� � ���������� ����������� �. 1.

************************************/

/**************************************************************************************************************/
class CSudokuSet // ������� ���������� ��������� ��� �������� ����� 1...9
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
bool CheckSudoku(BYTE ** pData) // �������� ������������ ����������
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
int SudokuFindMinPoint(CPointSudoku& p, BYTE ** pData) // ���� ������ � ���������� ��������� ������ �������� 1...9
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

	if (bFree && nMinVariances < 10) return 1; // �����, ���� ����� ��������� �����
	if (bFree) return -1; // ��������� ����� ���� �� ����, �� ��������� ������ ������
	return 0; // ��� ���������� �����
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

	// ���� ������ � ���������� ������ ����������:
	iRet = SudokuFindMinPoint(pt, pData); 

	if (iRet == 0) return 1; // ������� �������
	if (iRet == -1) return -1; // ��������� ����������

	BYTE ** pSaveData = SudokuCreateTempArray(pData);
	SudokuCopyArray(pData, pSaveData); // ��������� ��, ��� ����

	for (BYTE variance = 1; variance <= 9; variance++) // �������� ������� �� ��������� ������ ����� �� 1 �� 9
	{
		pData[pt.i][pt.j] = variance;
		
		if (! CheckSudoku(pData)) continue; // ����� ����� ��������� ������. ���� ���������.
	
		iRet = SudokuSolveRecursive(pData); // ��������� ������� ����������

		if (iRet == -1) // ��������� ����������
		{
			SudokuCopyArray(pSaveData, pData); // ��������������� ������ � ���� ��������� ����� �� �� �� �����
			continue;
		}

		if (iRet == 1) // ������� �������
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
