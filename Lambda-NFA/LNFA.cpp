#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
using namespace std;

class LNFA
{
	set<int> Q, F;
	set<char> Sigma;
	set <int> q0;
	map<pair<int, char>, set<int> > delta;

public:
	LNFA() { this->q0.insert(0); }
	LNFA(set<int> Q, set<char> Sigma, map<pair<int, char>, set <int> > delta, set <int> q0, set<int> F)
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
    set<int> getInitialState() const { return this->q0; }
	map<pair<int, char>,set <int> > getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, LNFA&);

	bool IntersectionNotEmpty(set <int>);
	set <int> deltaStar(set<int>, string);
	set <int> lambdaInchidere(int q);
};

bool LNFA::IntersectionNotEmpty(set<int> st)
{
	for (set<int>::iterator x=st.begin(); x!=st.end(); x++)
        for(set<int>::iterator y=F.begin(); y!=F.end(); y++)
            if(*x==*y) return true;
    return false;

}


istream& operator >> (istream& f, LNFA& M)
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
	M.Sigma.insert('#');

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i)
	{
		int s, d, nrstari;
		char ch;
		f >> s >> ch >> nrstari;
		for (int i = 0; i < nrstari; ++i)
        {
            f>>d;
            M.delta[{s, ch}].insert(d);
        }
	}

	int noOfInitialStates;
	f>>noOfInitialStates;
	for (int i = 0; i < noOfInitialStates; i++)
        {
            int x;
            f >> x;
            M.q0.insert(x);
        }

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

set<int> LNFA::lambdaInchidere( int q )
{
    set<int> lambdaTransitions;
    lambdaTransitions.insert(q);

    for(set<int>::iterator x = lambdaTransitions.begin(); x != lambdaTransitions.end();x++)
        for (set<int>::iterator i = delta[{*x, '#'}].begin(); i != delta[{*x, '#'}].end(); i++)
            lambdaTransitions.insert(*i);

    return lambdaTransitions;
}


set <int> LNFA::deltaStar(set<int> q, string w)
{

    int n=w.length();
    set <int> localFinalStates, z;
    z.insert(*q.begin());
    for (set<int>::iterator j=z.begin(); j!=z.end(); j++)
        {
            for(set <int>::iterator i = delta[{*j, w[0]}].begin(); i != delta[{*j, w[0]}].end(); i++ )
                localFinalStates.insert(*i);
        }
    n--;
    if (n==0)
    {
        return localFinalStates;
    }
    int contor=0;
    while(n)
    {
        set <int> auxiliar;
        for (set <int>::iterator i=localFinalStates.begin(); i!=localFinalStates.end(); i++)
        {
            for( set<int>::iterator s = lambdaInchidere(*i).begin(); s!=lambdaInchidere(*i).end();s++ )
                for (set <int>::iterator j = delta[{*s, w[contor+1]}].begin(); j != delta[{*s, w[contor+1]}].end(); j++)
                    auxiliar.insert(*j);
        }
        n--;
        contor++;
        localFinalStates.clear();

        for(set <int>::iterator i=auxiliar.begin(); i!=auxiliar.end(); i++)
            localFinalStates.insert(*i);
        auxiliar.clear();

    }
    return localFinalStates;
}


int main()
{
	LNFA M;

	ifstream fin("lambda-nfa.txt");
	fin >> M;
	fin.close();

	string s;
	cin>>s;

	set <int> lastState = M.deltaStar(M.getInitialState(), s);

	if (M.IntersectionNotEmpty(lastState))
	{
		cout << "Cuvant acceptat";
	}
	else
	{
		cout << "Cuvant respins";
	}

	return 0;
}
