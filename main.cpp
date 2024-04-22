#include <SFML/Graphics.hpp>

#include "Grille.h"

static const std::vector<std::string> NIVEAU = {
    "WWW__GGGGW___________________W_W",
    "_PW___GGGW___W____GGGG______GW_W",
    "_WW_____GWWWWWW__GGGGGG_____GW_W",
    "_WG_____GW_GGW___WWWWWWWG___GW_W",
    "_WGGG___GW__GW__WGGGWG__WWGGGW_W",
    "_WWWW___GW__GW__WG__W_____WWWW_W",
    "_____GGGGW__G__WG___W__WGGG____W",
    "WWWWWWW_GW___GGWGGG___WWWWWWWW_W",
    "WGGGGG__GW____GGGWWW_________W_W",
    "WGG____GWW____GW____G___GGGG_W_W",
    "WGG___WWGW____GW____W__WWWWWWW_W",
    "WG____WGGG____GW____W_GWGGGG___W",
    "W__W__WGGW_GGGW____WWWWWG_WGWG_W",
    "_GGW__WGGW_GW____W____GWG_WGWG_W",
    "GWWWWWWGWW__W__WWWWW__GWWWWGWWGW",
    "_GGW____GW__W___GW____GW__GGGGWW",
    "_____WWWWW_W____GW___GGW_WWWWWDW",
    "________GGG_________GGGW______EW",
};

int main()
{
    Grille grille = Grille::CreerDuNiveau(NIVEAU);

    sf::RenderWindow window(
        sf::VideoMode(grille.obtenirLargeurPx(), grille.obtenirHauteurPx()),
        "INFO3212 - Exercices 6",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);
        
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Up: grille.bougerJoueur(Direction::Nord); break;
                case sf::Keyboard::Down: grille.bougerJoueur(Direction::Sud); break;
                case sf::Keyboard::Right: grille.bougerJoueur(Direction::Est); break;
                case sf::Keyboard::Left: grille.bougerJoueur(Direction::Ouest); break;
                }
            }
        }

        grille.bougerEnnemi();

        window.clear();

        grille.dessiner(window);

        window.display();
    }

    return 0;
}
