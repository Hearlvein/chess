#pragma once


#include <SFML/Graphics.hpp>

#include "Types.hpp"


class ResourceManager
{
public:
	ResourceManager()
	{
		m_basePath = "C:/dev/chess/";
		m_boardTexture.loadFromFile(m_basePath + "board.jpg");
		m_piecesTexture.loadFromFile(m_basePath + "pieces.png");
		m_possibleSquareTexture.loadFromFile(m_basePath + "possible.png");
	}

	~ResourceManager() = default;


	const int getScaleFactor() const { return m_scaleFactor; }
	const float getScaleFactorFloat() const { return static_cast<float>(m_scaleFactor); }

	const std::string& getBasePath() const { return m_basePath; }

	const sf::Texture& getBoardTexture() const { return m_boardTexture; }
	const sf::Texture& getPiecesTexture() const { return m_piecesTexture; }
	const sf::Texture& getPossibleSquaresTexture() const { return m_possibleSquareTexture; }

	const sf::Sprite& getBoardSprite() const { return m_boardSp; }
	sf::Sprite& getPieceSprite(PieceType type, PieceColor color)
	{
		int yTextCoord = color == PieceColor::White ? 0 : m_scaleFactor;
		m_genericPieceSp.setTextureRect({ 0 * m_scaleFactor, yTextCoord, m_scaleFactor, m_scaleFactor });
		return m_genericPieceSp;
	}
	sf::Sprite& getPossibleSquareSprite() { return m_possibleSquareSp; }

private:
	const int m_scaleFactor = 100;

	std::string m_basePath;
	sf::Texture m_boardTexture;
	sf::Texture m_piecesTexture;
	sf::Texture m_possibleSquareTexture;

	sf::Sprite m_boardSp;
	sf::Sprite m_genericPieceSp;
	sf::Sprite m_possibleSquareSp;
};