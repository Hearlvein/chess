#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
using namespace sf;

struct Coords
{
	Coords(int _x = 0, int _y = 0) : x(_x), y(_y) {}

	bool operator==(const Coords& other) const
	{
		return x == other.x && y == other.y;
	}

	int x, y;
};

enum class PlayState
{
	Idle, ShowingMoves
};

struct PlayPhase
{
	PlayState playState = PlayState::Idle;
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

struct Piece
{
	Piece(PieceType _type = PieceType::Void, PieceColor _color = PieceColor::White) : type(_type), color(_color) {}

	PieceType type;
	PieceColor color;
};

std::array< std::array<Piece, 8>, 8 > board;

struct Move
{
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

RenderWindow window({ 800,800 }, "Test SFML");
Texture boardTexture;
Texture piecesTexture;
Texture possibleSquareTexture;
Sprite boardSprite;
Sprite genericPiece;
Sprite possibleSquare;
template <typename T>
constexpr T scaleFactor = 100;
PieceColor colorPlaying = PieceColor::White;
PlayPhase playPhase;

std::ostream& operator<<(std::ostream& out, const Coords& coords)
{
	out << '(' << coords.x << ';' << coords.y << ')';
	return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	for (const auto& v : vec)
		out << v << ' ';
	return out;
}

void printChessboard()
{
	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			switch (board[x][y].type)
			{
			case PieceType::King:	std::cout << '#'; break;
			case PieceType::Queen:	std::cout << 'q'; break;
			case PieceType::Bishop:	std::cout << 'b'; break;
			case PieceType::Knight:	std::cout << 'k'; break;
			case PieceType::Tower:	std::cout << 't'; break;
			case PieceType::Pawn:	std::cout << '.'; break;
			default:				std::cout << ' ';
			}
		}
		std::cout << '\n';
	}
}

void setStartPosition()
{
	board[0][7] = board[7][7] = { PieceType::Tower, PieceColor::White };
	board[1][7] = board[6][7] = { PieceType::Knight, PieceColor::White };
	board[2][7] = board[5][7] = { PieceType::Bishop, PieceColor::White };
	board[3][7] = { PieceType::Queen, PieceColor::White };
	board[4][7] = { PieceType::King, PieceColor::White };
	for (size_t i = 0; i < 8; ++i)
		board[i][6] = { PieceType::Pawn, PieceColor::White };

	board[0][0] = board[7][0] = { PieceType::Tower, PieceColor::Black };
	board[1][0] = board[6][0] = { PieceType::Knight, PieceColor::Black };
	board[2][0] = board[5][0] = { PieceType::Bishop, PieceColor::Black };
	board[3][0] = { PieceType::Queen, PieceColor::Black };
	board[4][0] = { PieceType::King, PieceColor::Black };
	for (size_t i = 0; i < 8; ++i)
		board[i][1] = { PieceType::Pawn, PieceColor::Black };
}

