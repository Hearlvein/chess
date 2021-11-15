#pragma once

#include <array>


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
	std::vector<Coords> possibleSquares;
};

enum class PieceType
{
	King, Queen, Bishop, Knight, Tower, Pawn, Void
};

enum class PieceColor
{
	White, Black
};

/*
static PieceColor opposedColor(PieceColor color)
{
	if (color == PieceColor::White)
		return PieceColor::Black;
	return PieceColor::White;
}*/

struct Piece
{
	Piece(PieceType _type = PieceType::Void, PieceColor _color = PieceColor::White) : type(_type), color(_color) {}

	PieceType type;
	PieceColor color;
};


typedef std::array< std::array<Piece, 8>, 8 > Board;