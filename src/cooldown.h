namespace cooldown
{

    enum ID : int
    {
        NONE = 0,
        POTION = 1,
        CONJURED = 2,
        TRINKET1 = 3,
        TRINKET2 = 4,
        EVOCATION = 12051,
        COLD_SNAP = 11958,
        BERSERKING = 20554,
        ICY_VEINS = 12472,
        COMBUSTION = 29977,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        QUAGMIRRANS_EYE = 33370,
        UNSTABLE_CURRENTS = 38348,
        ETERNAL_SAGE = 35084,
        SPELL_BLASTING = 25906,
        DRUMS = 35476,
        LIGHTNING_CAPACITOR = 37657,
        INSIGHTFUL_EARTHSTORM = 27521,
        MARK_OF_DEFIANCE = 33511,
    };


    class Cooldown
    {

    public:
        ID id;
        double duration;

        Cooldown(ID _id = NONE, double _duration = 0)
        {
            if (_id)
                id = _id;
            if (_duration > 0)
                duration = _duration;
        }

    };

    class Potion : public Cooldown
    {

    public:
        Potion()
        {
            id = POTION;
            duration = 120;
        }

    };

    class Conjured : public Cooldown
    {

    public:
        Conjured(double _duration = 120)
        {
            id = CONJURED;
            duration = _duration;
        }

    };

    class IcyVeins : public Cooldown
    {

    public:
        IcyVeins()
        {
            id = ICY_VEINS;
            duration = 180;
        }

    };

    class ArcanePower : public Cooldown
    {

    public:
        ArcanePower()
        {
            id = ARCANE_POWER;
            duration = 180;
        }

    };

    class PresenceOfMind : public Cooldown
    {

    public:
        PresenceOfMind()
        {
            id = PRESENCE_OF_MIND;
            duration = 180;
        }

    };

    class Combustion : public Cooldown
    {

    public:
        Combustion()
        {
            id = COMBUSTION;
            duration = 180;
        }

    };

    class Berserking : public Cooldown
    {

    public:
        Berserking()
        {
            id = BERSERKING;
            duration = 180;
        }

    };

    class ColdSnap : public Cooldown
    {

    public:
        ColdSnap()
        {
            id = COLD_SNAP;
            duration = 480;
        }

    };

    class Evocation : public Cooldown
    {

    public:
        Evocation()
        {
            id = EVOCATION;
            duration = 480;
        }

    };

    class QuagmirransEye : public Cooldown
    {

    public:
        QuagmirransEye()
        {
            id = QUAGMIRRANS_EYE;
            duration = 45;
        }

    };

    class UnstableCurrents : public Cooldown
    {

    public:
        UnstableCurrents()
        {
            id = UNSTABLE_CURRENTS;
            duration = 45;
        }

    };

    class EternalSage : public Cooldown
    {

    public:
        EternalSage()
        {
            id = ETERNAL_SAGE;
            duration = 45;
        }

    };

    /* Wrath of Cenarius ring.
    Set cooldown = buff duration because it is unable to refresh itself */
    class SpellBlasting : public Cooldown
    {

    public:
        SpellBlasting()
        {
            id = SPELL_BLASTING;
            duration = 10;
        }

    };

    class Drums : public Cooldown
    {

    public:
        Drums()
        {
            id = DRUMS;
            duration = 120;
        }

    };

    class LightningCapacitor : public Cooldown
    {

    public:
        LightningCapacitor()
        {
            id = LIGHTNING_CAPACITOR;
            duration = 2.5;
        }

    };

    class InsightfulEarthstorm : public Cooldown
    {

    public:
        InsightfulEarthstorm()
        {
            id = INSIGHTFUL_EARTHSTORM;
            duration = 15;
        }

    };

    class MarkOfDefiance : public Cooldown
    {

    public:
        MarkOfDefiance()
        {
            id = MARK_OF_DEFIANCE;
            duration = 15;
        }

    };

}