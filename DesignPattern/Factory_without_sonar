#include <iostream>

class logo
{
public:
    virtual void affiche() = 0;
};

class logoCercle : public logo
{
public:
    void affiche()
    {
        std::cout << "logo circulaire" << std::endl;
    }
};

class logoRectangle : public logo
{
public:
    void affiche()
    {
        std::cout << "logo rectangle" << std::endl;
    }
};

class FabriqueLogoHasard
{
    public :
    logo *getlogo()
    {
        int n=rand();
        if (n<(RAND_MAX/2) ){return new logoCercle();}
        else {return new logoRectangle();}

    }
};

int main()
{
    // Vous pouvez créer une instance de logoCercle et appeler la fonction affiche().
    FabriqueLogoHasard fab ;
    for ( int i=0 ; i<4 ; i++)
    {
        logo *l =fab.getlogo();
        l->affiche();
        delete l ;
    }
    return 0;
}
