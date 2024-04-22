#include "Grille.h"

Grille Grille::CreerDuNiveau(const std::vector<std::string>& niveau)
{
    unsigned int hauteur = (unsigned int)niveau.size();

    unsigned int largeur = (unsigned int)niveau[0].size();

    Position positionJoueur;
    Position positionEnnemi;
    Position positionPorte;
    std::vector<Position> positionsMurs;
    std::vector<Position> positionsHerbe;

    for (unsigned int x = 0; x < largeur; x++)
    {
        for (unsigned int y = 0; y < hauteur; y++)
        {
            Position position(x, y);

            const char cell = niveau[y][x];

            switch (cell)
            {
            case 'P': positionJoueur = position; break;
            case 'E': positionEnnemi = position; break;
            case 'D': positionPorte = position; break;
            case 'W': positionsMurs.push_back(position); break;
            case 'G': positionsHerbe.push_back(position); break;
            }
        }
    }

    return Grille(largeur, hauteur, positionJoueur, positionEnnemi, positionPorte, positionsMurs, positionsHerbe);
}

Grille::Grille(
    unsigned int largeur,
    unsigned int hauteur,
    const Position& positionJoueur,
    const Position& positionEnnemi,
    const Position& positionPorte,
    const std::vector<Position>& positionsMurs,
    const std::vector<Position>& positionsHerbe
) : largeur(largeur), 
    hauteur(hauteur),
    positionJoueur(positionJoueur),
    positionEnnemi(positionEnnemi),
    positionPorte(positionPorte)
{
    srand((unsigned int)time(nullptr));

    // plancher
    this->texturePlancher.loadFromFile("assets/mesh_0_new.png");
    this->texturePlancher.setRepeated(true);
    this->spritePlancher.setTexture(this->texturePlancher);
    this->spritePlancher.setTextureRect(sf::IntRect(0, 0, this->obtenirLargeurPx(), this->obtenirHauteurPx()));

    // murs
    
    this->casesMarchables.resize(this->largeur);

    for (int i = 0; i < this->casesMarchables.size(); i++)
    {
        this->casesMarchables[i].resize(this->hauteur);

        for (int j = 0; j < (int)this->hauteur; j++)
        {
            this->casesMarchables[i][j] = true;
        }
    }

    this->textureMur.loadFromFile("assets/stone_2_dark0.png");

    for (const Position& positionMur : positionsMurs)
    {
        sf::Sprite spriteMur;
        spriteMur.setTexture(this->textureMur);
        spriteMur.setPosition(ConvertirPositionPixels(positionMur));
        this->spritesMur.push_back(spriteMur);
        this->casesMarchables[positionMur.x][positionMur.y] = false;
    }
    
    // ajouter les sommets au graphe
    
    for (unsigned int i = 0; i < this->largeur * this->hauteur; i++)
    {
        graphe.ajouterSommet(i);
    }

    // ajouter les arcs au graphe

    for (unsigned int x = 0; x < this->largeur; x++)
    {
        for (unsigned int y = 0; y < this->hauteur; y++)
        {
            Position positionCourante = Position(x, y);

            unsigned int cleCourante = this->positionACle(positionCourante);

            std::list<Position> casesAdjacentes = this->positionsAdjacentes(positionCourante);

            for (const Position& positionAdjacente : casesAdjacentes)
            {
                if (this->casesMarchables[x][y] == true
                    && this->casesMarchables[positionAdjacente.x][positionAdjacente.y] == true)
                {
                    unsigned int cleAdjacente = this->positionACle(positionAdjacente);

                    try
                    {
                        this->graphe.ajouterArc(cleCourante, cleAdjacente, rand() % 100); // fait parfois des détours
                        // this->graphe.ajouterArc(cleCourante, cleAdjacente, 1); // = non pondéré
                    }
                    catch (const std::runtime_error&)
                    {
                        // l'arc en sens inverse a probablement déjà été ajouté
                    }
                }
            }
        }
    }

    // herbe
    this->textureHerbe.loadFromFile("assets/pebble_brown_4_old.png");
    for (const Position& positionHerbe : positionsHerbe)
    {
        sf::Sprite spriteHerbe;
        spriteHerbe.setTexture(this->textureHerbe);
        spriteHerbe.setPosition(ConvertirPositionPixels(positionHerbe));
        this->spritesHerbe.push_back(spriteHerbe);
    }

    // joueur
    this->textureJoueur.loadFromFile("assets/deep_elf_demonologist.png");
    this->textureJoueurPerdu.loadFromFile("assets/necromancy.png");
    this->spriteJoueur.setTexture(this->textureJoueur);
    this->spriteJoueur.setPosition(ConvertirPositionPixels(this->positionJoueur));

    // ennemi
    this->textureEnnemi.loadFromFile("assets/frost_giant_new.png");
    this->textureEnnemiPerdu.loadFromFile("assets/blood_green_2.png");
    this->spriteEnnemi.setTexture(this->textureEnnemi);
    this->spriteEnnemi.setPosition(ConvertirPositionPixels(this->positionEnnemi));

    // chemin ennemi
    this->texturesCheminEnnemi[0].loadFromFile("assets/walk_north.png");
    this->texturesCheminEnnemi[1].loadFromFile("assets/walk_south.png");
    this->texturesCheminEnnemi[2].loadFromFile("assets/walk_east.png");
    this->texturesCheminEnnemi[3].loadFromFile("assets/walk_west.png");
    this->mettreAJourCheminEnnemi();

    // porte
    this->texturePorte.loadFromFile("assets/entrance.png");
    this->spritePorte.setTexture(this->texturePorte);
    this->spritePorte.setPosition(ConvertirPositionPixels(this->positionPorte));
}

