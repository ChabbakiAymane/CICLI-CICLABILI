#include <fstream>
#include <vector>
#include <iostream>
#include <climits>
#include <queue>
#include <bits/stdc++.h>

using namespace std;

//----------------------------------------------------- STRUTTURE DATI -----------------------------------------------------

//Struttura dati per i nodi che contiene campo booleano isBorder che specifica se il nodo è un nodo di confine del ciclo
struct nodo{
	vector<int> vic;
	bool isBorder = false;
};
typedef vector<nodo> Grafo;

//Struttura dati per i nodi dell'albero
struct nodoA{
	vector<int> figli;
	int montresorMenoQuattro = -1;
};
typedef vector<nodoA> Tree;

//Struttura dati per memorizzare inizio e fine del percorso
struct percorso{
	int inizio;
	int fine;
};

//---------------------------------------------------- TRACCIA PROGETTO ----------------------------------------------------
/**
 * Progetto n. 1 di ASD a.a 2019/2020:
 * 	- Gruppo: 
 * 		- (P)reasoners of the Void
 * 
 * 	- Esecuzione:
 * 		- /usr/bin/g++ -DEVAL -std=gnu++11 -O2 -pipe -static -s -o cicli cicli.cpp
 * 		- ./cicli
 * 
 * 	- Matricole:
 * 		- 201645: Leonardo Xompero
 * 		- 201973: Aymane Chabbaki
 * 		- 202691: Massimo Rizzoli
 * 
 * Traccia:
 * 		- La traccia del problema (con vincoli, assunzioni e note) è reperibile da: https://judge.science.unitn.it/arena/tasks/cicli/description
 * 
 * IMPORTANTE:
 * 		- Il gruppo ha preso spunto per implementazioni e per idea facendo riferimento ai lucidi sul sito del docente Alberto Montresor 
 * 		 (integrandoli anche con il materiale presente sul libro del docente) e sul materiale presente sul sito degli esercitatori.
 * 		- Per risolvere particolari problemi o per soluzioni efficienti, ci siamo affidati a paper/ricerche e a siti trovati sul Web, 
 * 		  in particolare: 
 * 			- Distance between two nodes --> https://www.geeksforgeeks.org/queries-find-distance-two-nodes-binary-tree-ologn-method/
 * 			- Best approach to find this distance  --> https://www.quora.com/What-would-be-the-best-approach-to-find-the-distance-between-two-nodes-of-a-tree
 * 			- LCA --> https://www.geeksforgeeks.org/find-lca-in-binary-tree-using-rmq/
 * 
 * 		- Teniamo a precisare che non è stato un lavoro di "CTRL-C" e "CTRL-V", ma tutt'altro; abbiamo prima studiato e capito a fondo la logica
 * 		  dietro a queste tecniche e il loro funzionamento, e successivamente abbiamo provedutto a modificarli e renderli adeguati per il
 * 		  problema che dobbiamo risolvere.
 * 		- Un'altra risorsa che è stata d'aiuto è il canale Youtube "JAlgs", dove spiega in modo dettagliato e fornisce un'implementazione generica
 * 		  delle operazioni che abbiamo usato:
 * 			- Canale: https://www.youtube.com/channel/UCIyYdngljsLPMe2HiJ-icPw
 * 			- Lowest Common Ancestor Tutorial --> https://www.youtube.com/watch?v=HeeyUZmaZg0
 */

//---------------------------------------------------- CODICE PROGETTO --------------------------------------------------------------------------------------

/** Funzione che crea un albero dato un grafo in input, salvandosi i parent dei nodi e (se presenti) i loro fratelli (nodi appartenenti alla stessa cricca)
 * @param:
 * 	- Grafo &g --> grafo passato per riferimento da trasformare in albero
 *  - Tree &t --> albero passato per riferimento da inizializzare
 *  - vector<int> &parent --> vettore passato per riferimento da inizializzare con i padri del nodo
 *  - vector<unordered_set<int>> &brother --> vettore di unordered set passato per riferimento da inizializzare con i fratelli del nodo
 * 	  (nodi appartenenti alla stessa cricca)
 */
void monTreeSor(Grafo &g, Tree &t, int radice, vector<int> &parent, vector<unordered_set<int>> &brother){
	vector<bool> visited; //Vettore dove salvo i nodi visitati
	queue<int> q;
	int u;

	visited.resize(g.size());
	t.resize(g.size());
	parent.resize(g.size());
	brother.resize(g.size());
	
	for(int i = 0; i < g.size(); i++){
		visited[i] = false;
		parent[i] = -1;
	}

	//Inizializzo la radice dell'albero
	visited[radice] = true;
	parent[radice] = radice;
	t[radice].montresorMenoQuattro = 0;
	q.push(radice);

	//BFS del grafo per generare l'albero
	while(!q.empty()){
		u = q.front();
		q.pop();

		for(int v : g[u].vic){
			if(!visited[v]){
				visited[v] = true;
				q.push(v);
				t[u].figli.push_back(v);
				parent[v] = u;
				t[v].montresorMenoQuattro = t[u].montresorMenoQuattro + 1;
			}else if(v != parent[u]){ //Se il nodo non è mio padre
				//Se il padre del nodo u vicino a v è anche mio padre allora u è mio fratello
				if(parent[v] == parent[u]){
					brother[u].insert(v);
				}
			}
		}
	}
}

