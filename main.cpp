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

RenderWindow window({ 800,800 }, "Test SFML");
Texture boardTexture;
Texture piecesTexture;
Texture possibleSquareTexture;
Sprite board;
Sprite genericPiece;
Sprite possibleSquare;
std::array< std::array<Piece, 8>, 8 > chessboard;
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
			switch (chessboard[x][y].type)
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
	chessboard[0][7] = chessboard[7][7] = { PieceType::Tower, PieceColor::White };
	chessboard[1][7] = chessboard[6][7] = { PieceType::Knight, PieceColor::White };
	chessboard[2][7] = chessboard[5][7] = { PieceType::Bishop, PieceColor::White };
	chessboard[3][7] = { PieceType::Queen, PieceColor::White };
	chessboard[4][7] = { PieceType::King, PieceColor::White };
	for (size_t i = 0; i < 8; ++i)
		chessboard[i][6] = { PieceType::Pawn, PieceColor::White };

	chessboard[0][0] = chessboard[7][0] = { PieceType::Tower, PieceColor::Black };
	chessboard[1][0] = chessboard[6][0] = { PieceType::Knight, PieceColor::Black };
	chessboard[2][0] = chessboard[5][0] = { PieceType::Bishop, PieceColor::Black };
	chessboard[3][0] = { PieceType::Queen, PieceColor::Black };
	chessboard[4][0] = { PieceType::King, PieceColor::Black };
	for (size_t i = 0; i < 8; ++i)
		chessboard[i][1] = { PieceType::Pawn, PieceColor::Black };
}

void renderPieces()
{
	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			int yTextCoord = (chessboard[x][y].color == PieceColor::White) ? 0 : scaleFactor<int>;
			switch (chessboard[x][y].type)
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
	window.draw(board);
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
		const auto& p = chessboard[xDest][yDest];

		if (p.type != PieceType::Void)
		{
			if (isPiece)
				*isPiece = true;

			if (p.color == color || p.type == PieceType::King)
				return false;

		}

		if (notAroundOtherKing)
		{
			auto aroundEnemyKing = [&](int x, int y) { return chessboard[x][y].color == opposedColor(color) && chessboard[x][y].type == PieceType::King; };

			if (	(yDest-1 >= 0 && 				 aroundEnemyKing(xDest, yDest-1))	||
					(yDest+1 <= 7 && 				 aroundEnemyKing(xDest, yDest+1))	||
					(xDest+1 <= 7 && 				 aroundEnemyKing(xDest+1, yDest))	||
					(xDest-1 >= 0 && 				 aroundEnemyKing(xDest-1, yDest))	||

					(xDest-1 >= 0 && yDest-1 >= 0 && aroundEnemyKing(xDest-1, yDest-1))	||
					(xDest-1 >= 0 && yDest+1 <= 7 && aroundEnemyKing(xDest-1, yDest+1))	||
					(xDest+1 <= 7 && yDest-1 >= 0 && aroundEnemyKing(xDest+1, yDest-1))	||
					(xDest+1 <= 7 && yDest+1 <= 7 && aroundEnemyKing(xDest+1, yDest+1))
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
	const auto& p = chessboard[x][y];

	if		(p.type == PieceType::Pawn)
	{
		if (p.color == PieceColor::White)
		{
			if (y > 0)
			{
				if (areValidCoords(x - 1, y - 1))
				{
					const auto& leftCapture = chessboard[x - 1][y - 1];
					if (x > 0
						&& leftCapture.type != PieceType::Void
						&& leftCapture.type != PieceType::King
						&& leftCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x - 1, y - 1);
				}

				if (areValidCoords(x + 1, y - 1))
				{
					const auto& rightCapture = chessboard[x + 1][y - 1];
					if (x < 7
						&& rightCapture.type != PieceType::Void
						&& rightCapture.type != PieceType::King
						&& rightCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x + 1, y - 1);
				}

				if (chessboard[x][y - 1].type == PieceType::Void)
				{
					possibleSquares.emplace_back(x, y - 1);

					if (y == 6 && chessboard[x][y - 2].type == PieceType::Void)
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

	auto& clickedPiece = chessboard[playPhase.clickedSquare.x][playPhase.clickedSquare.y];
	 
	// White pawn promotion
	if (clickedPiece.type == PieceType::Pawn
		&& clickedPiece.color == PieceColor::White
		&& yDest == 0)
		chessboard[xDest][yDest] = Piece(PieceType::Queen, PieceColor::White);
	else
		chessboard[xDest][yDest] = clickedPiece;
	
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


	board.setTexture(boardTexture);
	genericPiece.setTexture(piecesTexture);
	possibleSquare.setTexture(possibleSquareTexture);


	setStartPosition();
	renderAll();

	Event event;
	while (window.isOpen() && window.waitEvent(event))
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

				std::cout << playPhase.possibleSquares << std::endl;

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