// LetsGo.cpp : This file contains the 'main' function.
// Program execution begins and ends there.

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
			if (stone == BLACK)
			{
				MyBoard[cur_x][cur_y].black_score += cur_inf;
			}
			else if (stone == WHITE)
			{
				MyBoard[cur_x][cur_y].white_score += cur_inf;
			}

			MyBoard[cur_x][cur_y].last_visitor = ToLastVisitor(orig_x, orig_y);

			ExertInfluence(cur_x + 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x - 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x, cur_y - 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			ExertInfluence(cur_x, cur_y + 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
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
		cout << endl;
		bool legalMove = PlaceStone(x, y, newStone, is_test);

		// UNCOMMENT TO SEE PLAYER INFLUENCE:
		/*
		cout << "Your influence: " << endl;
		cout << "----------------------" << endl;
		CalculateInfluence();
		PrintInfluence();
		cout << endl;
		*/

		// UNCOMMENT TO SEE AGENT INFLUENCE BASED ON YOURS:
		TakeAITurn();
		/*
		cout << "Opponent's influence: " << endl;
		cout << "----------------------" << endl;
		CalculateInfluence();
		PrintInfluence();
		cout << endl;
		*/

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


void intro(){
	cout << endl;
	cout << "Hello, player. Here, you will be playing a game of GO." << endl;
	cout << endl;
	cout << "Notes: " << endl;
	cout << "---------------" << endl;
	cout << "➤ Player (you) are denoted by 'o' on the board." << endl;
	cout << "➤ The agent is denoted by 'x' on the board." << endl;
	cout << "➤ A move consists of an (x,y) coordinate." << endl;
	cout << "➤ The objective is to gain as much board space as you can against "
		<< "the agent." << endl;
	cout << "➤ By default, the game will simply show you where the pieces are being placed. If " << endl;
	cout << "you would like to see how the agent is making its decisions please see instructions in the code." << endl;
	cout << "➤ Like in real Go, you may quit anytime you wish." << endl;
	cout << endl;
}

int main()
{
	GoBoard MyGoBoard(9);
	bool done = false;
	char input_char;
	int input_x, input_y;

	// Start with empty board:
	intro();
	MyGoBoard.PrintBoard();

	while (!done)
	{
		cout << "Enter x-coordinate: ";
		cin >> input_x;
		cout << "Enter the y-coordinate: ";
		cin >> input_y;

		if (!MyGoBoard.TakePlayerTurn(input_x, input_y, BLACK, false))
			cout << "That is an illegal move... no stone placed." << endl;
		cout << endl;

		// Update board with new moves, displays pieces being placed
		// by default, see TakePlayerTurn for details:
		MyGoBoard.PrintBoard();

		cout << "Would you like to keep playing (y/n): ";
		cin >> input_char;
		if (input_char == 'n' || input_char == 'N')
			done = true;
		cout << endl;
	}
	return 0;
}
