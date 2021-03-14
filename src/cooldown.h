namespace cooldown
{

    enum ID : int
    {
        NONE = 0,
        POTION = 1,
        MANA_GEM = 2,
        TRINKET1 = 3,
        TRINKET2 = 4,
        EVOCATION = 12051,
        COLD_SNAP = 11958,
        BERSERKING = 20554,
        ICY_VEINS = 12472,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        QUAGMIRRANS_EYE = 33370,
        UNSTABLE_CURRENTS = 38348,
        ETERNAL_SAGE = 35084,
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

    class ManaGem : public Cooldown
    {

    public:
        ManaGem()
        {
            id = MANA_GEM;
            duration = 120;
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

}