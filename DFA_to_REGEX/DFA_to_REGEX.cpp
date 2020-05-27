#include <iostream>
#include <map>
#include <string>
#include <set>
#include<vector>
#include <fstream>
using namespace std;

class DFA
{
	set<int> Q, F;
	set<string> Sigma;
	int q0;
	map<pair<int, string>, int> delta;

public:
	DFA() { this->q0 = 0; }
	DFA(set<int> Q, set<string> Sigma, map<pair<int, string>, int> delta, int q0, set<int> F)
	{
		this->Q = Q;
		this->Sigma = Sigma;
		this->delta = delta;
		this->q0 = q0;
		this->F = F;
	}

	set<int> getQ() const { return this->Q; }
	set<int> getF() const { return this->F; }
	set<string> getSigma() const { return this->Sigma; }
	int getInitialState() const { return this->q0; }
	map<pair<int, string>, int> getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, DFA&);
    string regex(DFA&);
};


istream& operator >> (istream& f, DFA& M)
{
	int noOfStates;
	f >> noOfStates;
	for (int i = 0; i < noOfStates; ++i)
	{
		int q;
		f >> q;
		M.Q.insert(q+1); //vreau ca numerotarea starilor sa fie facuta de la 1
	}

	int noOfLetters;
	f >> noOfLetters;
	for (int i = 0; i < noOfLetters; ++i)
	{
		string ch;
		f >> ch;
		M.Sigma.insert(ch);
	}

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i)
	{
		int s, d;
		string ch;
		f >> s >> ch >> d;
		M.delta[{s+1, ch}] = d+1;
	}

	f >> M.q0;
	M.q0++;

	int noOfFinalStates;
	f >> noOfFinalStates;
	for (int i = 0; i < noOfFinalStates; ++i)
	{
		int q;
		f >> q;
		M.F.insert(q+1);
	}

	return f;
}



string drum(int i, int j, map<pair<int,string>,int> delta) 
{
	//functie care returneaza drumul (sirul de caractere) dintre doua noduri
	for(auto k : delta)
	{
		if(k.first.first == i && k.second == j)
			return k.first.second; 
	}
	return "";
} 

void stergere(int nod, map<pair<int,string>,int>& delta, set<string>& Sigma)
{
	vector<int> in; // vector in care retin ce noduri intra in nodul pe care 
					// vreau sa il sterg
	vector<int> out; // vector in care retin ce noduri ies din nodul pe care
					// vreau sa il sterg
	for(auto i : delta)
	{
		if(i.second == nod && i.first.first != nod)
		{
			in.push_back(i.first.first);
		}
		else if(i.first.first == nod && i.second!=nod)
		{
			out.push_back(i.second);
		}
	}
	for(int i : in)
	{
		for(int j : out)
		{
			//fac "produsul cartezian" al nodurilor din in cu cele din out
			string drumNou= "";
			if (drum(i,j,delta) != "")
			{
				drumNou = "(" + drum(i,j,delta) + ") + " ; // salvez ce aveam deja pe drumul dintre cele doua noduri

				for(map<pair<int,string>,int>::iterator k = delta.begin(); k != delta.end();)
				{
					if(k->first.first == i && k->second == j)
					{
						delta.erase(k++); // sterg drumul deja existent dintre cele doua drumuri
					}
					else k++;
				}
			}
			drumNou += "(" + drum(i,nod,delta)+")"; // adaug drumul de la in la nodul pe care vreau sa il sterg
			if(drum(nod,nod,delta) != "")
			{
				drumNou += "(" + drum(nod,nod,delta) + ")*"; // adaug drumul de la nod la el insusi (bucla)
			}
			drumNou += "(" + drum(nod, j, delta) + ")"; // adaug drumul de la nod la out
			
			delta[{i,drumNou}] = j; //refac drumul in-out
			Sigma.insert(drumNou); // adaug expresia la alfabet
		}

		
	}

	for(map<pair<int,string>,int>::iterator i = delta.begin(); i != delta.end();)
	{
		if(i->first.first == nod || i->second == nod)
		{
			delta.erase(i++); // sterg toate drumurile care duceau catre nodul pe care l-am sters
		}
		else i++;
	}

}


string DFA::regex(DFA& M)
{
	
    for(auto i: delta)
    {
		int ok = 0;
		string t;
        for(auto j:delta)
        {
			if(i.first.first == j.first.first && i.second == j.second && i.first.second != j.first.second) 
			{
				// daca de la un nod plec cu mai multe litere catre un acelasi nod, scriu literele sub forma de expresie ("a+b")
				t = j.first.second + " + " + i.first.second;
				ok = 1;
				delta.erase({i.first.first, i.first.second});
				delta.erase({i.first.first, j.first.second});//sterg drumurile vechi
			}

        }
		if(ok)
		{
			
			delta[{i.first.first, t}] = i.second; //il adaug pe cel nou
			Sigma.insert(t);

		}
		
    }

	int ok = 0;

	for(auto i : delta)
	{
		
		if(i.second == q0) //verific daca exista vreo tranzitie catre starea initiala
		{
			ok = 1;
			break;
		}
	}

	if(ok)
	{
		delta[{0, "."}]=q0; //creez o noua stare initiala si o lambda-tranzitie catre vechea stare initiala
		Q.insert(0);
		q0 = 0;
		Sigma.insert(".");
	}

	ok = 0;

	for(auto i:delta)
	{
		for(int j:F)
		{
			if(i.first.first == j)
				ok = 1; // verific daca pleaca vreo tranzitie din una dintre starile finale
		}
	}
	
	if(F.size() > 1 || ok)
	{
		int newSt = 0;
		for(int i : Q)
		{
			if(i > newSt)
			{
				newSt = i;
			}
		}

		newSt++;
		for(int i : F)
		{
			delta[{i,"."}] = newSt; //creez n o noua stare finala si lambda-tranzitii de la vechile stari finale la ea
		}

		F.clear(); // vechile stari finale se anuleaza
		F.insert(newSt); 
		Sigma.insert(".");
	}
	int final = *F.begin();
	int start = 2;
	if(q0 == 0)
	{
		start = 1; // am inceput numerotarea starilor de la 1
				// Daca am creat o alta stare initiala, am numerat-o cu 0.
				// Asa ca, daca am exista starea 0, o sa incep sa sterg nodurile cu 1.
				// Daca nu, o sa incep cu urmatorul nod dupa starea initiala, adica 2.
	}
	for(int i = start; i < final; i++)
	{
		stergere(i,delta,Sigma); //sterg pe rand toate nodurile
	}
	for(auto i : delta)
	{
		return i.first.second; //returnez expresia
	}
}

int main()
{
	DFA M;
	
	ifstream fin("dfa.txt");
	fin >> M;
	fin.close();

	

	string expresie = M.regex(M);
	for(int i = 0; i < expresie.length(); i++)
	{
		if(expresie[i]=='(' && expresie[i+1]=='.' && expresie[i+2]==')' && (expresie[i+3]=='(' || expresie[i-1] == ')'))
		{
			expresie.erase(expresie.begin()+i, expresie.begin()+i+3);
		}
		
	} //elimin inmultirile cu lambda

	for(int i = 0; i < expresie.length(); i++)
	{
		if(expresie[i]=='(' && expresie[i+2] == ')')
			{
				expresie.erase(expresie.begin()+i+2);
				expresie.erase(expresie.begin()+i);
				
			}
	} //elimin parantezele pentru un singur caracter
	cout<<expresie;
	
	

	return 0;
}