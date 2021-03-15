#include <string>

namespace dot
{

    enum ID : int
    {
        IGNITE = 12848,
    };


    class Dot
    {

    public:
        std::string name;
        int ticks = 1;
        int tick = 0;
        double dmg;
        double t_interval = 3;

        Dot(double _dmg)
        {
            dmg = _dmg;
        }

    };

    class Ignite : public Dot
    {

    public:
        Ignite(double _dmg) : Dot(_dmg)
        {
            name = "Ignite";
            t_interval = 2;
            ticks = 2;
        }

    };

    class Fireball : public Dot
    {

    public:
        Fireball(double _dmg) : Dot(_dmg)
        {
            name = "Fireball";
            t_interval = 2;
            ticks = 4;
        }

    };

}