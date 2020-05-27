#include<iostream>
#include<map>
#include<string>
#include<vector>
#include<set>
#include <fstream>
using namespace std;

class DFA{

    set<int> Q, F;
    set<char> Sigma;
    int q0;
    map<pair<int,char>,int> delta;

    public:
    DFA() { this->q0 = 0; }
	DFA(set<int> Q, set<char> Sigma, map<pair<int, char>, int> delta, int q0, set<int> F)
	{
		this->Q = Q;
		this->Sigma = Sigma;
		this->delta = delta;
		this->q0 = q0;
		this->F = F;
	}

	set<int> getQ() const { return this->Q; }
	set<int> getF() const { return this->F; }
	set<char> getSigma() const { return this->Sigma; }
	int getInitialState() const { return this->q0; }
	map<pair<int, char>, int> getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, DFA&);
	friend ostream& operator << (ostream&, DFA);
    void minimizare(DFA&);

};

istream& operator >> (istream& f, DFA& M)
{
	int noOfStates;
	f >> noOfStates;
	for (int i = 0; i < noOfStates; ++i)
	{
		int q;
		f >> q;
		M.Q.insert(q);
	}

	int noOfLetters;
	f >> noOfLetters;
	for (int i = 0; i < noOfLetters; ++i)
	{
		char ch;
		f >> ch;
		M.Sigma.insert(ch);
	}

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i)
	{
		int s, d;
		char ch;
		f >> s >> ch >> d;
		M.delta[{s, ch}] = d;
	}

	f >> M.q0;

	int noOfFinalStates;
	f >> noOfFinalStates;
	for (int i = 0; i < noOfFinalStates; ++i)
	{
		int q;
		f >> q;
		M.F.insert(q);
	}

  

	return f;
}

ostream& operator << (ostream& g, DFA M)
{
	cout<<"Starea initiala este "<<M.q0<<endl;
	for(auto i : M.delta)
	{
		g<< i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	}	
	cout<<"Starile finale sunt: ";
	for(auto i : M.F)
	{
		cout<<i<<" ";
	}


}



int part(int v, vector<set<int>> P0)
{ // functia returneaza numarul (indexul) partitiei din care face parte o stare v in vectorul de partitii P0
	int k = 0;
	for(auto i : P0)
	{
		for(auto j : i)
		{
			if(j == v)
			{
				return k;

			}
		}
		k++;
	}
	return -1;
}

vector<set<int>> partitii(vector<set<int>> P0, map<pair<int,char>,int> delta, set<char> Sigma, int& ok)
{
	//functia verifica partitiile deja existente in P0 si creeaza altele pornind de la ele
	vector<set<int>> P; //noul vector de partitii care va fi returnat
	ok = 0; //variabila care verifica daca s-a produs vreo modificare in vectorul de partitii
	for(auto i : P0)
	{
		//iau fiecare set de stari din P0, adica fiecare partitie din vectorul vechi
		set<int> prov; // partitia noua
		for(auto j : i)
		{
			for(auto k : i)
			{
				//vreau sa compar starile doua cate doua, pe rand, si sa verific daca trebuie sa le pun in partitii diferite
				for(char w : Sigma)
				{
					if(prov.find(k) == prov.end() && prov.find(j) == prov.end()) // daca deja sunt in partia noua, le las asa
					{
						int v1 = delta[{j, w}]; //obtin starea in care ajunge j cu caracterul w
						int v2 = delta[{k, w}]; //obtin starea in care ajunge k cu caracterul w
						if(part(v1, P0) != part(v2, P0))//daca cele doua stari obtinute se afla in partitii diferite, starile j si k trebuie puse si ele in partitii diferite
						{
							prov.insert(k); // inserez k in partitia nou formata
							ok = 1; // s-a produs o modificare in vectorul de partitii
						}
					}
				}
			}
		
		
		}
		
		set<int> nou;// set care va contine starile care nu au fost inserate in partitia nou formata, cele care nu au fost modificate

		for(auto j : i)
		{
			if(prov.find(j) == prov.end())
			{
				nou.insert(j);
			}
		}

		P.push_back(nou);
		nou.clear();
		P.push_back(prov);
		prov.clear();
		// P va contine partitiile nou formate, dar si pe cele vechi care nu au fost modificate
		
	}

	return P;
}

void DFA::minimizare(DFA& M)
{
	set<int> nefinale; //set in care pun starile nefinale
	set<int> finale; // set in care pun starile finale
	//la inceput vor fi doar doua partitii, cea a starilor finale si cea a starilor nefinale
	for(int i : Q)
	{
		int ok = 1;
		for(int j : F)
		{
			if(i == j)
			{
				finale.insert(i);
				ok = 0;
			}
		}
		if(ok)
		{
			nefinale.insert(i);
		}
	}
	vector<set<int>> P0;
	P0.push_back(nefinale);
	P0.push_back(finale);
	//acum vectorul P0 are cele doua partitii mentionate mai sus
	int ok = 1;
	vector<set<int>> P = partitii(P0, delta, Sigma, ok);
	while(ok){ // cat timp se produc modificari in impartirea partitiilor, aplic algoritmul de separare a starilor
		P = partitii(P, delta, Sigma, ok);
	}
	vector<set<int>> intermediar;
	for(auto i : P)
	{
		if(!i.empty())
		{
			intermediar.push_back(i);
		}
	}
	P.clear();
	for(auto i : intermediar)
	{
		P.push_back(i);
	}

	map<pair<int,char>,int> newDelta;
	set<int> newQ;
	set<int> newF;
	int new_q0;
	int nrStare = 0;
	for(auto i : P)
	{
		newQ.insert(nrStare); //renumerotez starile, astfel incat, daca nr de stari dintr-o partitie > 2, aceasta va forma o singura stare, cu indexul partitiei
		for(auto j : i)
		{
			if(j == q0)
			{
				new_q0 = nrStare; // partitia care contine starea initiala va forma noua stare initiala
			}
			for(auto k : F)
			{
				if(j == k)
				{
					newF.insert(nrStare); // partitiile care contin stari finale vor forma noile stari finale
				}
			}
		}
		for(auto w : Sigma)
		{
			int p = part(delta[{*i.begin(),w}], P);
			newDelta[{nrStare, w}] = p; //delta e actualizat in functie de noua numerotare a starilor
		}
		nrStare++;
	}

	Q.clear();
	Q = newQ;
	F.clear();
	F = newF;
	delta.clear();
	delta = newDelta;


}


using namespace std;

int main()
{
	DFA M;
	
	ifstream fin("dfa_minimization.txt");
	fin >> M;
	fin.close();

	M.minimizare(M);

	cout<<M;


    return 0;
}