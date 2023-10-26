#include "Move.hpp"


Move::Move(Type _type, Coords _src, Coords _dest, Board& board) : isCapture(false), type(_type), src(_src), dest(_dest)
{
	srcPieceType = board[src.x][src.y].type;
	const auto& ptDest = board[dest.x][dest.y].type;

	// Simple Capture
	if (ptDest != PieceType::Void)
	{
		capturedPieceType = ptDest;
		capturePosition = src;
		isCapture = true;
	}
	// Special pawn moves
	else if (srcPieceType == PieceType::Pawn)
	{
		// En-passant
		if (type == Type::Enpassant)
		{
			// Black plays?
			if (dest.y > src.y)
				capturePosition = Coords(src.x, dest.y - 1);
			else
				capturePosition = Coords(src.x, dest.y + 1);

			capturedPieceType = PieceType::Pawn;
			isCapture = true;

		}
		// Promotion
		else if (dest.y == 0 || dest.y == 7)
			type = Type::Promotion;
	}
}

Move::Move(Coords _src, Coords _dest, Board& board) : isCapture(false), type(Type::Simple), src(_src), dest(_dest)
{
	srcPieceType = board[src.x][src.y].type;
	const auto& ptDest = board[dest.x][dest.y].type;

	// Simple Capture
	if (ptDest != PieceType::Void)
	{
		capturedPieceType = ptDest;
		capturePosition = src;
		isCapture = true;
	}
	// Special pawn moves
	else if (srcPieceType == PieceType::Pawn)
	{
		// En-passant
		if (type == Type::Enpassant)
		{
			// Black plays?
			if (dest.y > src.y)
				capturePosition = Coords(src.x, dest.y - 1);
			else
				capturePosition = Coords(src.x, dest.y + 1);

			capturedPieceType = PieceType::Pawn;
			isCapture = true;

		}
		// Promotion
		else if (dest.y == 0 || dest.y == 7)
			type = Type::Promotion;
	}

	// Castle
	if (srcPieceType == PieceType::King && std::abs(dest.x - src.x) > 1)
		type = Type::Castle;

	// En passant and promotion
	else if (srcPieceType == PieceType::Pawn)
	{
		if (ptDest == PieceType::Void && dest.x != src.x)
			type = Move::Type::Enpassant;

		else if (dest.y == 0 || dest.y == 7)
			type = Move::Type::Promotion;
	}
}

std::string Move::notation() const
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