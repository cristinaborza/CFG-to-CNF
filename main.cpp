#include <bits/stdc++.h>

//#define se second
//#define fi first

using namespace std;

const string Lamba = "$";

class Productie {
    string alfa; ///neterminal
    vector <string> beta; ///reuniune de terminali si neterminali

public:
    Productie () {
    }

    Productie (string alfa, vector <string> beta) {
        this -> alfa = alfa;
        this -> beta = beta;
    }

    bool operator< (const Productie other) const;
    bool operator== (const Productie other) const;

    string getAlfa() { return alfa; }
    vector <string> getBeta() { return beta; }
};

bool Productie::operator< (const Productie other) const {
    if (alfa < other.alfa) {
        return true;
    }

    if (alfa > other.alfa) {
        return false;
    }

    int sz = min (beta.size(), other.beta.size());
    for (int i = 0; i < sz; ++ i) {
        if (beta[i] < other.beta[i]) {
            return true;
        }

        if (beta[i] > other.beta[i]) {
            return false;
        }
    }

    return false;
}

bool Productie::operator== (const Productie other) const {
    if (alfa != other.alfa) {
        return false;
    }

    if (beta.size() != other.beta.size()) return false;
    int sz = beta.size();
    for (int i = 0; i < sz; ++ i) {
        if (beta[i] != other.beta[i]) return false;
    }

    return true;
}

class CFG {
    set <string> N; /// neterminalele
    set <string> Sigma; /// alfabetul
    string S; ///simbolul de start
    set <Productie> P; ///multimea productiilor

public:
    CFG () {
    }

    CFG (set <string> N, set <string> Sigma, string S, set <Productie> P) {
        this -> N = N;
        this -> Sigma = Sigma;
        this -> S = S;
        this -> P = P;
    }

    set <string> getN() { return N; }
    set <string> getSigma() { return Sigma; }
    string getS() { return S; }
    set <Productie> getP() { return P; }

    void setN(vector <string>);
    void setSigma(vector <string>);
    void setS(string);
    void setP(vector <pair <string, vector <string> > >);

    void setN(set <string>);
    void setSigma(set <string>);
    void setP(set <Productie>);

    friend ostream& operator <<(ostream&, CFG);
};

ostream& operator <<(ostream& g, CFG G) {
    g << "Neterminalele sunt: ";
    for (auto net: G.N) {
        g << net << ", ";
    }

    g << '\n' << "Terminalele sunt: ";
    for  (auto term: G.Sigma) {
        g << term << ", ";
    }
    g << '\n' << "Simbolul de start este: " << G.S << '\n';
    g << "Productiile sunt:\n";
    for (auto prod: G.P) {
        g << prod.getAlfa() << " -> ";
        vector <string> v = prod.getBeta();
        for (auto it: v) g << it << " ";
        g << '\n';
    }

    return g;
}

void CFG::setN(vector <string> s) {
    for (auto it: s) N.insert (it);
}

void CFG::setSigma(vector <string> s) {
    for (auto it: s) Sigma.insert (it);
}

void CFG::setS (string s) {
    S = s;
}

void CFG::setP (vector <pair <string, vector <string> > > s) {
    for (auto it: s) {
        Productie p(it.first, it.second);
        P.insert (p);
    }
}

void CFG::setN(set <string> N) { this -> N = N; }
void CFG::setSigma(set <string> Sigma) { this -> Sigma = Sigma; }
void CFG::setP(set <Productie> P) { this -> P = P; }

CFG createCFG () {
    /// creaza o gramatica independenta de context
    CFG G;

    vector <string> N = {"S", "B", "D", "F", "G", "H"};
    G.setN(N); /// neterminalele: S, B, D, F, G, H


    vector <string> Sigma = {"a", "b", "c", "d", "e", "f", "g"};
    G.setSigma(Sigma); /// terminalele: a, b, c, d, e, f, g

    G.setS ("S"); /// "S" - simbolul de start

    vector <pair <string, vector <string> > > P = {{"S", {"a", "B", "c", "D", "e", "F"}}, {"S", {"H", "F"}}, {"S", {"H", "B", "c"}}, {"B", {"b"}}, {"B", {Lamba}}, {"D", {"d"}}, {"D", {Lamba}}, {"F", {"G"}}, {"G", {"f"}}, {"G", {"g"}}, {"H", {Lamba}}};
    G.setP(P);  ///productiile  S -> aBcDeF | HF | HBc
                ///             B -> b | $
                ///             D -> d | $
                ///             F -> G
                ///             G -> f | g
                ///             H -> $
    return G;
}