void renderPieces()
{
	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			int yTextCoord = (board[x][y].color == PieceColor::White) ? 0 : scaleFactor<int>;
			switch (board[x][y].type)
			{
			case PieceType::King:	genericPiece.setTextureRect({ 0 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			case PieceType::Queen:	genericPiece.setTextureRect({ 1 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			case PieceType::Bishop:	genericPiece.setTextureRect({ 2 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			case PieceType::Knight:	genericPiece.setTextureRect({ 3 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			case PieceType::Tower:	genericPiece.setTextureRect({ 4 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			case PieceType::Pawn:	genericPiece.setTextureRect({ 5 * scaleFactor<int>, yTextCoord, scaleFactor<int>, scaleFactor<int> }); break;
			default:				continue;
			}
			genericPiece.setPosition(x * scaleFactor<float>, y * scaleFactor<float>);
			window.draw(genericPiece);
		}
	}
}

void renderPossibleSquares()
{
	if (playPhase.playState != PlayState::ShowingMoves)
		return;

	for (const auto& coord : playPhase.possibleSquares)
	{
		possibleSquare.setPosition(coord.x * scaleFactor<float>, coord.y * scaleFactor<float>);
		window.draw(possibleSquare);
	}
}

void renderAll()
{
	window.clear();
	window.draw(boardSprite);
	renderPieces();
	renderPossibleSquares();
	window.display();
	// std::cout << "rendering\n";
}

bool areValidCoords(int x, int y)
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

PieceColor opposedColor(PieceColor color)
{
	if (color == PieceColor::White)
		return PieceColor::Black;
	return PieceColor::White;
}

bool colorCanGo(PieceColor color, int xDest, int yDest, bool* isPiece = nullptr, bool notAroundOtherKing = false)
{
	if (areValidCoords(xDest, yDest))
	{
		const auto& p = board[xDest][yDest];

		if (p.type != PieceType::Void)
		{
			if (isPiece)
				*isPiece = true;

			if (p.color == color || p.type == PieceType::King)
				return false;

		}

		if (notAroundOtherKing)
		{
			auto isEnemyKing = [&](int x, int y) { return board[x][y].color == opposedColor(color) && board[x][y].type == PieceType::King; };

			if (	(yDest-1 >= 0 && 				 isEnemyKing(xDest, yDest-1))	||
					(yDest+1 <= 7 && 				 isEnemyKing(xDest, yDest+1))	||
					(xDest+1 <= 7 && 				 isEnemyKing(xDest+1, yDest))	||
					(xDest-1 >= 0 && 				 isEnemyKing(xDest-1, yDest))	||

					(xDest-1 >= 0 && yDest-1 >= 0 && isEnemyKing(xDest-1, yDest-1))	||
					(xDest-1 >= 0 && yDest+1 <= 7 && isEnemyKing(xDest-1, yDest+1))	||
					(xDest+1 <= 7 && yDest-1 >= 0 && isEnemyKing(xDest+1, yDest-1))	||
					(xDest+1 <= 7 && yDest+1 <= 7 && isEnemyKing(xDest+1, yDest+1))
				)
				return false;
		}

		return true;
	}

	return false;
}

std::vector<Coords> getPossibleSquares(const int x, const int y)
{
	std::vector<Coords> possibleSquares;
	const auto& p = board[x][y];

	if		(p.type == PieceType::Pawn)
	{
		if (p.color == PieceColor::White)
		{
			if (y > 0)
			{
				if (areValidCoords(x - 1, y - 1))
				{
					const auto& leftCapture = board[x - 1][y - 1];
					if (x > 0
						&& leftCapture.type != PieceType::Void
						&& leftCapture.type != PieceType::King
						&& leftCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x - 1, y - 1);
				}

				if (areValidCoords(x + 1, y - 1))
				{
					const auto& rightCapture = board[x + 1][y - 1];
					if (x < 7
						&& rightCapture.type != PieceType::Void
						&& rightCapture.type != PieceType::King
						&& rightCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x + 1, y - 1);
				}

				if (board[x][y - 1].type == PieceType::Void)
				{
					possibleSquares.emplace_back(x, y - 1);

					if (y == 6 && board[x][y - 2].type == PieceType::Void)
						possibleSquares.emplace_back(x, y - 2);
				}
			}
		}
	}
	else if (p.type == PieceType::Knight)
	{
		const std::vector<Coords> potentialCoords{
			Coords(x + 1, y - 2), Coords(x + 2, y - 1),
			Coords(x + 2, y + 1), Coords(x + 1, y + 2),
			Coords(x - 1, y + 2), Coords(x - 2, y + 1),
			Coords(x - 2, y - 1), Coords(x - 1, y - 2)
		};
		for (const auto& coord : potentialCoords)
		{
			if (colorCanGo(PieceColor::White, coord.x, coord.y))
				possibleSquares.emplace_back(coord);
		}
	}
	else if (p.type == PieceType::Tower		|| p.type == PieceType::Queen)
	{
		// Left to right
		bool isPiece = false;
		for (int xDest = x + 1; xDest < 8; ++xDest)
		{
			if (colorCanGo(PieceColor::White, xDest, y, &isPiece))
				possibleSquares.emplace_back(xDest, y);

			if (isPiece)
				break;
		}

		// Right to left
		isPiece = false;
		for (int xDest = x - 1; xDest >= 0; --xDest)
		{
			if (colorCanGo(PieceColor::White, xDest, y, &isPiece))
				possibleSquares.emplace_back(xDest, y);

			if (isPiece)
				break;
		}

		// Up to down
		isPiece = false;
		for (int yDest = y + 1; yDest < 8; ++yDest)
		{
			if (colorCanGo(PieceColor::White, x, yDest, &isPiece))
				possibleSquares.emplace_back(x, yDest);

			if (isPiece)
				break;
		}

		// Down to up
		isPiece = false;
		for (int yDest = y - 1; yDest >= 0; --yDest)
		{
			if (colorCanGo(PieceColor::White, x, yDest, &isPiece))
				possibleSquares.emplace_back(x, yDest);

			if (isPiece)
				break;
		}
	}
	if		(p.type == PieceType::Bishop	|| p.type == PieceType::Queen)
	{
		// Bottom left
		bool isPiece = false;
		int xDest, yDest;
		for (xDest = x + 1, yDest = y + 1; xDest < 8 && yDest < 8; ++xDest, ++yDest)
		{
			if (colorCanGo(PieceColor::White, xDest, yDest, &isPiece))
				possibleSquares.emplace_back(xDest, yDest);

			if (isPiece)
				break;
		}

		// Top left
		isPiece = false;
		for (xDest = x + 1, yDest = y - 1; xDest < 8 && yDest >= 0; ++xDest, --yDest)
		{
			if (colorCanGo(PieceColor::White, xDest, yDest, &isPiece))
				possibleSquares.emplace_back(xDest, yDest);

			if (isPiece)
				break;
		}

		// Top right
		isPiece = false;
		for (xDest = x - 1, yDest = y - 1; xDest >= 0 && yDest >= 0; --xDest, --yDest)
		{
			if (colorCanGo(PieceColor::White, xDest, yDest, &isPiece))
				possibleSquares.emplace_back(xDest, yDest);

			if (isPiece)
				break;
		}

		// Bottom right
		isPiece = false;
		for (xDest = x - 1, yDest = y + 1; xDest >= 0 && yDest < 8; --xDest, ++yDest)
		{
			if (colorCanGo(PieceColor::White, xDest, yDest, &isPiece))
				possibleSquares.emplace_back(xDest, yDest);

			if (isPiece)
				break;
		}
	}
	else if (p.type == PieceType::King)
	{
		const std::vector<Coords> potentialCoords{
			Coords(x - 1, y - 1),	Coords(x, y - 1),
			Coords(x - 1, y + 1),	Coords(x, y + 1),
			Coords(x + 1, y - 1),	Coords(x - 1, y),
			Coords(x + 1, y + 1),	Coords(x + 1, y)
		};
		for (const auto& coord : potentialCoords)
		{
			if (colorCanGo(PieceColor::White, coord.x, coord.y, nullptr, true))
				possibleSquares.emplace_back(coord);
		}
	}

	return possibleSquares;
}

bool applyMove(int xDest, int yDest)
{
	// No valid destination for a move
	if (std::find(playPhase.possibleSquares.begin(), playPhase.possibleSquares.end(), Coords(xDest, yDest)) == playPhase.possibleSquares.end())
		return false;

	auto& clickedPiece = board[playPhase.clickedSquare.x][playPhase.clickedSquare.y];

	Move currentMove(Coords(playPhase.clickedSquare.x, playPhase.clickedSquare.y), Coords(xDest, yDest));
	std::cout << currentMove.notation() << std::endl;
	
	// White pawn promotion
	if (clickedPiece.type == PieceType::Pawn
		&& clickedPiece.color == PieceColor::White
		&& yDest == 0)
		board[xDest][yDest] = Piece(PieceType::Queen, PieceColor::White);
	else
		board[xDest][yDest] = clickedPiece;
	
	clickedPiece = Piece(PieceType::Void, PieceColor::White);

	return true;
}

int main()
{
	window.setVerticalSyncEnabled(true);

	const std::string resPath = "C:/dev/chess/";
	boardTexture.loadFromFile(resPath + "board.jpg");
	piecesTexture.loadFromFile(resPath + "pieces.png");
	possibleSquareTexture.loadFromFile(resPath + "possible.png");


	boardSprite.setTexture(boardTexture);
	genericPiece.setTexture(piecesTexture);
	possibleSquare.setTexture(possibleSquareTexture);


	setStartPosition();
	renderAll();

	Event event;
	while (window.waitEvent(event) && window.isOpen())
	{
		if (event.type == Event::Closed)
			window.close();

		else if (event.type == Event::MouseButtonPressed)
		{
			int x = event.mouseButton.x / scaleFactor<int>;
			int y = event.mouseButton.y / scaleFactor<int>;
			
			if (playPhase.playState == PlayState::Idle)
			{
				playPhase.clickedSquare = Coords(x, y);
				playPhase.possibleSquares = getPossibleSquares(x, y);

				// std::cout << playPhase.possibleSquares << std::endl;

				playPhase.playState = PlayState::ShowingMoves;
			}
			else if (playPhase.playState == PlayState::ShowingMoves)
			{
				applyMove(x, y);
				playPhase.playState = PlayState::Idle;
			}
		}

		else
			continue;

		renderAll();
	}

	return 0;
}