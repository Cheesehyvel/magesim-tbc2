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
        MANA_TIDE,
        SPELLSTRIKE,
        EYE_OF_MAGTHERIDON,
        RESTRAINED_ESSENCE,
        QUAGMIRRANS_EYE,
        UNSTABLE_CURRENTS,
        LIGHTNING_CAPACITOR,
        BERSERKING,
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
            name = "Silver Crescent";
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

    class Spellstrike : public Buff
    {

    public:
        Spellstrike()
        {
            id = SPELLSTRIKE;
            name = "Lesser Spell Blasting";
            duration = 10;
        }

    };

    class EyeOfMagtheridon : public Buff
    {

    public:
        EyeOfMagtheridon()
        {
            id = EYE_OF_MAGTHERIDON;
            name = "Recurring Power";
            duration = 10;
        }

    };

    class RestrainedEssence : public Buff
    {

    public:
        RestrainedEssence()
        {
            id = RESTRAINED_ESSENCE;
            name = "Essence of Sapphiron";
            duration = 20;
        }

    };

    class QuagmirransEye : public Buff
    {

    public:
        QuagmirransEye()
        {
            id = QUAGMIRRANS_EYE;
            name = "Spell Haste";
            duration = 6;
        }

    };

    class UnstableCurrents : public Buff
    {

    public:
        UnstableCurrents()
        {
            id = UNSTABLE_CURRENTS;
            name = "Unstable Currents";
            duration = 15;
        }

    };

    class LightningCapacitor : public Buff
    {

    public:
        LightningCapacitor()
        {
            id = LIGHTNING_CAPACITOR;
            name = "Lightning Capacitor";
            duration = 200;
            max_stacks = 3;
        }

    };

    class Berserking : public Buff
    {

    public:
        Berserking()
        {
            id = BERSERKING;
            name = "Berserking";
            duration = 10;
        }

    };

}