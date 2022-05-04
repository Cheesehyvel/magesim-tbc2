namespace spell
{

    enum ID : int
    {
        ARCANE_BLAST = 30451,
        ARCANE_EXPLOSION = 27082,
        ARCANE_EXPLOSION1 = 1449,
        ARCANE_MISSILES = 38699,
        FROSTBOLT = 27072,
        FROSTBOLT11 = 25304,
        FIREBALL = 27070,
        SCORCH = 27074,
        FIRE_BLAST = 27079,
        PYROBLAST = 33938,
        LIGHTNING_CAPACITOR = 28785,
        ENGULFING_SHADOWS = 21978,
        ARCANE_BOLT = 45429,
        TIMBALS_SHADOW_BOLT = 45055,
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
        bool aoe = false;
        double aoe_cap = 0;
        int ticks = 0;
        School school;

        Result result;
        double dmg = 0;
        double resist = 0;
        double actual_cost = 0;
        int tick = 0;
        bool done = false;

        int misses = 0;
        int hits = 0;
        int crits = 0;

        double avgDmg()
        {
            return (min_dmg + max_dmg)/2.0;
        }

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

    class ArcaneExplosion : public Spell
    {

    public:
        ArcaneExplosion()
        {
            id = ARCANE_EXPLOSION;
            name = "Arcane Explosion";
            cost = 545;
            min_dmg = 377;
            max_dmg = 407;
            cast_time = 0;
            coeff = 1.5/3.5/2;
            school = SCHOOL_ARCANE;
            aoe = true;
            aoe_cap = 10180;
        }

    };

    class ArcaneExplosion1 : public Spell
    {

    public:
        ArcaneExplosion1()
        {
            id = ARCANE_EXPLOSION1;
            name = "Arcane Explosion (Rank 1)";
            cost = 75;
            min_dmg = 34;
            max_dmg = 38;
            cast_time = 0;
            coeff = 0.064;
            school = SCHOOL_ARCANE;
            aoe = true;
            aoe_cap = 1000; // Unknown aoe cap, so this is just a complete guess
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

    class Frostbolt11 : public Spell
    {

    public:
        Frostbolt11()
        {
            id = FROSTBOLT11;
            name = "Frostbolt (Rank 11)";
            cost = 290;
            min_dmg = 527;
            max_dmg = 568;
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

    class FireBlast : public Spell
    {

    public:
        FireBlast()
        {
            id = FIRE_BLAST;
            name = "Fire Blast";
            cost = 465;
            min_dmg = 664;
            max_dmg = 786;
            cast_time = 0;
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

    class EngulfingShadows : public Spell
    {

    public:
        EngulfingShadows()
        {
            id = ENGULFING_SHADOWS;
            name = "Engulfing Shadows";
            cost = 0;
            min_dmg = 100;
            max_dmg = 100;
            cast_time = 0;
            school = SCHOOL_SHADOW;
            coeff = 0;
            proc = true;
        }
    };

    // Shattered Sun Pendant of Acumen (Sunwell Neck: Scryer)
    class ArcaneBolt : public Spell
    {

    public:
        ArcaneBolt()
        {
            id = ARCANE_BOLT;
            name = "Arcane Bolt";
            cost = 0;
            min_dmg = 333;
            max_dmg = 367;
            cast_time = 0;
            school = SCHOOL_ARCANE;
            coeff = 0;
            proc = true;
        }
    };

    class TimbalsShadowBolt : public Spell
    {

    public:
        TimbalsShadowBolt()
        {
            id = TIMBALS_SHADOW_BOLT;
            name = "Shadow Bolt";
            cost = 0;
            min_dmg = 285;
            max_dmg = 475;
            cast_time = 0;
            school = SCHOOL_SHADOW;
            coeff = 0;
            proc = true;
        }
    };

}