#include "Move.h"
extern "C" {
    #include "jansson.h"
}
#include <string>

Move::Move(int _row, int _column, int _direction) : row(_row),
        column(_column), direction(_direction) {}

void Move::setScore(int _score) {
    score = _score;
}

int Move::getRow() {
	return row;
}

int Move::getColumn() {
    return column;
}

int Move::getDirection() {
    return direction;
}

int Move::getScore() {
    return score;
}

json_t* Move::toJson() {
    json_t *root = json_object();

    json_t *rowJson = json_integer(row);
    json_t *colJson = json_integer(column);
    json_t *dirJson = json_integer(direction);

    json_object_set_new(root, "row", rowJson);
    json_object_set_new(root, "column", colJson);
    json_object_set_new(root, "direction", dirJson);

    return root;
}
