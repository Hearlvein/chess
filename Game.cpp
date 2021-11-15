#include "Game.hpp"


Game::Game()
{
	m_window.create(sf::VideoMode(800, 800), "James plays Chess!");
	m_window.setVerticalSyncEnabled(true);

	Move::board = m_board;
}