void Grille::dessiner(sf::RenderWindow& window)
{
    window.draw(this->spritePlancher);

    window.draw(this->spritePorte);

    for (const sf::Sprite& spriteMur : this->spritesMur)
    {
        window.draw(spriteMur);
    }

    for (const sf::Sprite& spriteHerbe : this->spritesHerbe)
    {
        window.draw(spriteHerbe);
    }

    if (this->etatJeu == EtatJeu::EnCours)
    {
        for (const sf::Sprite& spriteCheminEnnemi : this->spritesCheminEnnemi)
        {
            window.draw(spriteCheminEnnemi);
        }
    }

    window.draw(this->spriteJoueur);

    if (this->etatJeu == EtatJeu::EnCours || this->etatJeu == EtatJeu::Gagne)
    {
        window.draw(this->spriteEnnemi);
    }
}

void Grille::bougerJoueur(Direction direction)
{
    if (this->etatJeu != EtatJeu::EnCours)
    {
        return;
    }

    if (this->horloge.getElapsedTime() - this->dernierDeplacementJoueur < sf::milliseconds(DelaiDeplacementJoueurMs))
    {
        return;
    }

    Position deplacement(0, 0);

    switch (direction)
    {
    case Direction::Nord: deplacement.y = -1; break;
    case Direction::Sud: deplacement.y = +1; break;
    case Direction::Est: deplacement.x = +1; break;
    case Direction::Ouest: deplacement.x = -1; break;
    }

    Position testPosition = this->positionJoueur;
    testPosition += deplacement;

    if (testPosition.x < 0 || testPosition.x >= (int)this->largeur)
    {
        return;
    }

    if (testPosition.y < 0 || testPosition.y >= (int)this->hauteur)
    {
        return;
    }

    if (this->casesMarchables[testPosition.x][testPosition.y] == false)
    {
        return;
    }

    this->positionJoueur = testPosition;

    this->spriteJoueur.setPosition(ConvertirPositionPixels(this->positionJoueur));

    this->dernierDeplacementJoueur = this->horloge.getElapsedTime();

    this->mettreAJourCheminEnnemi();
}

