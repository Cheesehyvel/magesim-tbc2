#include <string>

namespace spell
{

    enum ID : int
    {
        ARCANE_BLAST,
        ARCANE_MISSILES,
        FROSTBOLT,
        LIGHTNING_CAPACITOR,
    };

    enum Result : int
    {
        HIT,
        CRIT,
        MISS
    };

    class Spell
    {

    public:
        ID id;
        std::string name;
        double cost = 0;
        double min_dmg = 0;
        double max_dmg = 0;
        double cast_time = 0;
        double coeff = 1;
        bool channeling = false;
        bool proc = false;
        int ticks = 0;
        School school;

        Result result;
        double dmg = 0;
        double actual_cost = 0;
        int tick = 0;
        bool done = false;

    };

    class ArcaneBlast : public Spell
    {

    public:
        ArcaneBlast()
        {
            id = ARCANE_BLAST;
            name = "Arcane Blast";
            cost = 195;
            min_dmg = 648;
            max_dmg = 752;
            cast_time = 2.5;
            coeff = 2.5/3.5;
            school = SCHOOL_ARCANE;
        }

    };

    class ArcaneMissiles : public Spell
    {

    public:
        ArcaneMissiles()
        {
            id = ARCANE_MISSILES;
            name = "Arcane Missiles";
            cost = 785;
            min_dmg = 280;
            max_dmg = 280;
            cast_time = 5;
            coeff = 3.5/5;
            school = SCHOOL_ARCANE;
            channeling = true;
            ticks = 5;
        }

    };

    class Frostbolt : public Spell
    {

    public:
        Frostbolt()
        {
            id = FROSTBOLT;
            name = "Frostbolt";
            cost = 330;
            min_dmg = 600;
            max_dmg = 647;
            cast_time = 3;
            coeff = 3/3.5 * 0.95;
            school = SCHOOL_FROST;
        }

    };

    class LightningCapacitor : public Spell{

    public:
        LightningCapacitor()
        {
            id = LIGHTNING_CAPACITOR;
            name = "Lightning Capacitor";
            cost = 0;
            min_dmg = 694;
            max_dmg = 807;
            cast_time = 0;
            school = SCHOOL_NONE;
            coeff = 0;
            proc = true;
        }
    };

}