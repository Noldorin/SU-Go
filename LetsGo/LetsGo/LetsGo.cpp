// LetsGo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <vector>

using namespace std;

const char WHITE = 'x';
const char BLACK = 'o';
const char EMPTY = '-';
const char EDGE = 'e';

struct BoardSpace
{
	int x_loc;
	int y_loc;
	int white_score;
	int black_score;
	string last_visitor;
	char stone;
	char owner;

	BoardSpace()
	{
		int x_loc = 0;
		int y_loc = 0;
		int white_score = 0;
		int black_score = 0;
		string last_visitor = "";
		char stone = EMPTY;
		char owner = EMPTY;
	}
};

struct TestMove
{
	int x;
	int y;
	int score;

	TestMove()
	{
		int x = -1;
		int y = -1;
		int score = -1;
	}
};

class GoBoard
{

private:

	void RemoveCapturedStone(char newStone)
	{
		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if ((MyBoard[x][y].stone != EMPTY) && (MyBoard[x][y].stone != newStone))
				{
					if (MyBoard[x + 1][y].stone == newStone &&
						MyBoard[x - 1][y].stone == newStone &&
						MyBoard[x][y + 1].stone == newStone &&
						MyBoard[x][y - 1].stone == newStone)
					{
						MyBoard[x][y].stone = EMPTY;
					}
				}
			}
		}
	}

	void TestRemoveCapturedStone(char newStone)
	{
		int x_removed = -1;
		int y_removed = -1;

		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if ((MyBoard[x][y].stone != EMPTY) && (MyBoard[x][y].stone != newStone))
				{
					if (MyBoard[x + 1][y].stone == newStone &&
						MyBoard[x - 1][y].stone == newStone &&
						MyBoard[x][y + 1].stone == newStone &&
						MyBoard[x][y - 1].stone == newStone)
					{
						MyBoard[x][y].stone = EMPTY;
						x_removed = x;
						y_removed = y;
					}
				}
			}
		}

		CalculateInfluence();

		if (x_removed > 0)
		{
			MyBoard[x_removed][y_removed].stone = EMPTY;
		}
	}

	static bool compareTestMoves(TestMove a, TestMove b)
	{
		return (a.score > b.score);
	}
	
	void TakeAITurn()
	{		
		vector<TestMove> possibleMoves;
		TestMove tempMove;

		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if (MyBoard[x][y].stone == EMPTY)
				{
					PlaceStone(x, y, WHITE, true);

					tempMove.score = total_white_score;
					tempMove.x = x;
					tempMove.y = y;

					possibleMoves.push_back(tempMove);

					MyBoard[x][y].stone = EMPTY;
				}
			}
		}

		sort(possibleMoves.begin(), possibleMoves.end(), &compareTestMoves);

		tempMove = possibleMoves[0];

		PlaceStone(tempMove.x, tempMove.y, WHITE, false);
	}

	string ToLastVisitor(int x, int y)
	{
		ostringstream oss;

		oss << x << y;
		return oss.str();
	}

	void InitBoard(BoardSpace** board)
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			board[x][0].stone = EDGE;
			board[0][x].stone = EDGE;
			board[x][BoardSize - 1].stone = EDGE;
			board[BoardSize - 1][x].stone = EDGE;
		}

		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				board[x][y].x_loc = 0;
				board[x][y].y_loc = 0;
				board[x][y].white_score = 0;
				board[x][y].black_score = 0;
				board[x][y].last_visitor = "";
				board[x][y].stone = EMPTY;
				board[x][y].owner = EMPTY;
			}
		}
	}

	// Returns True if (x1, y1) is closer to (x3, y3) than (x2, y2) is.
	bool IsCloserToOrig(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		double sqr1 = pow((x1 - x3), 2.0);
		double sqr2 = pow((y1 - y3), 2.0);
		double sqr3 = pow((x2 - x3), 2.0);
		double sqr4 = pow((y2 - y3), 2.0);

		return (sqr1 + sqr2) < (sqr3 + sqr4);
	}

	void ExertInfluence(int cur_x, int cur_y, int prev_x, int prev_y, int orig_x, int orig_y, int cur_inf, char stone)
	{
		string mylastVis = ToLastVisitor(orig_x, orig_y);

		if ((cur_inf > 0) &&
			(MyBoard[cur_x][cur_y].last_visitor.compare(mylastVis) != 0) &&
			((MyBoard[cur_x][cur_y].stone == EMPTY) || (cur_x == orig_x && cur_y == orig_y)) &&
			(!(IsCloserToOrig(cur_x, cur_y, prev_x, prev_y, orig_x, orig_y)))
		   )
		{
			//cin.get();
			//cout << "exploring space " << cur_x << " " << cur_y << " with LV " << MyBoard[cur_x][cur_y].last_visitor << " and actual orig " << mylastVis;
			if (stone == BLACK)
			{
				//cin.get();
				//cout << "influencing space " << cur_x << " " << cur_y << " by " << cur_inf << endl;
				MyBoard[cur_x][cur_y].black_score += cur_inf;
			}
			else if (stone == WHITE)
			{
				MyBoard[cur_x][cur_y].white_score += cur_inf;
			}
			//else
			//{
			//	cout << "not influencing space " << cur_x << " " << cur_y << endl;
			//}
			MyBoard[cur_x][cur_y].last_visitor = ToLastVisitor(orig_x, orig_y);

			//cout << "recurring north" << endl;
			ExertInfluence(cur_x + 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			//cout << "recurring south" << endl;
			ExertInfluence(cur_x - 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			//cout << "recurring east" << endl;
			ExertInfluence(cur_x, cur_y - 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			//cout << "recurring west" << endl;
			ExertInfluence(cur_x, cur_y + 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
		}
		//cout << "returning >>> ";
		return;
	}
	 
public:

	unsigned int BoardSize;
	unsigned int total_white_score;
	unsigned int total_black_score;

	BoardSpace **MyBoard;

	GoBoard(int size)
	{
		BoardSize = size + 2;
		MyBoard = new BoardSpace*[BoardSize];

		for (unsigned int x = 0; x < BoardSize; x++)
		{
			MyBoard[x] = new BoardSpace[BoardSize];
		}

		InitBoard(MyBoard);
	}

	~GoBoard()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			delete [] MyBoard[x];
		}

		delete [] MyBoard;
	}

	void PrintBoard()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			for (unsigned int y = 0; y < BoardSize; y++)
			{
				cout << MyBoard[x][y].stone << ' ';
			}
			cout << endl;
		}
		cout << endl;
	}

	void PrintInfluence()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			for (unsigned int y = 0; y < BoardSize; y++)
			{
				if (MyBoard[x][y].stone != EDGE)
				{
					cout << MyBoard[x][y].owner << ' ';
				}
				else
				{
					cout << EDGE << ' ';
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void PrintBlackScores()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			for (unsigned int y = 0; y < BoardSize; y++)
			{
				if (MyBoard[x][y].stone != EDGE)
				{
					cout << MyBoard[x][y].black_score << "   ";
				}
				else
				{
					cout << EDGE << "   ";
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void PrintWhiteScores()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			for (unsigned int y = 0; y < BoardSize; y++)
			{
				if (MyBoard[x][y].stone != EDGE)
				{
					cout << MyBoard[x][y].white_score << "   ";
				}
				else
				{
					cout << EDGE << "   ";
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	bool TakePlayerTurn(int x, int y, char newStone, bool is_test)
	{
		bool legalMove = PlaceStone(x, y, newStone, is_test);
		CalculateInfluence();
		PrintInfluence();
		cin.get();

		TakeAITurn();
		CalculateInfluence();
		PrintInfluence();
		cin.get();

		return legalMove;
	}

	bool PlaceStone(int x, int y, char newStone, bool is_test)
	{
		if (MyBoard[x][y].stone == EMPTY)
		{
			MyBoard[x][y].stone = newStone;

			if (is_test)
			{
				TestRemoveCapturedStone(newStone);
			}
			else
			{
				RemoveCapturedStone(newStone);
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	void CalculateInfluence()
	{
		total_black_score = 0;
		total_white_score = 0;

		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				MyBoard[x][y].white_score = 0;
				MyBoard[x][y].black_score = 0;
				MyBoard[x][y].last_visitor.clear();
			}
		}
		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if (MyBoard[x][y].stone == BLACK)
				{
					ExertInfluence(x, y, x, y, x, y, BoardSize / 2, BLACK);
				}
				else if (MyBoard[x][y].stone == WHITE)
				{
					ExertInfluence(x, y, x, y, x, y, BoardSize / 2, WHITE);
				}
			}
		}
		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if (MyBoard[x][y].stone == EMPTY)
				{
					if (MyBoard[x][y].white_score < MyBoard[x][y].black_score)
					{
						MyBoard[x][y].owner = BLACK;
						total_black_score++;
					}
					else if (MyBoard[x][y].white_score > MyBoard[x][y].black_score)
					{
						MyBoard[x][y].owner = WHITE;
						total_white_score++;
					}
					else
					{
						MyBoard[x][y].owner = EMPTY;
					}
				}
			}
		}
	}
};


int main()
{
	GoBoard MyGoBoard(9);
	bool done = false;
	char input_char;
	int input_x, input_y;


	MyGoBoard.PrintBoard();

	while (!done)
	{
		cout << endl << "Enter the x-coordinate of your move for black: ";
		cin >> input_x;
		cout << "Enter the y-coordinate of your move black: ";
		cin >> input_y;

		if (!MyGoBoard.TakePlayerTurn(input_x, input_y, BLACK, false))
		{
			cout << "That is an illegal move... no stone placed." << endl;
		}

		cout << endl;
		MyGoBoard.PrintBoard();

		//cout << endl << "Enter the x-coordinate of your move for white: ";
		//cin >> input_x;
		//cout << "Enter the y-coordinate of your move white: ";
		//cin >> input_y;
		//cin.ignore();

		//if (!MyGoBoard.PlaceStone(input_x, input_y, WHITE))
		//{
		//	cout << "That is an illegal move... no stone placed." << endl;
		//}

		cout << endl;
		MyGoBoard.PrintBoard();

		cout << "Would you like to keep playing? y/n : ";
		cin >> input_char;
		cin.ignore();
		if (input_char == 'n' || input_char == 'N')
		{
			done = true;
		}
	}

	return 0;
}