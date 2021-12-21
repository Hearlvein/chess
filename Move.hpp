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

	Move(Coords _src, Coords _dest, Board& board) : src(_src), dest(_dest)
	{
		srcPieceType = board[src.x][src.y].type;
		const auto& ptDest = board[dest.x][dest.y].type;
		if (ptDest != PieceType::Void)
		{
			capturedPieceType = ptDest;
			isCapture = true;
		}

		if (srcPieceType == PieceType::King && std::abs(dest.x - src.x) > 1)
			type = Type::Castle;

		else if (srcPieceType == PieceType::Pawn)
		{
			if (ptDest == PieceType::Void && dest.x != src.x)
			{
				type = Move::Type::Enpassant;
				isCapture = true;
			}
			else if (dest.y == 0 || dest.y == 7)
				type = Move::Type::Promotion;
		}
	}

	std::string notation() const
	{
		std::string out;

		switch (srcPieceType)
		{
		case PieceType::King: 		out += '#'; break;
		case PieceType::Queen: 		out += 'Q'; break;
		case PieceType::Tower: 		out += 'T'; break;
		case PieceType::Bishop: 	out += 'B'; break;
		case PieceType::Knight: 	out += 'K'; break;
		default: break;
		}

		if (isCapture) out += 'x';

		out += ((char)(dest.x) + 'a');
		out += std::to_string(8 - dest.y);

		return out;
	}

	bool isCapture = false;
	PieceType srcPieceType;
	PieceType capturedPieceType = PieceType::Void;
	Coords dest;
	Coords src;
	Type type = Type::Simple;
};
