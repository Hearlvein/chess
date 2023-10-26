#pragma once


#include <string>
#include <array>
#include "Types.hpp"

typedef std::array< std::array<Piece, 8>, 8 > Board;


struct Move
{
	enum class Type
	{
		Simple, Castle, Enpassant, Promotion
	};

	Move(Type _type, Coords _src, Coords _dest, Board& board);

	Move(Coords _src, Coords _dest, Board& board);

	std::string notation() const;

	bool isCapture;
	PieceType srcPieceType;
	PieceType capturedPieceType = PieceType::Void;
	Coords capturePosition;
	Coords dest;
	Coords src;
	Type type;
};
