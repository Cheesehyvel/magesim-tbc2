namespace spell
{

    enum ID : int
    {
        ARCANE_BLAST = 30451,
        ARCANE_MISSILES = 38699,
        FROSTBOLT = 27072,
        FIREBALL = 27070,
        SCORCH = 27074,
        PYROBLAST = 33938,
        LIGHTNING_CAPACITOR = 28785,
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
        bool binary = false;
        int ticks = 0;
        School school;

        Result result;
        double dmg = 0;
        double resist = 0;
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
            min_dmg = 668;
            max_dmg = 772;
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
            cost = 740;
            min_dmg = 265;
            max_dmg = 265;
            cast_time = 5;
            coeff = 5/3.5;
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
            binary = true;
        }

    };

    class Fireball : public Spell
    {

    public:
        Fireball()
        {
            id = FIREBALL;
            name = "Fireball";
            cost = 425;
            min_dmg = 649;
            max_dmg = 821;
            cast_time = 3.5;
            coeff = 1;
            school = SCHOOL_FIRE;
        }

    };

    class Scorch : public Spell
    {

    public:
        Scorch()
        {
            id = SCORCH;
            name = "Scorch";
            cost = 180;
            min_dmg = 305;
            max_dmg = 361;
            cast_time = 1.5;
            coeff = 1.5/3.5;
            school = SCHOOL_FIRE;
        }

    };

    class Pyroblast : public Spell
    {

    public:
        Pyroblast()
        {
            id = PYROBLAST;
            name = "Pyroblast";
            cost = 500;
            min_dmg = 939;
            max_dmg = 1191;
            cast_time = 6;
            coeff = 1.15;
            school = SCHOOL_FIRE;
        }

    };

    class LightningCapacitor : public Spell
    {

    public:
        LightningCapacitor()
        {
            id = LIGHTNING_CAPACITOR;
            name = "Lightning Capacitor";
            cost = 0;
            min_dmg = 694;
            max_dmg = 807;
            cast_time = 0;
            school = SCHOOL_NATURE;
            coeff = 0;
            proc = true;
        }
    };

}