bool check1 (Productie P, set <string> N, CFG G) {
    ///verifica ca priductia (u -> v) sa contina in v doar terminali si neterminali din N
    vector <string> S = P.getBeta();
    set <string> Sigma = G.getSigma();
    for (auto it: S) {
        if (Sigma.find (it) == Sigma.end() && N.find(it) == N.end() && it != Lamba) return false;
    }

    return true;
}

CFG Reducere1(CFG G) {
    set <string> N1;
    set <Productie> P = G.getP();

    bool ok = true;
    while (ok) {
        ok = false;
        for (auto it: P) {
            if (check1(it, N1, G)) {
                string A = it.getAlfa ();
                if (N1.find(A) == N1.end()) {
                    ok = true;
                    N1.insert(A);
                }
            }
        }
    }

    if (N1.find(G.getS()) == N1.end()) {
        cout << "Gramatica nu genereaza niciun cuvant!\n";
        exit(0);
    }

    CFG ans;
    ans.setN(N1);
    ans.setSigma(G.getSigma());
    ans.setS(G.getS());
    set <Productie> ansP;
    for (auto it: P) {
        if (N1.find(it.getAlfa()) != N1.end() && check1 (it, N1, G)) {
            ansP.insert(it);
        }
    }

    ans.setP (ansP);
    return ans;
}

CFG Reducere2(CFG G) {
    ///scot productiile si simbolurile inacesibile

    string S = G.getS();
    set <Productie> P = G.getP();
    set <string> N = G.getN();

    set <string> N1;
    N1.insert(S);

    bool ok = true;
    while (ok) {
        ok = false;
        for (auto prod: P) {
            string A = prod.getAlfa();
            if (N1.find(A) != N1.end()) {
                vector <string> v = prod.getBeta();
                for (auto it: v) {
                    if (N.find(it) != N.end() && N1.find(it) == N1.end()) {
                        ok = true;
                        N1.insert(it);
                    }
                }
            }
        }
    }

    CFG ans;
    ans.setN(N1);
    ans.setSigma(G.getSigma());
    ans.setS(G.getS());
    set <Productie> ansP;
    for (auto it: P) {
        if (N1.find(it.getAlfa()) != N1.end() && check1 (it, N1, G)) {
            ansP.insert(it);
        }
    }

    ans.setP (ansP);
    return ans;
}

CFG scoate(CFG G, string A, bool type) {
    set <Productie> newP;

    set <Productie> P = G.getP();
    for (auto prod: P) {
        string alfa = prod.getAlfa();
        vector <string> beta = prod.getBeta();

        if (alfa != A) {
            bool ok = true;
            for (auto it: beta) {
                if (it == A) {
                    ok = false;
                    break;
                }
            }

            if (ok) {
                newP.insert (prod);
                continue;
            }

            vector <string> newBeta;
            for (auto it: beta) {
                if (it != A) {
                    newBeta.push_back (it);
                }
            }

            ///vreau sa adaug productia din care din membrul drept scot netrminaul A
            if (newBeta.size() > 0) {
                /// daca mai exista si alti neterminali in afara de A
                Productie newProd(alfa, newBeta);
                newP.insert(newProd);
            }

            else {
                ///daca in productie am in membrul drept doar neterminalul A
                if (type == 0) {
                    ///daca neterminalul A nu avea nicio productie in afara de lambda-productia mea
                    ///atunci adaug o noua productie, cea in care in locul lui A, pun lambda
                    newBeta.push_back (Lamba);
                    Productie newProd(alfa, newBeta);
                    newP.insert(newProd);
                }
            }

            if (type == 1) newP.insert(prod);
        }

        else {
            if (beta.size() == 1 && beta[0] == Lamba) {
                ///atunci e fix productia pe care vreau sa o scot
                continue;
            }

            bool ok = true;
            for (auto it: beta) {
                if (it == A) {
                    ok = false;
                    break;
                }
            }

            if (ok) {
                newP.insert(prod);
                continue;
            }

            vector <string> newBeta;
            for (auto it: beta) {
                if (it != A) {
                    newBeta.push_back(it);
                }
            }

            ///vreau sa adaug productia din care din membrul drept scot netrminaul A
            Productie newProd(alfa, newBeta);
            newP.insert(newProd);
            newP.insert(prod);
        }
    }

    CFG ans(G);

    set <string> N = G.getN();
    if (type == 0) {
        N.erase(A);
    }

    ans.setP(newP);
    ans.setN(N);

    return ans;
}

