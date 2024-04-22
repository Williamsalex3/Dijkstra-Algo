#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Graphe.h"

enum class Direction
{
	Nord,
	Sud,
	Est,
	Ouest,
};

enum class EtatJeu
{
	Gagne,
	Perdu,
	EnCours,
};

struct Position
{
	Position() : x(0), y(0) {}
	Position(int x, int y) : x(x), y(y) {}
	void operator+=(const Position& that)
	{
		this->x += that.x;
		this->y += that.y;
	}
	bool operator==(const Position& that)
	{
		return this->x == that.x && this->y == that.y;
	}
	bool operator!=(const Position& that)
	{
		return !(*this == that);
	}
	int x;
	int y;
};

class Grille
{
public:

	static Grille CreerDuNiveau(const std::vector<std::string>& niveau);

	Grille(
		unsigned int largeur, 
		unsigned int hauteur,
		const Position& positionJoueur,
		const Position& positionEnnemi,
		const Position& positionPorte,
		const std::vector<Position>& positionsMurs,
		const std::vector<Position>& positionsHerbe
	);

	void dessiner(sf::RenderWindow& window);

	void bougerJoueur(Direction direction);

	void bougerEnnemi();

	unsigned int obtenirLargeurPx() { return this->largeur * LargeurTuilePx; }

	unsigned int obtenirHauteurPx() { return this->hauteur * HauteurTuilePx; }

private:

	sf::Texture texturePlancher;
	sf::Sprite spritePlancher;

	sf::Texture texturePorte;
	sf::Sprite spritePorte;
	const Position positionPorte;

	sf::Texture textureHerbe;
	std::vector<sf::Sprite> spritesHerbe;

	sf::Texture textureMur;
	std::vector<sf::Sprite> spritesMur;

	sf::Texture textureJoueur;
	sf::Texture textureJoueurPerdu;
	sf::Sprite spriteJoueur;
	Position positionJoueur;

	sf::Texture textureEnnemi;
	sf::Texture textureEnnemiPerdu;
	sf::Sprite spriteEnnemi;
	Position positionEnnemi;

	sf::Texture texturesCheminEnnemi[4];
	std::list<sf::Sprite> spritesCheminEnnemi;
	Chemin cheminEnnemi;

	unsigned int largeur;
	unsigned int hauteur;

	std::vector<std::vector<bool>> casesMarchables;

	sf::Clock horloge;

	static sf::Vector2f ConvertirPositionPixels(const Position& position);

	Position cleAPosition(unsigned int cle) const { return Position(cle % this->largeur, cle / this->largeur); }

	unsigned int positionACle(const Position& position) const { return position.y * this->largeur + position.x; }

	std::list<Position> positionsAdjacentes(const Position& position) const;

	Direction obtenirDirection(unsigned int cleSource, unsigned int cleDest) const;

	void mettreAJourCheminEnnemi();

	Graphe graphe;

	static const unsigned int LargeurTuilePx = 32;
	static const unsigned int HauteurTuilePx = 32;

	sf::Time dernierDeplacementJoueur;
	static const sf::Int32 DelaiDeplacementJoueurMs = 80;

	sf::Time dernierDeplacementEnnemi;
	static const sf::Int32 DelaiDeplacementEnnemiMs = 350;

	EtatJeu etatJeu = EtatJeu::EnCours;
};
