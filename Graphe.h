#pragma once

#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <queue>

constexpr unsigned int INFINI = std::numeric_limits<unsigned int>::max();

typedef std::list<unsigned int> Chemin;

struct Arc;

struct Noeud
{
	unsigned int cle;
	unsigned int distance;
	Noeud* parent;
	bool visite;
	std::list<Arc*> arcs;
};

struct Arc
{
	unsigned int poids;
	Noeud* destination;
};

class Graphe
{
public:

	/// <summary>
	/// Crée un graphe vide.
	/// </summary>
	Graphe() : noeuds() {}

	/// <summary>
	/// Ajoute un sommet portant la clé spécifiée.
	/// </summary>
	/// <param name="cle">La clé du sommet à ajouter.</param>
	/// <exception cref="std::runtime_error">
	/// Lancée si un sommet portant cette clé existe déjà.
	/// </exception>
	void ajouterSommet(unsigned int cle);

	/// <summary>
	/// Ajoute un arc (pondéré) entre le sommet identifié par <paramref name="cleSource"/>
	/// et le sommet identifié par <paramref name="cleDest"/>. 
	/// L'arc en sens inverse est automatiquement ajouté.
	/// </summary>
	/// <param name="cleSource">La clé du sommet source.</param>
	/// <param name="cleDest">La clé du sommet destination.</param>
	/// <exception cref="std::runtime_error">
	/// Lancée si le sommet source n'existe pas, ou si le sommet destination n'existe pas,
	/// ou s'il existe déjà un arc entre les sommets source et destination.
	/// </exception>
	void ajouterArc(unsigned int cleSource, unsigned int cleDest, unsigned int poids);

	/// <summary>
	/// Retourne les clés des sommets formant le plus court chemin entre 
	/// le sommet identifié par <paramref name="cleSource"/> 
	/// et le sommet identifié par <paramref name="cleDest"/>.
	/// </summary>
	/// <param name="cleSource">La clé du sommet source.</param>
	/// <param name="cleDest">La clé du sommet destination.</param>
	/// <returns>Les clés des sommets formant le plus court chemin entre les sommets spécifiés.</returns>
	/// <exception cref="std::runtime_error">
	/// Lancée si le sommet destination est inatteignable à partir du sommet source.
	/// </exception>
	Chemin dijkstra(unsigned int cleSource, unsigned int cleDest);

	/// <summary>
	/// Affiche le chemin fourni.
	/// </summary>
	/// <param name="nomChemin">Le nom du chemin à afficher.</param>
	/// <param name="chemin">Un chemin composé de clés de sommets.</param>
	static void AfficherChemin(const std::string& nomChemin, const Chemin& chemin)
	{
		std::cout << nomChemin << std::endl;

		int i = 0;

		for (unsigned int cle : chemin)
		{
			std::cout << cle;

			if (i++ < chemin.size() - 1)
			{
				std::cout << " -> ";
			}
		}

		std::cout << std::endl << std::endl;
	}

private:

	Noeud* rechercherSommetNonVisiteDistanceMin();

	std::map<unsigned int, Noeud*> noeuds;
};

inline void Graphe::ajouterSommet(unsigned int cle)
{
	auto it = this->noeuds.find(cle);

	if (it != this->noeuds.end())
	{
		throw std::runtime_error("Sommet existe deja");
	}

	Noeud* nouveau = new Noeud();
	nouveau->cle = cle;

	this->noeuds[cle] = nouveau;
}

inline void Graphe::ajouterArc(unsigned int cleSource, unsigned int cleDest, unsigned int poids)
{
	auto itSource = this->noeuds.find(cleSource);

	if (itSource == this->noeuds.end())
	{
		throw std::runtime_error("Sommet source introuvable");
	}

	Noeud* source = itSource->second;

	auto itDest = this->noeuds.find(cleDest);

	if (itDest == this->noeuds.end())
	{
		throw std::runtime_error("Sommet destination introuvable");
	}

	Noeud* dest = itDest->second;

	auto itArcEndroit = std::find_if(source->arcs.begin(), source->arcs.end(),
		[cleDest](Arc* arc)
		{
			return arc->destination->cle == cleDest;
		});

	if (itArcEndroit != source->arcs.end())
	{
		throw std::runtime_error("Arc existe deja (endroit)");
	}

	auto itArcEnvers = std::find_if(dest->arcs.begin(), dest->arcs.end(),
		[cleSource](Arc* arc)
		{
			return arc->destination->cle == cleSource;
		});

	if (itArcEnvers != dest->arcs.end())
	{
		throw std::runtime_error("Arc existe deja (envers)");
	}

	Arc* nouveauEndroit = new Arc();
	nouveauEndroit->poids = poids;
	nouveauEndroit->destination = dest;
	
	source->arcs.push_back(nouveauEndroit);

	Arc* nouveauEnvers = new Arc();
	nouveauEnvers->poids = poids;
	nouveauEnvers->destination = source;

	dest->arcs.push_back(nouveauEnvers);
}

