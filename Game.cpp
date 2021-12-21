#include "Game.hpp"


Game::Game()
{
	m_window.create(sf::VideoMode(800, 800), "James plays Chess!");
	m_window.setVerticalSyncEnabled(true);
}

void Game::run()
{
	setStartPosition();
	renderAll();

	sf::Event event;
	while (m_window.waitEvent(event) && m_window.isOpen())
	{
		if (event.type == sf::Event::Closed)
			m_window.close();

		else if (event.type == sf::Event::MouseButtonPressed)
		{
			int x = event.mouseButton.x / m_resMgr.getScaleFactor();
			int y = event.mouseButton.y / m_resMgr.getScaleFactor();

			if (m_turnInfo.state == Turn::Idle)
			{
				m_turnInfo.clickedSquare = Coords(x, y);
				m_turnInfo.possibleSquares = getPossibleSquares(x, y);

				// std::cout << currentTurn.possibleSquares << std::endl;

				m_turnInfo.state = Turn::ShowingMoves;
			}
			else if (m_turnInfo.state == Turn::ShowingMoves)
			{
				applyMove(x, y);
				m_turnInfo.state = Turn::Idle;
			}
		}

		else
			continue;

		renderAll();
	}
}

void Game::printChessm_board()
{
	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			switch (m_board[x][y].type)
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

void Game::setStartPosition()
{
	m_board[0][7] = m_board[7][7] = { PieceType::Tower, PieceColor::White };
	m_board[1][7] = m_board[6][7] = { PieceType::Knight, PieceColor::White };
	m_board[2][7] = m_board[5][7] = { PieceType::Bishop, PieceColor::White };
	m_board[3][7] = { PieceType::Queen, PieceColor::White };
	m_board[4][7] = { PieceType::King, PieceColor::White };
	for (size_t i = 0; i < 8; ++i)
		m_board[i][6] = { PieceType::Pawn, PieceColor::White };

	m_board[0][0] = m_board[7][0] = { PieceType::Tower, PieceColor::Black };
	m_board[1][0] = m_board[6][0] = { PieceType::Knight, PieceColor::Black };
	m_board[2][0] = m_board[5][0] = { PieceType::Bishop, PieceColor::Black };
	m_board[3][0] = { PieceType::Queen, PieceColor::Black };
	m_board[4][0] = { PieceType::King, PieceColor::Black };
	for (size_t i = 0; i < 8; ++i)
		m_board[i][1] = { PieceType::Pawn, PieceColor::Black };
}

void Game::renderAll()
{
	m_window.clear();
	m_window.draw(m_resMgr.getBoardSprite());
	renderPieces();
	renderPossibleSquares();
	m_window.display();
	// std::cout << "rendering\n";
}

void Game::renderPieces()
{
	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			if (auto& sq = m_board[x][y]; sq.type != PieceType::Void)
			{
				auto& genericPiece = m_resMgr.getPieceSprite(sq.type, sq.color);
				float factor = m_resMgr.getScaleFactorFloat();
				genericPiece.setPosition(x * factor, y * factor);
				m_window.draw(genericPiece);
			}
		}
	}
}

void Game::renderPossibleSquares()
{
	if (m_turnInfo.state != Turn::ShowingMoves)
		return;

	for (const auto& coord : m_turnInfo.possibleSquares)
	{
		auto& possibleSquare = m_resMgr.getPossibleSquareSprite();
		float factor = m_resMgr.getScaleFactorFloat();
		possibleSquare.setPosition(coord.x * factor, coord.y * factor);
		m_window.draw(possibleSquare);
	}
}

PieceColor Game::opposedColor(PieceColor color) const
{
	return (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool Game::colorCanGo(PieceColor color, int xDest, int yDest, bool* isPiece, bool notAroundOtherKing)
{
	if (Coords::areValidCoords(xDest, yDest))
	{
		const auto& p = m_board[xDest][yDest];

		if (p.type != PieceType::Void)
		{
			if (isPiece)
				*isPiece = true;

			if (p.color == color || p.type == PieceType::King)
				return false;

		}

		if (notAroundOtherKing)
		{
			auto isEnemyKing = [&](int x, int y) { return m_board[x][y].color == opposedColor(color) && m_board[x][y].type == PieceType::King; };

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

std::vector<Coords> Game::getPossibleSquares(const int x, const int y)
{
	std::vector<Coords> possibleSquares;
	const auto& p = m_board[x][y];

	if		(p.type == PieceType::Pawn)
	{
		if (p.color == PieceColor::White)
		{
			if (y > 0)
			{
				if (Coords::areValidCoords(x - 1, y - 1))
				{
					const auto& leftCapture = m_board[x - 1][y - 1];
					if (x > 0
						&& leftCapture.type != PieceType::Void
						&& leftCapture.type != PieceType::King
						&& leftCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x - 1, y - 1);
				}

				if (Coords::areValidCoords(x + 1, y - 1))
				{
					const auto& rightCapture = m_board[x + 1][y - 1];
					if (x < 7
						&& rightCapture.type != PieceType::Void
						&& rightCapture.type != PieceType::King
						&& rightCapture.color == PieceColor::Black)
						possibleSquares.emplace_back(x + 1, y - 1);
				}

				if (m_board[x][y - 1].type == PieceType::Void)
				{
					possibleSquares.emplace_back(x, y - 1);

					if (y == 6 && m_board[x][y - 2].type == PieceType::Void)
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

bool Game::applyMove(int xDest, int yDest)
{
	// No valid destination for a move
	if (std::find(m_turnInfo.possibleSquares.begin(), m_turnInfo.possibleSquares.end(), Coords(xDest, yDest)) == m_turnInfo.possibleSquares.end())
		return false;

	auto& clickedPiece = m_board[m_turnInfo.clickedSquare.x][m_turnInfo.clickedSquare.y];

	Move currentMove(Coords(m_turnInfo.clickedSquare.x, m_turnInfo.clickedSquare.y), Coords(xDest, yDest), m_board);
	std::cout << currentMove.notation() << std::endl;
	
	// White pawn promotion
	if (clickedPiece.type == PieceType::Pawn
		&& clickedPiece.color == PieceColor::White
		&& yDest == 0)
		m_board[xDest][yDest] = Piece(PieceType::Queen, PieceColor::White);
	else
		m_board[xDest][yDest] = clickedPiece;
	
	clickedPiece = Piece(PieceType::Void, PieceColor::White);

	return true;
}