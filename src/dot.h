namespace dot
{

    enum ID : int
    {
        IGNITE = 12848,
        FIREBALL = 27070,
        PYROBLAST = 33938,
    };


    class Dot
    {

    public:
        ID id;
        std::string name;
        int ticks = 1;
        int tick = 0;
        double dmg;
        double t_interval = 3;
        bool stackable = false;

        Dot(double _dmg = 0)
        {
            dmg = _dmg;
        }

        virtual void onTick()
        {
            tick++;
        }

        virtual void stack(double _dmg){}

    };

    class Ignite : public Dot
    {

    public:
        Ignite(double _dmg = 0) : Dot(_dmg)
        {
            id = IGNITE;
            name = "Ignite";
            t_interval = 2;
            ticks = 2;
            stackable = true;
        }

        void stack(double _dmg)
        {
            if (tick == 0)
                dmg+= _dmg;
            else if (tick == 1)
                dmg = _dmg + round(dmg/2);
            else
                dmg = _dmg;

            tick = 0;
        }

    };

    class Fireball : public Dot
    {

    public:
        Fireball(double _dmg = 0) : Dot(_dmg)
        {
            id = FIREBALL;
            name = "Fireball";
            t_interval = 2;
            ticks = 4;
            dmg = 21;
        }

    };

    class Pyroblast : public Dot
    {

    public:
        Pyroblast(double _dmg = 0) : Dot(_dmg)
        {
            id = PYROBLAST;
            name = "Pyroblast";
            t_interval = 3;
            ticks = 4;
            dmg = 89;
        }

    };

}