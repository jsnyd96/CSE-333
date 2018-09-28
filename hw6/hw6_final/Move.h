#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "jansson.h"

class Move {

	public:
	Move(int _row, int _column, int _direction);

	void setScore(int _score);

	int getRow();

	int getColumn();

	int getDirection();

	int getScore();

    json_t* toJson();

	private:
	const int row;
	const int column;
	const int direction; // enum Direction{left, right, down, up}; 	
	int score;

	
};

#endif
