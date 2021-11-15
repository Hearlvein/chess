#pragma once

#include "Types.hpp"

class Game;


struct Move
{
	static Board& board;

	enum class MoveType
	{
		Simple, Castle, Enpassant, Promotion
	};

	Move(Coords _src, Coords _dest) : src(_src), dest(_dest)
	{
		srcPieceType = board[src.x][src.y].type;
		const auto& ptDest = board[dest.x][dest.y].type;
		if (ptDest != PieceType::Void)
		{
			capturedPieceType = ptDest;
			isCapture = true;
		}

		if (srcPieceType == PieceType::King && std::abs(dest.x - src.x) > 1)
			type = MoveType::Castle;

		else if (srcPieceType == PieceType::Pawn)
		{
			if (ptDest == PieceType::Void && dest.x != src.x)
			{
				type = MoveType::Enpassant;
				isCapture = true;
			}
			else if (dest.y == 0 || dest.y == 7)
				type = MoveType::Promotion;
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
	MoveType type = MoveType::Simple;
};