CFG scoateSlambda(CFG G) {
    set <Productie> P = G.getP();
    string S = G.getS();
    bool ok = true;
    for (auto prod: P) {
        string alfa = prod.getAlfa();
        vector <string> beta = prod.getBeta();

        if (alfa == S && beta.size() == 1 && beta[0] == Lamba) {
            ok = false;
            break;
        }
    }

    if (ok) return G;

    string newS = "S1";
    G.setS(newS);

    vector <string> aux = {S};
    Productie p(newS, aux);
    P.insert(p);
    G.setP(P);

    return G;
}

CFG LambdaProductii(CFG G) {
    ///elimin lambda-productiile
    ///daca S are o lambda-productie o scot, inserez un nou terminal S1, in adaug productia S1 -> S
    G = scoateSlambda(G);

    bool ok = true;
    while (ok) {
        ok = false;

        set <Productie> P = G.getP();
        map <string, int> mp;

        for (auto prod: P) ++ mp[prod.getAlfa()];
        for (auto prod: P) {
            string alfa = prod.getAlfa();
            vector <string> beta = prod.getBeta();
            if (beta.size() == 1 && beta[0] == Lamba) {
                /// atunci am o lambda productie
                ok = true;
                if(mp[alfa] == 1) G = scoate(G, alfa, 0);
                else G = scoate(G, alfa, 1);
                break;
            }
        }
    }

    return G;
}

CFG Reducere (CFG G) {
    /// algoritmul de reducere
    /// scot simbolurile si productiile neutilizabile
    G = Reducere1 (G);

    ///scot simbolurile si productiile inaccesibile
    G = Reducere2 (G);

    return G;
}

CFG eliminaRedenumire(Productie p, CFG G) {
    ///elimina productia prod, care este o redenumire
    string s = p.getBeta()[0];
    set <Productie> P = G.getP();
    set <Productie> newP;

    bool ok = false;
    for (auto prod: P) {
        vector <string> v = prod.getBeta();
        if (prod == p) continue;

        for (auto it: v) {
            if (it == s) {
                ok = true;
                break;
            }
        }

        if (ok) break;
    }

    for (auto prod: P) {
        string alfa = prod.getAlfa();
        vector <string> beta = prod.getBeta();

        if (alfa == p.getAlfa() && beta == p.getBeta()) {
            for (auto prod1: P) {
                if (prod1.getAlfa() == s) {
                    Productie pr(p.getAlfa(), prod1.getBeta());
                    newP.insert(pr);
                }
            }

            continue;
        }

        if (ok == false && alfa == s) {
            continue;
        }

        newP.insert(prod);
    }

    CFG ans(G);

    set <string> newN;
    set <string> N = G.getN();
    for (auto s: N) {
        for (auto prod: newP) {
            if (prod.getAlfa() == s) {
                newN.insert(s);
                break;
            }
        }
    }

    ans.setN(newN);
    ans.setP(newP);

    return ans;
}

