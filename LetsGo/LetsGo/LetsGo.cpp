/* LetsGo.cpp : This file contains the methods and types for simplified version of the game Go
				and the AI agent which the user will play against.

				For ease of implementation, these two components of our project share a
				single namespace: GoBoard.

   Definitions of some higher level custom types are declared outside of the GoBoard

   A High Level description/explanation of this project with additional notes
   is contained in the provided README file
*/
#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <vector>

using namespace std;

// These values indicate whether a board space contains a black/white stone,
// is empty, or is an edge space.
const char WHITE = 'x';
const char BLACK = 'o';
const char EMPTY = '-';
const char EDGE = 'e';  // Edge space are not technically legal game spaces but have
						// important meaning in certain calculation and warrent explicit
						// documentation

// This struct represents a single space on the game board
// A GameBoard will be represented by a 2-D array of these space
struct BoardSpace
{
	int x_loc;
	int y_loc;
	int white_score; // the combined influence of all white pieces on this space
	int black_score; // the combined influence of all black pieces on this space
	string last_visitor;  // This value is used ExertInfluence to prevent a single stone
						  // from influencing the same space more than once per turn
	char stone;  // Either BLACK/WHITE/EMPTY/EDGE
	char owner;  // determined by relative value of white_score/black_score

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

// This struct is used by the AI Agent to store the values
// potential moves it could make
struct TestMove
{
	int x;
	int y;
	int score; // This is the total influence score the AI would have if it
			   //  made a move in the space (x, y)

	TestMove()
	{
		int x = -1;
		int y = -1;
		int score = -1;
	}
};

/* The main class of this project.

 Contains methods for:
	Storing/Representing a Go gameboard and all relevant meta-data
	Playing a SIMPLIFIED version of Go including placing/capturing stones on the board

Important Note: The AI is always White and the player is always Black
 */
class GoBoard
{

private:

	// Checks the board for any stones that are surrounded by stones of the opposing
	// color and removes them.
	// In this simplified version of the game, only single isolated stones may be 
	// captured (see README for more details)
	void RemoveCapturedStone(char newStone)
	{
		for (unsigned int x = 1; x < BoardSize - 1; x++)
		{
			for (unsigned int y = 1; y < BoardSize - 1; y++)
			{
				if ((MyBoard[x][y].stone != EMPTY) && (MyBoard[x][y].stone != newStone))
				{
					if (((MyBoard[x + 1][y].stone == newStone) || (MyBoard[x + 1][y].stone == EDGE)) &&
						((MyBoard[x - 1][y].stone == newStone) || (MyBoard[x - 1][y].stone == EDGE)) &&
						((MyBoard[x][y + 1].stone == newStone) || (MyBoard[x][y + 1].stone == EDGE)) &&
						((MyBoard[x][y - 1].stone == newStone) || (MyBoard[x][y - 1].stone == EDGE)))
					{
						MyBoard[x][y].stone = EMPTY;
					}
				}
			}
		}
	}

