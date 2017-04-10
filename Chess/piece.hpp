#pragma once

enum colour {
	BLACK = 0,
	WHITE = 1
};

enum type {
	KING = 0,
	QUEEN = 1,
	ROOK = 2,
	BISHOP = 3,
	KNIGHT = 4,
	PAWN = 5
};

enum direction {
	NONE = 0,
	NORTH = 1,
	NORTHEAST = 2,
	EAST = 3,
	SOUTHEAST = 4,
	SOUTH = 5,
	SOUTHWEST = 6,
	WEST = 7,
	NORTHWEST = 8
};

enum unicode {
	WHITE_KING = 9812,
	WHITE_QUEEN = 9813,
	WHITE_ROOK = 9814,
	WHITE_BISHOP = 9815,
	WHITE_KNIGHT = 9816,
	WHITE_PAWN = 9817,
	BLACK_KING = 9818,
	BLACK_QUEEN = 9819,
	BLACK_ROOK = 9820,
	BLACK_BISHOP = 9821,
	BLACK_KNIGHT = 9822,
	BLACK_PAWN = 9823
};

class piece {
public:
	int colour;
	int type;
	int unicode;
	bool hasMoved;
	bool canTakeInPassing;

public:
	piece() : colour(-1), type(-1), unicode(0020), hasMoved(true), canTakeInPassing(false) {}
	piece(int colour, int type) : colour(colour), type(type), hasMoved(false), canTakeInPassing(false) {
		unicode = ((colour == WHITE) ? 0 : 6) + type + 9812;
	}
	piece(int colour, int type, bool hasMoved) : colour(colour), type(type), hasMoved(hasMoved), canTakeInPassing(false) {
		unicode = ((colour == WHITE) ? 0 : 6) + type + 9812;
	}
	piece(int unicode) : unicode(unicode), hasMoved(true), canTakeInPassing(false) {
		if (unicode >= WHITE_KING && unicode <= WHITE_PAWN) {
			colour = WHITE;
			type = unicode - WHITE_KING;
		}
		else if (unicode >= BLACK_KING && unicode <= BLACK_PAWN) {
			colour = BLACK;
			type = unicode - BLACK_KING;
		}
		else {
			colour = -1;
			type = -1;
		}
	}

	bool empty() {
		return (colour == -1 && type == -1) ? true : false;
	}
};