CFG Redenumire(CFG G) {
    ///elimin redenumirile
    set <Productie> P = G.getP();
    set <Productie> newP;
    set <string> N = G.getN();

    ///scot productiile de tipul A -> A
    for (auto prod: P) {
        string alfa = prod.getAlfa();
        vector <string> beta = prod.getBeta();

        if (beta.size() == 1 && beta[0] == alfa) continue;
        newP.insert (prod);
    }

    G.setP(newP);

    ///scot redenumirile, cate o redenumire la un pas
    bool ok = true;
    while (ok) {
        set <Productie> P = G.getP();

        ok = false;
        for (auto prod: P) {
            string alfa = prod.getAlfa();
            vector <string> beta = prod.getBeta();

            if (beta.size() == 1 && N.find(beta[0]) != N.end()) {
                ///atunci productia mea e o redenumire, si trebuie scoasa
                ok = true;
                G = eliminaRedenumire(prod, G);
                break;
            }
        }
    }

    return G;
}

CFG scoateTerminale(CFG G) {
    set <Productie> P = G.getP();
    set <Productie> newP;
    set <string> Sigma = G.getSigma();
    set <string> T;
    set <string> N = G.getN();

    for (auto prod: P) {
        vector <string> v = prod.getBeta();
        if (v.size() <= 1) {
            newP.insert(prod);
            continue;
        }

        bool ok = true;
        for (int i = 0; i < (int)v.size(); ++ i) {
            string it = v[i];
            if (Sigma.find(it) != Sigma.end()) {
                ok = false;
                string newNet = "X" + it;
                N.insert(newNet);


                vector <string> aux = {it};
                Productie p(newNet, aux);
                P.insert(p);

                v[i] = newNet;
            }
        }

        if (ok) newP.insert(prod);
        else {
            Productie p(prod.getAlfa(), v);
            newP.insert(p);
        }
    }

    G.setN(N);
    G.setP(newP);

    return G;
}

string transf(int x) {
    string ans;
    while (x) {
        char c = x % 10 + (int)'0';
        ans.push_back(c);
        x /= 10;
    }

    reverse(ans.begin(), ans.end());
    return ans;
}

CFG spargeCuvant(CFG G) {
    set <Productie> P = G.getP();
    set <Productie> newP = G.getP();
    set <string> newN = G.getN();

    int idx = 0;
    for (auto prod: P) {
        ///verific daca productia mea are in membrul drept mai mult de 2 neterminale
        vector <string> v = prod.getBeta();
        if (v.size() > 2) {
            newP.erase(prod);

            string newNet = "Y" + transf(++idx);
            newN.insert(newNet);

            vector <string> aux = {v[0], newNet};
            Productie p(prod.getAlfa(), aux);
            newP.insert(p);

            string lst = newNet;
            for (int i = 1; i < (int)v.size() - 2; ++ i) {
                string newNet = "Y" + transf(++idx);
                newN.insert(newNet);

                aux = {v[i], newNet};
                Productie p(lst, aux);
                newP.insert(p);

                lst = newNet;
            }

            aux = {v[v.size() - 2], v[v.size() - 1]};
            Productie p1(lst, aux);
            newP.insert(p1);
        }
    }

    G.setN(newN);
    G.setP(newP);
    return G;
}

CFG CFG_to_CNF (CFG G) {
    /// rescrie gramtica mea in forma normala Chomsky
    /// algoritmul de reducere
    G = Reducere (G);

    /// elimin lambda - productiile
    G = LambdaProductii (G);

    /// elimin redenumirile
    G = Redenumire (G);

    /// din nou algoritmul de reducere
    G = Reducere (G);

    /// pana acum am obtinut o gramatica redusa
    /// adaug neterminale noi pentru terminalele din cuvinte
    G = scoateTerminale (G);


    /// aduag neterminale pentru cuvintele mai lungi de 2
    G = spargeCuvant (G);
    /// gramatica obtinuta e in forma normala Chomsky
    return G;
}

int main() {
    CFG G = createCFG();
    CFG ans = CFG_to_CNF(G);
    cout << ans << '\n';
    return 0;
}
