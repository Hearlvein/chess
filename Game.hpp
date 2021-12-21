#pragma once

#include <array>
#include <SFML/Graphics.hpp>

#include "ResourceManager.hpp"
#include "Types.hpp"
#include "utility.hpp"
#include "Move.hpp"


typedef std::array< std::array<Piece, 8>, 8 > Board;


class Game
{
public:
	Game();

	~Game() = default;

	void run();
	
	void printChessm_board();

	void setStartPosition();

	void renderAll();

	void renderPieces();

	void renderPossibleSquares();

	PieceColor opposedColor(PieceColor color) const;

	bool colorCanGo(PieceColor color, int xDest, int yDest, bool* isPiece = nullptr, bool notAroundOtherKing = false);

	std::vector<Coords> getPossibleSquares(const int x, const int y);

	bool applyMove(int xDest, int yDest);


private:
	sf::RenderWindow m_window;
	ResourceManager m_resMgr;
	PieceColor colorPlaying = PieceColor::White;
	TurnInfo m_turnInfo;
	Board m_board;
};