void Grille::bougerEnnemi()
{
    if (this->etatJeu != EtatJeu::EnCours)
    {
        return;
    }

    if (this->horloge.getElapsedTime() - this->dernierDeplacementEnnemi < sf::milliseconds(DelaiDeplacementEnnemiMs))
    {
        return;
    }

    if (this->cheminEnnemi.empty())
    {
        return;
    }

    Position prochainePosition = this->cleAPosition(this->cheminEnnemi.front());
    this->cheminEnnemi.pop_front();
    this->spritesCheminEnnemi.pop_front();

    this->positionEnnemi = prochainePosition;

    this->spriteEnnemi.setPosition(ConvertirPositionPixels(this->positionEnnemi));

    this->dernierDeplacementEnnemi = this->horloge.getElapsedTime();

    if (this->positionEnnemi == this->positionJoueur)
    {
        this->etatJeu = EtatJeu::Perdu;
        this->spriteJoueur.setTexture(this->textureJoueurPerdu);
    }
    else if (this->positionJoueur == this->positionPorte)
    {
        this->etatJeu = EtatJeu::Gagne;
        this->spriteEnnemi.setTexture(this->textureEnnemiPerdu);
    }
}

sf::Vector2f Grille::ConvertirPositionPixels(const Position& position)
{
    return sf::Vector2f(position.x * (float)LargeurTuilePx, position.y * (float)HauteurTuilePx);
}

std::list<Position> Grille::positionsAdjacentes(const Position& position) const
{
    std::list<Position> resultat;

    if (position.x - 1 >= 0)
    {
        resultat.push_back(Position(position.x - 1, position.y));
    }

    if (position.x + 1 < (int)this->largeur)
    {
        resultat.push_back(Position(position.x + 1, position.y));
    }

    if (position.y - 1 >= 0)
    {
        resultat.push_back(Position(position.x, position.y - 1));
    }

    if (position.y + 1 < (int)this->hauteur)
    {
        resultat.push_back(Position(position.x, position.y + 1));
    }

    return resultat;
}

void Grille::mettreAJourCheminEnnemi()
{
    unsigned int cleEnnemi = this->positionACle(this->positionEnnemi);

    unsigned int cleJoueur = this->positionACle(this->positionJoueur);

    this->cheminEnnemi = this->graphe.dijkstra(cleEnnemi, cleJoueur);

    this->cheminEnnemi.pop_front(); // déjà sur cette case

    this->spritesCheminEnnemi.clear();

    std::vector<unsigned int> chemin(this->cheminEnnemi.begin(), this->cheminEnnemi.end());

    Direction derniereDirection = Direction::Ouest;

    for (size_t i = 0; i < chemin.size(); i++)
    {
        unsigned int cle = chemin[i];

        Position position = this->cleAPosition(cle);

        sf::Sprite spriteCheminEnnemi;

        if (i == chemin.size() - 1)
        {
            spriteCheminEnnemi.setTexture(this->texturesCheminEnnemi[(int)derniereDirection]);
        }
        else
        {
            unsigned int nextCle = chemin[i + 1];
            Direction dir = this->obtenirDirection(cle, nextCle);
            spriteCheminEnnemi.setTexture(this->texturesCheminEnnemi[(int)dir]);
            derniereDirection = dir;
        }

        spriteCheminEnnemi.setPosition(ConvertirPositionPixels(position));

        this->spritesCheminEnnemi.push_back(spriteCheminEnnemi);
    }
}

Direction Grille::obtenirDirection(unsigned int cleSource, unsigned int cleDest) const
{
    Position source = this->cleAPosition(cleSource);

    Position dest = this->cleAPosition(cleDest);

    if (dest.y - source.y < 0) return Direction::Nord;
    else if (dest.y - source.y > 0) return Direction::Sud;
    else if (dest.x - source.x > 0) return Direction::Est;
    else if (dest.x - source.x < 0) return Direction::Ouest;

    return Direction::Ouest;
}
