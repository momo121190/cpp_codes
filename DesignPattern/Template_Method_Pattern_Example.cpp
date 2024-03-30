#include <iostream>
using namespace std;

class Forme
{
public:
    void affiche() // ici, non virtuelle
    {
        afficheNature();
        cout << "-- Coordonnées = " << x << " " << y << endl;
        afficheAutresInfos();
    }
    virtual void afficheNature() = 0; // à redéfinir obligatoirement

protected:
    virtual void afficheAutresInfos() {} // version par défaut si pas redéfinie

private:
    int x, y; // pour éviter pb acces dans classes dérivées
};

class Point : public Forme
{
public:
    Point(int x, int y) { this->x = x; this->y = y; }
    virtual void afficheNature() { cout << "Je suis un Point" << endl; }
};

class Cercle : public Forme
{
public:
    Cercle(int x, int y, double r) { this->x = x; this->y = y; this->r = r; }
    virtual void afficheNature() { cout << "Je suis un Cercle" << endl; }
    virtual void afficheAutresInfos()
    {
        cout << "-- Rayon = " << r << endl;
    }

private:
    double r;
};

int main()
{
    Point *p = new Point(2, 5);
    p->affiche();

    Cercle *c = new Cercle(3, 8, 4.5);
    c->affiche();

    return 0;
}

