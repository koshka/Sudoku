#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <stdio.h>
#include <windows.h>
#include <fstream>
using namespace std;

typedef int (*ptr1)(int, int);
typedef int (*ptr2)(int);

HMODULE hLibrary = LoadLibraryA("Binary.dll");
ptr1 AddOneInPos;
ptr1 RemoveOneFromPos;
ptr2 NumOf1;
ptr1 VerifyIsOne;



//list of possible variants for each positions 0..80
struct MyList
{
	int num_of_elements;
	int list;
};
//field, add to vector<CArr*> solutions
struct CArr
{
	int arr[9][9];
};

class CField
{
public:
	//default constructor
	CField()
	{
		m_full_number = 81;
		m_visited = vector<bool>(81, false);
		memset(m_lines, 0, sizeof(int) * 9);
		memset(m_columns, 0, sizeof(int) * 9);
		memset(m_blocks, 0, sizeof(int) * 9);
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j)
				m_desk[i][j] = 0;
	}
	//constructor with parameters
	CField(CField& field)
	{
		for (int i = 0; i < 9; ++i)
		{
			for(int j = 0; j < 9; ++j)
				m_desk[i][j] = field.m_desk[i][j];

		}
		m_full_number = field.m_full_number;
		m_variants = field.m_variants;
		m_visited = field.m_visited;
		memcpy(m_lines, field.m_lines, sizeof(int) * 9);
		memcpy(m_columns, field.m_columns, sizeof(int) * 9);
		memcpy(m_blocks, field.m_blocks, sizeof(int) * 9);
		m_q = field.m_q;
	}

	//read field from file, where position or 

	void ReadData(BYTE** pData)
	{
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j)
				m_desk[i][j] = (int)pData[i][j];
	}
	void OutputData(int** pData)
	{
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j)
				pData[i][j] = m_desk[i][j];
	}
	//print field in output.txt file
	void CField::Print()
	{
		ofstream ofs("output.txt", ios::out|ios::app);
		ofs << endl;
		for (int i = 0; i < 9; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				ofs << m_desk[i][j];
				if ((j + 1) % 3 == 0)
				{
					ofs << ' ';
				}
			}
			ofs << endl;
			if ((i + 1) % 3 == 0)
			{
				ofs << endl;
			}
		}
		ofs << endl;
	}
	//index from {0-80}
	//deleted index_value from lines of possible values in
	//vector<MyList> variants for each position from 
	//line/column/block which contain this index
	void NumInLine(int index, MyList* List)
	{
		int i = index / 9;
		for (int j = 0; j < 9; ++j)
		{
			if (j == index % 9)
				continue;
			if (m_desk[i][j] != 0)
			{
				if (VerifyIsOne(List->list, m_desk[i][j] - 1))
				{
					(*List).list = 
						RemoveOneFromPos((*List).list, m_desk[i][j] - 1);
					List->num_of_elements--;
				}
			}
		}
	}

	void NumInColumn(int index, MyList* List)
	{
		int j = index % 9;
		for (int i = 0; i < 9; ++i)
		{
			if (i == index / 9)
				continue;
			if (m_desk[i][j] != 0)
			{
				if (VerifyIsOne((*List).list, m_desk[i][j] - 1))
				{
					(*List).list = 
						RemoveOneFromPos((*List).list, m_desk[i][j] - 1);
					List->num_of_elements--;
				}
			}
		}
	}

	void NumInBlock(int index, MyList* List)
	{
		int i = index / 9;
		int j = index % 9;
		int i_x  = i / 3 * 3;
		int j_x = j / 3 * 3;

		for (int k = i_x; k < i_x + 3; ++k)
			for (int m = j_x; m < j_x + 3; ++m)
			{
				if (k != i && m != j)
					if (m_desk[k][m] != 0)
					{
						if (VerifyIsOne((*List).list, m_desk[k][m] - 1))
						{
							(*List).list = 
								RemoveOneFromPos((*List).list, m_desk[k][m] - 1);
							List->num_of_elements--;
						}
					}
			}
	}
	//for each index from {0-80} make MyList for vector<MyList> variants
	void CalcFirstField()
	{
		m_variants.reserve(81);
		for (int index = 0; index < 81; ++index)
		{
			int i = index / 9;
			int j = index % 9;
			MyList List;
			List.num_of_elements = 9;
			if (m_desk[i][j] != 0)
			{
				List.num_of_elements = -1;
				List.list = 0;
				m_variants.push_back(List);
				continue;
			}
			//1 - numbers, which can be put, 0 - can not be put
			List.list = 511;
			NumInLine(index, &List);
			NumInColumn(index, &List);
			NumInBlock(index, &List);
			if (List.num_of_elements == 1)
				{
					m_q.push(index);
					m_visited[index] = true;
				}
			m_variants.push_back(List);
		}
	}

	/*----------------------------------------------------------------- */
	//add_number - number from {1-9} or 0 if number is unknown
	//index from {0-80}
	//return true if number have added, false otherwise
	bool AddNumber(int index, int add_number)
	{
		CField NewField(*this);
		//if unknown number
		if (add_number == 0)
		{
			for (int i = 0; i < 9; ++i)
			{
				if(VerifyIsOne(NewField.m_variants[index].list, i))
				{
					add_number = i + 1;
					break;
				}
				else
					continue;
			}
		}
		int i = index / 9;
		int j = index % 9;

		NewField.m_desk[i][j] = add_number;
		AddOneInPos(m_lines[i], add_number - 1);
		AddOneInPos(m_columns[j], add_number - 1);
		AddOneInPos(m_blocks[i / 3 * 3 + j / 3], add_number - 1);
		NewField.m_full_number--;
		NewField.m_variants[index].num_of_elements = -1;
		NewField.m_variants[index].list = 0;

		//solved
		if (NewField.Final())
		{
			*this = NewField;
			return true;
		}

		//if this value of index not from the queue
		if (!NewField.m_visited[index])
			NewField.m_visited[index] = true;
		int initial_num = this->m_full_number;
		*this = NewField.Recalc(add_number, index, *this);

		if(NewField.m_variants[index].num_of_elements != -1 ||
		   initial_num == NewField.m_full_number)
			return false;
		else
			return true;
	}
	//add_number from {1-9}, index from {0-80}
	//make changes in m_variants, add in m_q indexes 
	//if num_of_elements == 1
	//return new field if number is added correctly or 
	//previous field otherwise
	CField Recalc(int add_number, int index, CField& PreviosField)
	{
		CField NewField(*this);
		int i = index / 9;
		int j = index % 9;
		int i_x = i / 3 * 3;
		int j_x = j / 3 * 3;

		for (int k = 0; k < 9; ++k)//column
			if (NewField.m_desk[k][j] == 0 && 
				VerifyIsOne(NewField.m_variants[k * 9 + j].list, add_number - 1))
			{
				int ind = k * 9 + j;
				NewField.m_variants[ind].list = 
					RemoveOneFromPos(NewField.m_variants[ind].list, add_number - 1);
				NewField.m_variants[ind].num_of_elements--;
				//error, return previous field
				if (NewField.m_variants[ind].num_of_elements == 0 && 
					NewField.m_desk[k][j] == 0)
				{
					*this = PreviosField; 
					return PreviosField;
				}
				if (NewField.m_variants[ind].num_of_elements == 1)
					if (!NewField.m_visited[ind])//haven't been in queue yet
					{
						NewField.m_q.push(ind);
						NewField.m_visited[ind] = true;
					}
			}

		for (int m = 0; m < 9; ++m)//line
			if	(NewField.m_desk[i][m] == 0  && 
				 VerifyIsOne(NewField.m_variants[i * 9 + m].list, add_number - 1))
			{
				int ind = i * 9 + m;
				NewField.m_variants[ind].list = 
					RemoveOneFromPos(NewField.m_variants[ind].list, add_number - 1);
				NewField.m_variants[ind].num_of_elements--;
				//error, return previous field
				if (NewField.m_variants[ind].num_of_elements == 0 && 
					NewField.m_desk[i][m] == 0)
				{
					*this = PreviosField; 
					return PreviosField;
				}
				if (NewField.m_variants[ind].num_of_elements == 1)
					if(!NewField.m_visited[ind])//haven't been in queue yet
					{
						NewField.m_q.push(ind);
						NewField.m_visited[ind] = true;
					}
			}

		for (int p = i_x; p < i_x + 3; ++p)//the rest 4 numbers in block
			for (int l = j_x; l < j_x + 3; ++l)
			{
				if (p != i && l != j)
					if	(NewField.m_desk[p][l] == 0 && 
						  VerifyIsOne(NewField.m_variants[p * 9 + l].list, add_number - 1))
					{
						int ind = p * 9 + l;
						NewField.m_variants[ind].list = 
							RemoveOneFromPos(NewField.m_variants[ind].list, add_number - 1);
						NewField.m_variants[ind].num_of_elements--;
						//error, return previous field
						if (NewField.m_variants[ind].num_of_elements == 0 && 
							NewField.m_desk[p][l] == 0)
						{
							*this = PreviosField; 
							return PreviosField;
						}
						if (NewField.m_variants[ind].num_of_elements == 1)
							if(!NewField.m_visited[ind])//haven't been in queue yet
							{
								NewField.m_q.push(ind);
								NewField.m_visited[ind] = true;
							}
					}
			}
		*this = NewField;
		return NewField;
	}
	//PreviousField - before changing which have lead to the queue
	//add numbers for each index from the q
	//return new_field if each numbers added correctly
	//or previous_field otherwise
	bool VerifQueue(CField& PreviousField)
	{
		CField NewField(*this);
		while (!NewField.m_q.empty())
		{
			int current = NewField.m_q.front();
			NewField.m_q.pop();
			if (!NewField.AddNumber(current, 0)) return false;
		}
		*this = NewField;
		return true;
	}
	/*------------------------------------------------------------------*/
	//check if number from {1-9} can be added
	//for each block/column/line
	bool Blocks()
	{
		CField NewField(*this);
		for (int b = 0; b < 9; ++b)//number of block
		{
			int cnt = 0;
			int add_index;
			//coordinates of very left position block
			int i = b / 3 * 3;
			int j = (b % 3) * 3;
			for (int num = 1; num <= 9; ++num)
			{
				//this number not in this block yet
				if(!VerifyIsOne(NewField.m_blocks[b], num - 1))
				{
					cnt = 0;
					for (int m = i; m < i + 3; ++m)
						for (int k = j; k < j + 3; ++k)
						{
							int ind = m * 9 + k;
							if (NewField.m_variants[ind].list != 0 &&
							   VerifyIsOne(NewField.m_variants[ind].list, num - 1))
							{
								++cnt;
								add_index = ind;
								if (cnt > 1) break;
							}
						}
					if (cnt == 1)
					{
						if (!NewField.AddNumber(add_index, num)) return false;
						//verifying wasn't successful
						if (!NewField.VerifQueue(*this)) return false;
					}
				}
			}
		//////////verification of pairs//////////////
			int curr_index;
			for (int m = i; m < i + 3; ++m)
				for (int k = j; k < j + 3; ++k)
				{
					curr_index = m * 9 + k;
					if (NewField.m_variants[curr_index].num_of_elements == 2)
					{
						int num_of_duplicates = 0;
						int index_of_duplicate = 0;
						for (int p = m; p < i + 3; ++p)
							for (int l = j; l < j + 3; ++l)
							{
								if ((p == m && l > k) || (p > m))
								{
									if (NewField.m_variants[curr_index].list 
										== NewField.m_variants[p * 9 + l].list)
									{
										index_of_duplicate = p * 9 + l;
										++num_of_duplicates;
										//there are more then two position
										//with equal range of possible numbers
										if (num_of_duplicates > 1)
											return false;
									}
								}
								else
									continue;
							}
						if (num_of_duplicates == 1)
						{
							for (int p = i; p < i + 3; ++p)
								for (int l = j; l < j + 3; ++l)
								{
									int ind = p * 9 + l;
									if (ind == index_of_duplicate || ind == curr_index)
										continue;
									else
										if (NewField.m_variants[ind].list != 0)
										{
											NewField.m_variants[ind].list =	NewField.m_variants[ind].list &
																		   (~NewField.m_variants[curr_index].list);
											NewField.m_variants[ind].num_of_elements = 
												NumOf1(NewField.m_variants[ind].list);
											if (NewField.m_variants[ind].num_of_elements == 0)
												return false;
											if (NewField.m_variants[ind].num_of_elements == 1)
											{
												if (!NewField.AddNumber(ind, 0)) return false;
												//verifying wasn't successful
												if (!NewField.VerifQueue(*this)) return false;
											}
										}
								}
						}

					}
				}
		}
		*this = NewField;
		return true;
	}
	bool Lines()
	{
		CField NewField(*this);
		for (int i = 0; i < 9; ++i)//number of line
		{
			int cnt = 0;
			int add_index;

			for (int num = 1; num <= 9; ++num)
			{
				if (!VerifyIsOne(NewField.m_lines[i], num - 1))
				{
					for (int j = 0; j < 9; ++j)
					{
						int ind = i * 9 + j;
						if (NewField.m_variants[ind].list != 0 &&
							 VerifyIsOne(NewField.m_variants[ind].list, num - 1))
						{
							++cnt;
							add_index = ind;
							if (cnt > 1) break;
						}
					}
					if (cnt == 1)
					{
						if (!NewField.AddNumber(add_index, num)) return false;
						//verifying wasn't successful
						if (!NewField.VerifQueue(*this)) return false;
					}
					cnt = 0;
				}
			}
			int curr_index;
			for (int m = 0; m < 9; ++m)
			{
				curr_index = i * 9 + m;
				if (NewField.m_variants[curr_index].num_of_elements == 2)
				{
					int num_of_duplicates = 0;
					int index_of_duplicate = 0;
					for(int p = m + 1; p < 9; ++p)
					{
						if (NewField.m_variants[curr_index].list 
							== NewField.m_variants[i * 9 + p].list)
						{
							index_of_duplicate = i * 9 + p;
							++num_of_duplicates;
							//there are more then two position with
							//equal range of possible numbers
							if (num_of_duplicates > 1)
								return false;
						}
					}
					
					if (num_of_duplicates == 1)
					{
						for (int p = 0; p < 9; ++p)
						{
							int ind = i * 9 + p;
							if (ind == index_of_duplicate || ind == curr_index)
								continue;
							else
								if (NewField.m_variants[ind].list != 0)
								{
									NewField.m_variants[ind].list =	NewField.m_variants[ind].list &
																   (~NewField.m_variants[curr_index].list);
									NewField.m_variants[ind].num_of_elements 
									= NumOf1(NewField.m_variants[ind].list);
									if (NewField.m_variants[ind].num_of_elements == 0)
										return false;
									if (NewField.m_variants[ind].num_of_elements == 1)
									{
										if (!NewField.AddNumber(ind, 0)) return false;
										//verifying wasn't successful
										if (!NewField.VerifQueue(*this)) return false;
									}
								}
						}	
					}
				}
			}
		}
		
		*this = NewField;
		return true;
	}
	bool Columns()
	{
		CField NewField(*this);
		for (int j = 0; j < 9; ++j)//number of column
		{
			int cnt = 0;
			int add_index;
			for (int num = 1; num <= 9; ++num)
			{
				if (!VerifyIsOne(NewField.m_columns[j], num - 1))
				{
					for (int i = 0; i < 9; ++i)
					{
						int ind = i * 9 + j;
						if (NewField.m_variants[ind].list != 0 && 
							 VerifyIsOne(NewField.m_variants[ind].list, num - 1))
							{
								++cnt;
								add_index = ind;
								if (cnt > 1) break;
							}
					}
					if (cnt == 1)
					{
						if (!NewField.AddNumber(add_index, num)) return false;
						//verifying wasn't successful
						if (!NewField.VerifQueue(*this)) return false;
					}
					cnt = 0;
				}
			}
			//////////////////////////////////
			int curr_index;
			for(int m = 0; m < 9; ++m)
			{
				curr_index = m * 9 + j;
				if (NewField.m_variants[curr_index].num_of_elements == 2)
				{
					int num_of_duplicates = 0;
					int index_of_duplicate = 0;
					for (int p = m + 1; p < 9; ++p)
					{
						if (NewField.m_variants[curr_index].list 
										== NewField.m_variants[p * 9 + j].list)
						{
							index_of_duplicate = p * 9 + j;
							++num_of_duplicates;
							//there are more then two position with
							//equal range of possible numbers
							if (num_of_duplicates > 1)
								return false;
						}
					}
					if (num_of_duplicates == 1)
					{
						for (int p = 0; p < 9; ++p)
						{
							int ind = p * 9 + j;
							if (ind == index_of_duplicate || ind == curr_index)
								continue;
							else
								if (NewField.m_variants[ind].list != 0)
								{
									NewField.m_variants[ind].list =	NewField.m_variants[ind].list &
																   (~NewField.m_variants[curr_index].list);
									NewField.m_variants[ind].num_of_elements 
									= NumOf1(NewField.m_variants[ind].list);
									if (NewField.m_variants[ind].num_of_elements == 0)
										return false;
									if (NewField.m_variants[ind].num_of_elements == 1)
									{
										if (!NewField.AddNumber(ind, 0)) return false;
										//verifying wasn't successful
										if (!NewField.VerifQueue(*this)) return false;
									}
								}
						}
					}
				}
			}
			
		}
		*this = NewField;
		return true;
	}
	//return true if field is filled, false otherwise
	bool Final()
	{
		bool is_full = true;
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j)
				if (m_desk[i][j] == 0)
				{
					is_full = false;
					break;
				}
		return is_full;
	}
	/*------------------------------------------------------------------*/
	//recurrent function for forsed addition of possible numbers
	bool Search(vector<CArr>* solutions, int N, bool is_write)
	{
		ofstream ofs("output.txt", ios::out|ios::app);
		if (is_write)
		{
			ofs << "Enter the recursion" << endl;
		}
		CField NewField(*this);
		for (int n = 2; n < 3; ++n)
		{
			int index = 0;
			bool flag = false;
			while (NewField.m_variants[index].num_of_elements != n)
				++index;
			for (int i = 0; i < 9; ++i)//searching of n numbers
				if (VerifyIsOne(NewField.m_variants[index].list, i))
				{
					bool is_added = NewField.AddNumber(index , i + 1);
					if (is_write == true)
					{
						ofs << "i = " << index / 9 << " j = " << index % 9
							<< " number = " << i + 1 << endl;
						NewField.Print();
					}
					//haven't been added
					if (!is_added)
					{
						NewField = *this;
						continue;
					}
				
					//verifying wasn't successful
					if (!NewField.VerifQueue(*this))
					{
						NewField = *this;
						continue;
					}
					
					if (!NewField.Blocks() || !NewField.Lines() ||
						!NewField.Columns())
					{
						NewField = *this;
						continue;
					}
										
					//solved
					if (NewField.Final())
					{

						if (solutions->size() < N)
						{
							flag = true;//one or more solutions have been found
							CArr pArr;
							for (int k = 0; k < 9; ++k)
								for (int m = 0; m < 9; ++m)
									pArr.arr[k][m] = NewField.m_desk[k][m];
							solutions->push_back(pArr);

							if (solutions->size() < N)
							{
								NewField = *this;
								continue;
							}
							else
							{
								if (is_write)
								{
									ofs << "Exit the recursion" << endl;
								}
								return true;
							}
						}
						else
						{
							if (is_write)
								{
									ofs << "Exit the recursion" << endl;
								}
							return true;
						}
					}
					else
					{
						if (solutions->size() < N)
						{
							flag = NewField.Search(solutions, N, is_write);
							if (solutions->size() < N)
							{
								NewField = *this;
								continue;
							}
							else
							{
								if (is_write)
								{
									ofs << "Exit the recursion" << endl;
								}
								return flag;
							}
						}
						else
						{
							if (is_write)
							{
								ofs << "Exit the recursion" << endl;
							}
							return true;
						}
					}
				}
				//there is no valid variants, previous position is bad
				if (!flag)
				{
					if (is_write)
					{
						ofs << "Exit the recursion" << endl;
					}
					return false;
				}
				else
				{
					if (is_write)
					{
						ofs << "Exit the recursion" << endl;
					}
					return true;
				}
		}
		if (is_write)
		{
			ofs << "Exit the recursion" << endl;
		}
		return true;
	}
		
	//bool lists and quantity of possible numbers for each position 0..80
	vector<MyList> m_variants;

	//queue of positions, which have one possible number to add
	queue<int> m_q;

private:
	int m_desk[9][9];
	//how much numbers still not added
	int m_full_number;
	//for each position: 0 - number is not added, 1 - is added
	vector<bool> m_visited;
	//numbers which have added to each line/column/block
	int m_lines[9];
	int m_columns[9];
	int m_blocks[9];
};
