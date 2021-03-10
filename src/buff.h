#include <string>

namespace buff
{

    enum ID : int
    {
        ARCANE_BLAST,
        ARCANE_MADNESS,
        ARCANE_POWER,
        ICY_VEINS,
        PRESENCE_OF_MIND,
        BLOODLUST,
        INNERVATE,
        CLEARCAST,
        SILVER_CRESCENT,
        SERPENT_COIL,
        MANA_TIDE
    };


    class Buff
    {

    public:
        ID id;
        std::string name;
        double duration;
        int stacks = 1;
        int max_stacks = 1;

        int addStack()
        {
            if (stacks < max_stacks)
                stacks++;
            else
                return 0;

            return stacks;
        }

    };

    class ArcaneBlast : public Buff
    {

    public:
        ArcaneBlast()
        {
            id = ARCANE_BLAST;
            name = "Arcane Blast";
            duration = 8;
            max_stacks = 3;
        }

    };

    class ArcaneMadness : public Buff
    {

    public:
        ArcaneMadness()
        {
            id = ARCANE_MADNESS;
            name = "Arcane Madness";
            duration = 6;
        }

    };

    class ArcanePower : public Buff
    {

    public:
        ArcanePower()
        {
            id = ARCANE_POWER;
            name = "Arcane Power";
            duration = 15;
        }

    };

    class IcyVeins : public Buff
    {

    public:
        IcyVeins()
        {
            id = ICY_VEINS;
            name = "Icy Veins";
            duration = 20;
        }

    };

    class SilverCrescent : public Buff
    {

    public:
        SilverCrescent()
        {
            id = SILVER_CRESCENT;
            name = "Icy Veins";
            duration = 20;
        }

    };

    class SerpentCoil : public Buff
    {

    public:
        SerpentCoil()
        {
            id = SERPENT_COIL;
            name = "Serpent Coil";
            duration = 15;
        }

    };

    class Clearcast : public Buff
    {

    public:
        Clearcast()
        {
            id = CLEARCAST;
            name = "Clearcast";
            duration = 15;
        }

    };

    class Bloodlust : public Buff
    {

    public:
        Bloodlust()
        {
            id = BLOODLUST;
            name = "Bloodlust";
            duration = 40;
        }

    };

    class Innervate : public Buff
    {

    public:
        Innervate()
        {
            id = INNERVATE;
            name = "Innervate";
            duration = 20;
        }

    };

    class ManaTide : public Buff
    {

    public:
        ManaTide()
        {
            id = MANA_TIDE;
            name = "Mana Tide";
            duration = 12;
        }

    };

}