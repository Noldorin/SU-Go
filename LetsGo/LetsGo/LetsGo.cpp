// LetsGo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

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
};

class GoBoard
{

private:

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
				MyBoard[x][y].stone = EMPTY;
			}
		}
	}

public:

	unsigned int BoardSize;
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

	void ExertInfluence(int cur_x, int cur_y, int orig_x, int orig_y, char stone)
	{
		int north_x, south_x, east_x, west_x, north_y, south_y, east_y, west_y;
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
		MyGoBoard.PrintBoard();

		cout << "Would you like to keep playing? y/n : ";
		cin >> input_char;

		if (input_char == 'n' || input_char == 'N')
		{
			done = true;
		}
	}

	return 0;
}