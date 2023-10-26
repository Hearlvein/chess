#pragma once

#include "Move.hpp"


struct Coords
{
	Coords(int _x = 0, int _y = 0) : x(_x), y(_y) {}

	bool operator==(const Coords& other) const
	{
		return x == other.x && y == other.y;
	}

	static bool areValidCoords(int _x, int _y)
	{
		return _x >= 0 && _x < 8 && _y >= 0 && _y < 8;
	}

	int x, y;
};

enum class Turn
{
	Idle, ShowingMoves
};

struct TurnInfo
{
	Turn state = Turn::Idle;
	Coords clickedSquare;
	std::vector<Move> possibleMoves;
};

enum class PieceType
{
	King, Queen, Tower, Bishop, Knight, Pawn, Void
};

enum class PieceColor
{
	White, Black
};

struct Piece
{
	Piece(PieceType _type = PieceType::Void, PieceColor _color = PieceColor::White) : type(_type), color(_color) {}

	PieceType type;
	PieceColor color;
};
