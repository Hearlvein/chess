#include <fstream>

#include "Game.hpp"


Game::Game()
{
	m_window.create(sf::VideoMode(800, 800), "James plays Chess!");
	m_window.setVerticalSyncEnabled(true);
}

Game::~Game()
{
	std::ofstream st(m_resMgr.getBasePath() + "moves_list.txt");
	printMoves(st);
	st.close();
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

			// Idle -> ShowingMoves
			if (m_turnInfo.state == Turn::Idle && m_board[x][y].color == m_colorPlaying)
			{
				m_turnInfo.clickedSquare = Coords(x, y);
				m_turnInfo.possibleMoves = getPossibleMoves(x, y);
				// std::cout << m_turnInfo.possibleMoves << std::endl;
				m_turnInfo.state = Turn::ShowingMoves;
			}
			// ShowingMoves -> Idle (backwards, may not play the previously clicked piece)
			else if (m_turnInfo.state == Turn::ShowingMoves && Coords(x, y) == m_turnInfo.clickedSquare)
			{
				m_turnInfo.possibleMoves.clear();
				m_turnInfo.state = Turn::Idle;
			}
			// ShowingMoves -> opponent Idle
			else if (m_turnInfo.state == Turn::ShowingMoves)
			{
				if (applyMove(x, y))
				{
					m_colorPlaying = opposedColor(m_colorPlaying);
					m_turnInfo.state = Turn::Idle;
				}
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

std::ostream& Game::printMoves(std::ostream& out) const
{
	for (size_t i = 0; i < m_moves.size(); ++i)
	{
		out << i+1 << ". " << m_moves[i].notation() << std::endl;
	}

	return out;
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

	for (const Move& move : m_turnInfo.possibleMoves)
	{
		auto& possibleSquare = m_resMgr.getPossibleSquareSprite();
		float factor = m_resMgr.getScaleFactorFloat();
		possibleSquare.setPosition(move.src.x * factor, move.src.y * factor);
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

std::vector<Move> Game::getPossibleMoves(const int x, const int y)
{
	std::vector<Move> possibleMoves;
	const auto& p = m_board[x][y];

	if		(p.type == PieceType::Pawn)
	{
		if		(p.color == PieceColor::White)
		{
			if (y > 0)
			{
				if (Coords::areValidCoords(x - 1, y - 1))
				{
					const auto& leftCapture = m_board[x - 1][y - 1];
					if (leftCapture.type != PieceType::Void &&
						leftCapture.type != PieceType::King &&
						leftCapture.color == PieceColor::Black
						) possibleMoves.emplace_back(Move::Type::Simple, x, y, x - 1, y - 1, m_board);
				}

				if (Coords::areValidCoords(x + 1, y - 1))
				{
					const auto& rightCapture = m_board[x + 1][y - 1];
					if (rightCapture.type != PieceType::Void &&
						rightCapture.type != PieceType::King &&
						rightCapture.color == PieceColor::Black
						) possibleMoves.emplace_back(Move::Type::Simple, x, y, x + 1, y - 1, m_board);
				}

				if (m_board[x][y - 1].type == PieceType::Void)
				{
					possibleMoves.emplace_back(Move::Type::Simple, x, y, x, y - 1, m_board);

					if (y == 6 && m_board[x][y - 2].type == PieceType::Void)
						possibleMoves.emplace_back(Move::Type::Simple, x, y, x, y - 2, m_board);
				}
			}
		}
		else if (p.color == PieceColor::Black)
		{
			if (y < 7)
			{
				if (Coords::areValidCoords(x - 1, y + 1))
				{
					const auto& leftCapture = m_board[x - 1][y + 1];
					if (leftCapture.type !=  PieceType::Void &&
						leftCapture.type !=  PieceType::King &&
						leftCapture.color == opposedColor(m_colorPlaying)
						) possibleMoves.emplace_back(Move::Type::Simple, x, y, x - 1, y + 1, m_board);
				}

				if (Coords::areValidCoords(x + 1, y + 1))
				{
					const auto& rightCapture = m_board[x + 1][y + 1];
					if (rightCapture.type !=  PieceType::Void &&
						rightCapture.type !=  PieceType::King &&
						rightCapture.color == opposedColor(m_colorPlaying)
						) possibleMoves.emplace_back(Move::Type::Simple, x, y, x + 1, y + 1, m_board);
				}

				if (m_board[x][y + 1].type == PieceType::Void)
				{
					possibleMoves.emplace_back(Move::Type::Simple, x, y, x, y + 1, m_board);

					if (y == 1 && m_board[x][y + 2].type == PieceType::Void)
						possibleMoves.emplace_back(Move::Type::Simple, x, y, x, y + 2, m_board);
				}

				// En-passant
				if (y == 4)
				{
					const Move& lm = m_moves.back();

					// Left capture
					if (x > 0 &&
						lm.src.y == 6 &&
						lm.dest.y == 4 &&
						(m_board[x - 1][6].type == PieceType::Void || m_board[x - 1][6].color == PieceColor::White)
						) possibleMoves.emplace_back(Move::Type::Enpassant, x, y, Coords(x - 1, 5), m_board);

					// Right capture
					if (x < 7 &&
						lm.src.y == 6 &&
						lm.dest.y == 4 &&
						(m_board[x + 1][6].type == PieceType::Void || m_board[x + 1][6].color == PieceColor::White)
						) possibleMoves.emplace_back(Move::Type::Enpassant, x, y, Coords(x + 1, 5), m_board);
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
			if (colorCanGo(m_colorPlaying, coord.x, coord.y))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, coord, m_board);
		}
	}
	else if (p.type == PieceType::Tower		|| p.type == PieceType::Queen)
	{
		// Left to right
		bool isPiece = false;
		for (int xDest = x + 1; xDest < 8; ++xDest)
		{
			if (colorCanGo(m_colorPlaying, xDest, y, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, y, m_board);

			if (isPiece)
				break;
		}

		// Right to left
		isPiece = false;
		for (int xDest = x - 1; xDest >= 0; --xDest)
		{
			if (colorCanGo(m_colorPlaying, xDest, y, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, y, m_board);

			if (isPiece)
				break;
		}

		// Up to down
		isPiece = false;
		for (int yDest = y + 1; yDest < 8; ++yDest)
		{
			if (colorCanGo(m_colorPlaying, x, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, x, yDest, m_board);

			if (isPiece)
				break;
		}

		// Down to up
		isPiece = false;
		for (int yDest = y - 1; yDest >= 0; --yDest)
		{
			if (colorCanGo(m_colorPlaying, x, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, x, yDest, m_board);

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
			if (colorCanGo(m_colorPlaying, xDest, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, yDest, m_board);

			if (isPiece)
				break;
		}

		// Top left
		isPiece = false;
		for (xDest = x + 1, yDest = y - 1; xDest < 8 && yDest >= 0; ++xDest, --yDest)
		{
			if (colorCanGo(m_colorPlaying, xDest, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, yDest, m_board);

			if (isPiece)
				break;
		}

		// Top right
		isPiece = false;
		for (xDest = x - 1, yDest = y - 1; xDest >= 0 && yDest >= 0; --xDest, --yDest)
		{
			if (colorCanGo(m_colorPlaying, xDest, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, yDest, m_board);

			if (isPiece)
				break;
		}

		// Bottom right
		isPiece = false;
		for (xDest = x - 1, yDest = y + 1; xDest >= 0 && yDest < 8; --xDest, ++yDest)
		{
			if (colorCanGo(m_colorPlaying, xDest, yDest, &isPiece))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, xDest, yDest, m_board);

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
			if (colorCanGo(m_colorPlaying, coord.x, coord.y, nullptr, true))
				possibleMoves.emplace_back(Move::Type::Simple, x, y, coord, m_board);
		}
	}

	return possibleMoves;
}

bool Game::applyMove(int xDest, int yDest)
{
	const Move* currentMove = nullptr;
	for (auto it = m_turnInfo.possibleMoves.cbegin(); it != m_turnInfo.possibleMoves.cend(); ++it)
	{
		if (it->dest.x == xDest && it->dest.y == yDest)
		{
			// Should only be one ocurrence
			currentMove = &(*it);
			break;
		}
	}

	if (!currentMove)
	{
		std::cout << "No valid destination for the move!" << std::endl;
		return false;
	}

	auto& clickedPiece = m_board[m_turnInfo.clickedSquare.x][m_turnInfo.clickedSquare.y];

	Move currentMove(Coords(m_turnInfo.clickedSquare.x, m_turnInfo.clickedSquare.y), Coords(xDest, yDest), m_board);
	std::cout << currentMove->notation() << std::endl;
	m_moves.emplace_back(currentMove);
	
	// White pawn promotion
	if (currentMove->type == Move::Type::Promotion)
		m_board[xDest][yDest] = Piece(PieceType::Queen, m_colorPlaying);
	else
	{
		if (currentMove->type == Move::Type::Enpassant)
		{
			const Coords& cp = currentMove->capturePosition;
			m_board[cp.x][cp.y] = Piece(PieceType::Void, PieceColor::White);
		}

		// Finally move the piece to the clicked square
		m_board[xDest][yDest] = clickedPiece;
	}
	
	clickedPiece = Piece(PieceType::Void, PieceColor::White);

	return true;
}