/** Funzione per calcolare la distanza tra 2 nodi qualsiasi dell'albero
 * @param: 
 * 	- Tree &t --> albero da scorrere
 *  - int u --> nodo di partenza
 *  - int v --> nodo di destinazione
 *  - vector<int> &parent --> vettore passato per riferimento che contiene i padri del nodo
 *  - vector<unordered_set<int>> &brother --> vettore di unordered set che contiene i fratelli del nodo (nodi appartenenti alla stessa cricca)
*/
int calcolaDistanza(Tree &t, int u, int v, vector<int> &parent, vector<unordered_set<int>> &brother){
	//Variabile booleana per fermare il ciclo quando ho calcolato la distanza dei nodi
	bool stop = false;

	//Calcolo Erdos dei due nodi
	int erdU = t[u].montresorMenoQuattro;
	int erdV = t[v].montresorMenoQuattro;

	//Calcolo la distanza tra i nodi u e v in termini di distanza dal root
	int dist = erdU + erdV;

	//Mi salvo i padri dei nodi u e v
	int puNext = u;
	int pvNext = v;

	//Se i due nodi sono a livelli differenti, risalgo i livelli del nodo più profondo
	while(erdU < erdV){
		pvNext = parent[pvNext];
		erdV--;
	}
	
	while(erdV < erdU){
		puNext = parent[puNext];
		erdU--;
	}

	//Continuo a risalire le lista dei padri finché non trovo un nodo comune ad entrambi i nodi o una delle condizioni sottostanti si verifichi
	while(!stop){
		//Se il nodo che ho raggiunto contiene nella lista dei fratelli il padre del nodo che voglio raggiungere, calcolo la distanza
		if(brother[puNext].find(pvNext) != brother[puNext].end()){
			// *2 perché siamo sullo stesso livello
			dist = dist - 2*(t[puNext].montresorMenoQuattro) + 1;
			stop = true;
		}else if(puNext == pvNext){
			//Se il nodo che ho raggiunto è lo stesso del nodo di arrivo (padre sia mio che dell'altro nodo), calcolo la distanza:
			dist = dist - 2*(t[puNext].montresorMenoQuattro);
			stop = true;
		}else{
			//Nessun caso sopra si è verificato, devo salire al livello soprastante
			puNext = parent[puNext];
			pvNext = parent[pvNext];
		}
	}
	return dist;
}
//-------------------------------------------------------- MAIN --------------------------------------------------------
int main(){
	int N; //Numero di luoghi
	int M; //Numero di piste ciclabili
	int Q; //Numero di richieste
	//cout << "Ciaone" << endl;
	Grafo grafoCiclabile; //Piste ciclabili
	Tree t; //Creo l'albero t basato sul grafo ciclabile
	vector<percorso> vRichieste; //Richieste dei cittadini
	vector<int> parent; //Vettore che contiene i padri del nodo
	vector<unordered_set<int>> brother; //Vettore che contiene i fratelli del nodo

	ifstream in("input.txt");
	in >> N >> M >> Q;

	grafoCiclabile.resize(N);
	vRichieste.resize(Q);
	
	//Inserisco nel grafo i nodi collegati da una pista ciclabile
	int j, from, to;
	for (j = 0; j < M; j++){
		in >> from >> to;
		//Il grafo non è orientato, quindi (a -> b) e (b -> a)
		grafoCiclabile[from].vic.push_back(to);
		grafoCiclabile[to].vic.push_back(from);
	}

	//Inserisco nel vettore delle richieste i Q nodi di parcdztenza e di arrivo
	for (j = 0; j < Q; j++){
		//Inizializzo la variabile d'appoggio 'tmp' per inizializzare il vettore delle richieste
		in >> vRichieste[j].inizio >> vRichieste[j].fine;
	}

	ofstream out("output.txt");
	
	//Funzione che crea l'albero di copertura sul grafo e inizializza i vettori parent e brother (si basa su Erdos)
	monTreeSor(grafoCiclabile, t, 0, parent, brother);

	for(j = 0; j < Q; j++){
		//Funzione che calcola il percorso minimo tra nodo di inizio e di fine
		out << calcolaDistanza(t, vRichieste[j].inizio, vRichieste[j].fine, parent, brother) << "\n";
	}
	return 0;
}