	// This function is used when the AI is checking potential movies it could take
	//
	// It functions almost identically to RemoveCapturedStone except that it calculates the 
	// resulting influence and then "un-captures" any captured stone to facilitate backtracking
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
					if (((MyBoard[x + 1][y].stone == newStone) || (MyBoard[x + 1][y].stone == EDGE)) &&
						((MyBoard[x - 1][y].stone == newStone) || (MyBoard[x - 1][y].stone == EDGE)) &&
						((MyBoard[x][y + 1].stone == newStone) || (MyBoard[x][y + 1].stone == EDGE)) &&
						((MyBoard[x][y - 1].stone == newStone) || (MyBoard[x][y - 1].stone == EDGE)))
					{
						MyBoard[x][y].stone = EMPTY;
						x_removed = x;
						y_removed = y;
					}
				}
			}
		}

		CalculateInfluence();

		// If a stone was removed... un-remove it
		if (x_removed > 0)
		{
			MyBoard[x_removed][y_removed].stone = BLACK;
		}
	}

	// A custom compare function used for sorting a list of TestMoves
	static bool compareTestMoves(TestMove a, TestMove b)
	{
		return (a.score > b.score);
	}

	void TakeAITurn()
	{
		vector<TestMove> possibleMoves;
		TestMove tempMove;

		// Try and store the results of all possible moves the AI could make this turn
		//
		// This "try, backtrack" operation takes place on the shared MyBoard array itself
		// and so can use meta-data member variables like total_white_score
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

		// Sort the list of all possible movies in descending order of total influence score
		sort(possibleMoves.begin(), possibleMoves.end(), &compareTestMoves);

		// Choose the move first score in the list, either the move with the highest score
		// or one of the moves tied for highest score; there are no special tie-breaker 
		// conditions (see README)
		tempMove = possibleMoves[0];

		// Take the move
		PlaceStone(tempMove.x, tempMove.y, WHITE, false);
	}

	// Converts space Coordinates into a string that is used as an ID
	// for checking which space last exerted influence on each space
	//
	// Used ExertInfluence to prevent a single stone from influencing
	// the same space more than once per turn
	string ToLastVisitor(int x, int y)
	{
		ostringstream oss;

		oss << x << y;
		return oss.str();
	}


	// Initialize the gameboard
	void InitBoard(BoardSpace** board)
	{
		// Indicate the board's "edge"
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			board[x][0].stone = EDGE;
			board[0][x].stone = EDGE;
			board[x][BoardSize - 1].stone = EDGE;
			board[BoardSize - 1][x].stone = EDGE;
		}

		// Zero-out all the playable board spaces
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
	//
	// Stops the influence algorithm from looping back around towards
	// the stone currently exerting influence. This ensures that
	// influence only "radiates" directly outwards
	bool IsCloserToOrig(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		double sqr1 = pow((x1 - x3), 2.0);
		double sqr2 = pow((y1 - y3), 2.0);
		double sqr3 = pow((x2 - x3), 2.0);
		double sqr4 = pow((y2 - y3), 2.0);

		return (sqr1 + sqr2) < (sqr3 + sqr4);
	}


	/* This is the corner-stone function of the AI Agent.

	   It is used to calculate the primary performance metric that the Agent uses
	   for decision making

	   A detailed explanation of this algorithm is included on our website https://sugoproject.weebly.com/blog
	   and in the README of this program
	*/
	void ExertInfluence(int cur_x, int cur_y, int prev_x, int prev_y, int orig_x, int orig_y, int cur_inf, char stone)
	{
		// Get the last visitor ID of the stone currently exerting influence.
		string mylastVis = ToLastVisitor(orig_x, orig_y);

		// IF the current space hasn't been influenced by the active stone AND
		// IF the current space is EMPTY OR it is the space containing the active stone AND
		// IF this space isn't closer to the origin than the previous space influenced
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

			// Store the coordinates of the stone currently exerting influence
			// as a string ID to insure it doesn't influence this space twice
			MyBoard[cur_x][cur_y].last_visitor = ToLastVisitor(orig_x, orig_y);

			// recur north
			ExertInfluence(cur_x + 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			// recur south
			ExertInfluence(cur_x - 1, cur_y, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			// recur east
			ExertInfluence(cur_x, cur_y - 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
			// recur west 
			ExertInfluence(cur_x, cur_y + 1, cur_x, cur_y, orig_x, orig_y, cur_inf - 1, stone);
		}
		return;
	}

public:

	unsigned int BoardSize;  // Size of the game board
	unsigned int total_white_score;  // Total influence score for white each turn
	unsigned int total_black_score;  // "                  "                     "

	BoardSpace **MyBoard; // Pointer to the 2D Board array

	GoBoard(int size)
	{
		BoardSize = size + 2; // Allows us to initialize an explicit "edge" around the board
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

	// Prints the board including all currently placed stones
	void PrintBoard()
	{
		for (unsigned int x = 0; x < BoardSize; x++)
		{
			for (unsigned int y = 0; y < BoardSize; y++)
			{
				if ((x == BoardSize - 1 ) || x == 0)
				{
					if (y != 0 && y < BoardSize - 1)
					{
						cout << y << ' ';
					}
					else
					{
						cout << MyBoard[x][y].stone << ' ';
					}
				}
				else if ((y == BoardSize - 1) || y == 0)
				{
					if (x != 0 && x < BoardSize)
					{
						cout << x << ' ';
					}
					else
					{
						cout << MyBoard[x][y].stone << ' ';
					}
				}
				else
				{
					cout << MyBoard[x][y].stone << ' ';
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	// Prints the influence map for the current turn
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

	// Prints the influence scores for black for each space
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

	// Prints the influence scores for white for each space
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

	// Initiates a single turn cycle
	bool TakePlayerTurn(int x, int y, char newStone, bool is_test)
	{
		cout << endl;
		bool legalMove = PlaceStone(x, y, newStone, is_test);

		if (legalMove)
		{
			CalculateInfluence();
			// UNCOMMENT TO SEE PLAYER INFLUENCE:
			/*
			cout << "Your influence: " << endl;
			cout << "----------------------" << endl;
			PrintInfluence();
			cout << endl;
			*/


			TakeAITurn();
			CalculateInfluence();
			// UNCOMMENT TO SEE AGENT INFLUENCE BASED ON YOURS:
			/*
			cout << "Opponent's influence: " << endl;
			cout << "----------------------" << endl;
			PrintInfluence();
			cout << endl;
			*/
		}
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
		// This actual inputs are swapped because the way the board gets 
		// printed is rotated 90-degrees. there is no functional change.
		cout << "Enter x-coordinate: ";
		cin >> input_y;
		cout << "Enter the y-coordinate: ";
		cin >> input_x;

		if (!MyGoBoard.TakePlayerTurn(input_x, input_y, BLACK, false))
			cout << "That is an illegal move... no stone placed." << endl;
		cout << endl;

		// Update board with new moves, display pieces being placed
		// by default. See TakePlayerTurn for details:
		MyGoBoard.PrintBoard();

		cout << "Would you like to keep playing (y/n): ";
		cin >> input_char;
		if (input_char == 'n' || input_char == 'N')
			done = true;
		cout << endl;
	}
	return 0;
}
