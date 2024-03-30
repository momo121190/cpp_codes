#include <iostream>
#include <memory> // Pour std::unique_ptr
#include <random> // Pour la génération de nombres aléatoires

class logo
{
public:
    virtual void affiche() = 0;
    virtual ~logo() {} // Destructeur virtuel
};

class logoCercle : public logo
{
public:
    void affiche() override
    {
        std::cout << "logo circulaire" << std::endl;
    }
};

class logoRectangle : public logo
{
public:
    void affiche() override
    {
        std::cout << "logo rectangle" << std::endl;
    }
};

class FabriqueLogoHasard
{
public:
    std::unique_ptr<logo> getlogo()
    {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_int_distribution<int> distribution(0, 1);
       
        if (distribution(generator) == 0)
        {
            return std::make_unique<logoCercle>();
        }
        else
        {
            return std::make_unique<logoRectangle>();
        }
    }
};

int main()
{
    FabriqueLogoHasard fab;
    for (int i = 0; i < 4; i++)
    {
        std::unique_ptr<logo> l = fab.getlogo();
        l->affiche();
        // La mémoire sera automatiquement gérée lorsque l sortira du scope
    }
    return 0;
}
