// LetsGo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>

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

class GoBoard
{

private:

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



	void ExertInfluence(int cur_x, int cur_y, int orig_x, int orig_y, int cur_inf, char stone)
	{
		string mylastVis = ToLastVisitor(orig_x, orig_y);

		if ((cur_inf > 0) &&
			(MyBoard[cur_x][cur_y].last_visitor.compare(mylastVis) != 0) &&
			(MyBoard[cur_x][cur_y].stone == EMPTY) || (cur_x == orig_x && cur_y == orig_y))
		{
			if (stone == BLACK)
			{
				MyBoard[cur_x][cur_y].black_score += cur_inf;
			}
			else if (stone == WHITE)
			{
				MyBoard[cur_x][cur_y].white_score += cur_inf;
			}
			MyBoard[cur_x][cur_y].last_visitor = ToLastVisitor(orig_x, orig_y);

			ExertInfluence(cur_x + 1, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x - 1, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x, cur_y - 1, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x, cur_y + 1, orig_x, orig_y, cur_inf - 1, stone);
		}
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

	void PrintScores()
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

	bool PlaceStone(int x, int y, char newStone)
	{
		if (MyBoard[x][y].stone == EMPTY)
		{
			MyBoard[x][y].stone = newStone;
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
			}
		}
		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if (MyBoard[x][y].stone == BLACK)
				{
					ExertInfluence(x, y, x, y, BoardSize - 2, BLACK);
				}
				else if (MyBoard[x][y].stone == WHITE)
				{
					ExertInfluence(x, y, x, y, BoardSize - 2, WHITE);
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
					}
					else if (MyBoard[x][y].white_score > MyBoard[x][y].black_score)
					{
						MyBoard[x][y].owner = WHITE;
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
		cout << endl << "Enter the x-coordinate of your move: ";
		cin >> input_x;
		cout << "Enter the y-coordinate of your move: ";
		cin >> input_y;

		if (!MyGoBoard.PlaceStone(input_x, input_y, BLACK))
		{
			cout << "That is an illegal move... no stone placed." << endl;
		}

		cout << endl;		
		MyGoBoard.PrintInfluence();
		MyGoBoard.PrintBoard();
		MyGoBoard.CalculateInfluence();
		MyGoBoard.PrintInfluence();

		cout << "Would you like to keep playing? y/n : ";
		cin >> input_char;

		if (input_char == 'n' || input_char == 'N')
		{
			done = true;
		}
	}

	return 0;
}