inline Chemin Graphe::dijkstra(unsigned int cleSource, unsigned int cleDest)
{
	// À COMPLÉTER

	Chemin chemin;
	
	for(auto V : noeuds)
	{
		V.second->distance = INFINI;
		V.second->visite = false;
		V.second->parent = nullptr;
	}

	noeuds[cleSource]->distance = 0;
	while (true)
	{
		Noeud* V = rechercherSommetNonVisiteDistanceMin();
		if (V == nullptr)
		{
			//sortir de la boucle while
			break;
		}
		V->visite = true;
		for (auto arc : V->arcs)
		{
			Noeud* w = arc->destination;
			unsigned int dw = V->distance + arc->poids;
			if (!w->visite && dw < w->distance)
			{
				w->distance = dw;
				w->parent = V;
			}
		}
		Noeud* temp = new Noeud();
		temp = noeuds[cleDest];
		while (temp->parent!= nullptr)
		{
			chemin.push_front(temp->cle);
			temp = temp->parent;
		}
		chemin.push_front(cleSource);
	}
	return chemin;
}

inline Noeud* Graphe::rechercherSommetNonVisiteDistanceMin()
{
	// À COMPLÉTER
	Noeud* resultat = nullptr;
	int distanceMin = INFINI;
	for (auto V : noeuds)
	{
		if (!V.second->visite && V.second->distance < distanceMin)//si le sommet n'est pas visité et que sa distance est plus petite que la distance minimale
		{
			resultat = V.second;
			distanceMin = V.second->distance;
		}
	}
	return resultat;
}



//inline Chemin Graphe::dijkstra(unsigned int cleSource, unsigned int cleDest)
//{
//	Chemin chemin;
//	for (auto& paire : this->noeuds)//initialisations
//	{
//		Noeud* noeud = paire.second;
//		noeud->distance = INFINI;
//		noeud->parent = nullptr;
//		noeud->visite = false;
//	}
//
//	Noeud* source = this->noeuds[cleSource];
//	Noeud* dest = this->noeuds[cleDest];
//
//	source->distance = 0;
//
//	while(true)
//	{
//		Noeud* V = rechercherSommetNonVisiteDistanceMin();
//		if (V == nullptr)
//		{
//			break;
//		}
//		V->visite = true;
//		for (auto& arc : V->arcs)
//		{
//			Noeud* w = arc->destination;
//			int dw = V->distance + arc->poids;
//			if (dw < w->distance)
//			{
//				w->distance = dw;
//				w->parent = V;
//			}
//		}
//		Noeud* temp = dest;
//		while (temp != nullptr)
//		{
//			chemin.push_front(temp->cle);
//			temp = temp->parent;
//		}
//		//chemin.push_front(temp->cle);
//	}
//	return chemin;
//}

//inline Chemin Graphe::dijkstra(unsigned int cleSource, unsigned int cleDest)
//{
//	Chemin chemin;
//	for (auto& paire : this->noeuds)
//	{
//		Noeud* noeud = paire.second;
//		noeud->distance = INFINI;
//		noeud->parent = nullptr;
//		noeud->visite = false;
//	}
//	Noeud* source = this->noeuds[cleSource];
//	Noeud* dest = this->noeuds[cleDest];
//	source->distance = 0;
//	while (true)
//	{
//		Noeud* V = rechercherSommetNonVisiteDistanceMin();
//		if (V == nullptr)
//		{
//			break;
//		}
//		for (auto& arc : V->arcs)
//		{
//			Noeud* w = arc->destination;
//			int dw = V->distance + arc->poids;
//			if (dw < w->distance)
//			{
//				w->parent = V;
//				w->distance = dw;
//			}
//		}
//	}
//	Noeud* temp = dest;
//	while (temp != nullptr)
//	{
//		chemin.push_back(temp->cle);
//		temp = temp->parent;
//	}
//	return chemin;
//}
//
//inline Noeud* Graphe::rechercherSommetNonVisiteDistanceMin()
//{
//	Noeud* resultat = nullptr;
//	int dist_min = INFINI;
//	for (auto& paire : this->noeuds)
//	{
//		Noeud* noeud = paire.second;
//		if (!noeud->visite && noeud->distance < dist_min)
//		{
//			resultat = noeud;
//			dist_min = noeud->distance;
//		}
//	}
//	return resultat;
//}

//inline Noeud* Graphe::rechercherSommetNonVisiteDistanceMin()
//{
//	Noeud* res = nullptr;
//	int distMin = INFINI;
//	for (auto& paire : this->noeuds)
//	{
//		Noeud* noeud = paire.second;
//		if (!noeud->visite && noeud->distance < distMin)
//		{
//			res = noeud;
//			distMin = noeud->distance;
//		}
//	}
//
//	return res;
//}
