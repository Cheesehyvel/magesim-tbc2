#include <string>

namespace buff
{

    enum ID : int
    {
        ARCANE_BLAST = 30451,
        ARCANE_MADNESS = 37444,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        ICY_VEINS = 12472,
        BLOODLUST = 2825,
        INNERVATE = 29166,
        CLEARCAST = 12536,
        SILVER_CRESCENT = 35163,
        SERPENT_COIL = 30720,
        MANA_TIDE = 39609,
        SPELLSTRIKE = 32106,
        EYE_OF_MAGTHERIDON = 34747,
        RESTRAINED_ESSENCE = 28779,
        QUAGMIRRANS_EYE = 33370,
        UNSTABLE_CURRENTS = 38348,
        LIGHTNING_CAPACITOR = 37657,
        BERSERKING = 20554,
        MQG = 23723,
        BLUE_DRAGON = 23684,
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

    class PresenceOfMind : public Buff
    {

    public:
        PresenceOfMind()
        {
            id = PRESENCE_OF_MIND;
            name = "Presence of Mind";
            duration = 100;
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

    class MindQuickening : public Buff
    {

    public:
        MindQuickening()
        {
            id = MQG;
            name = "MQG";
            duration = 20;
        }

    };

    class BlueDragon : public Buff
    {

    public:
        BlueDragon()
        {
            id = BLUE_DRAGON;
            name = "Aura of the Blue Dragon";
            duration = 15;
        }

    };

}