namespace buff
{

    enum ID : int
    {
        ARCANE_BLAST = 30451,
        ARCANE_MADNESS = 37444,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        ICY_VEINS = 12472,
        COMBUSTION = 29977,
        BLOODLUST = 2825,
        POWER_INFUSION = 10060,
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
        SCRYERS_BLOODGEM = 35337,
        CRYSTAL_TALISMAN = 32355,
        PENDANT_VIOLET_EYE = 29601,
        ENLIGHTENMENT = 35095,
        VENGEANCE_ILLIDARI = 33662,
        ETERNAL_SAGE = 35084,
        SPELL_BLASTING = 25906,
        SKULL_GULDAN = 40396,
        SHRUNKEN_HEAD = 43712,
        NAARU_SLIVER = 45042,
        DRUMS_OF_BATTLE = 35476,
        DRUMS_OF_WAR = 35475,
        DRUMS_OF_RESTORATION = 35478,
        DESTRUCTION_POTION = 28508,
        FLAME_CAP = 28714,
        DARKMOON_CRUSADE = 39438,
        SPELL_POWER_BONUS = 37619,
        ASHTONGUE_TALISMAN = 40482,
        CALL_OF_THE_NEXUS = 34320,
    };


    class Buff
    {

    public:
        ID id;
        std::string name;
        double duration;
        int stacks = 1;
        int max_stacks = 1;
        bool hidden = false;

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

    class Combustion : public Buff
    {

    public:
        Combustion()
        {
            id = COMBUSTION;
            name = "Combustion";
            duration = 2000;
            max_stacks = 20;
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

    class PowerInfusion : public Buff
    {

    public:
        PowerInfusion()
        {
            id = POWER_INFUSION;
            name = "Power Infusion";
            duration = 15;
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

    class ScryersBloodgem : public Buff
    {

    public:
        ScryersBloodgem()
        {
            id = SCRYERS_BLOODGEM;
            name = "Spell Power";
            duration = 15;
        }

    };

    class CrystalTalisman : public Buff
    {

    public:
        CrystalTalisman()
        {
            id = CRYSTAL_TALISMAN;
            name = "Focused Power";
            duration = 20;
        }

    };

    class PendantVioletEye : public Buff
    {

    public:
        PendantVioletEye()
        {
            id = PENDANT_VIOLET_EYE;
            name = "Enlightenment";
            duration = 20;
            hidden = true;
        }

    };

    class Enlightenment : public Buff
    {

    public:
        Enlightenment()
        {
            id = ENLIGHTENMENT;
            name = "Enlightenment";
            duration = 20;
            max_stacks = 100;
        }

    };

    class VengeanceIllidari : public Buff
    {

    public:
        VengeanceIllidari()
        {
            id = VENGEANCE_ILLIDARI;
            name = "Arcane Energy";
            duration = 15;
        }

    };

    class EternalSage : public Buff
    {

    public:
        EternalSage()
        {
            id = ETERNAL_SAGE;
            name = "Band of Eternal Sage";
            duration = 10;
        }

    };

    // Buff associated with Wrath of Cenarius ring
    class SpellBlasting : public Buff
    {

    public:
        SpellBlasting()
        {
            id = SPELL_BLASTING;
            name = "Spell Blasting";
            duration = 10;
        }

    };

    class SkullGuldan : public Buff
    {

    public:
        SkullGuldan()
        {
            id = SKULL_GULDAN;
            name = "Fel Infusion";
            duration = 20;
        }

    };

    class ShrunkenHead : public Buff
    {

    public:
        ShrunkenHead()
        {
            id = SHRUNKEN_HEAD;
            name = "Mojo Madness";
            duration = 20;
        }

    };

    class NaaruSliver : public Buff
    {

    public:
        NaaruSliver()
        {
            id = NAARU_SLIVER;
            name = "Power Circle";
            duration = 15;
        }

    };

    class DrumsOfBattle : public Buff
    {

    public:
        DrumsOfBattle()
        {
            id = DRUMS_OF_BATTLE;
            name = "Drums of Battle";
            duration = 30;
        }

    };

    class DrumsOfWar : public Buff
    {

    public:
        DrumsOfWar()
        {
            id = DRUMS_OF_WAR;
            name = "Drums of War";
            duration = 30;
        }

    };

    class DrumsOfRestoration : public Buff
    {

    public:
        DrumsOfRestoration()
        {
            id = DRUMS_OF_RESTORATION;
            name = "Drums of Restoration";
            duration = 30;
        }

    };

    class DestructionPotion : public Buff
    {

    public:
        DestructionPotion()
        {
            id = DESTRUCTION_POTION;
            name = "Destruction";
            duration = 15;
        }

    };

    class FlameCap : public Buff
    {

    public:
        FlameCap()
        {
            id = FLAME_CAP;
            name = "Flame Cap";
            duration = 60;
        }

    };

    class DarkmoonCrusade : public Buff
    {

    public:
        DarkmoonCrusade()
        {
            id = DARKMOON_CRUSADE;
            name = "Aura of the Crusade";
            duration = 10;
            max_stacks = 10;
        }

    };

    class SpellPowerBonus : public Buff
    {

    public:
        SpellPowerBonus()
        {
            id = SPELL_POWER_BONUS;
            name = "Spell Power Bonus";
            duration = 15;
        }

    };

    class AshtongueTalisman : public Buff
    {

    public:
        AshtongueTalisman()
        {
            id = ASHTONGUE_TALISMAN;
            name = "Insight of the Ashtongue";
            duration = 5;
        }

    };

    class CallOfTheNexus : public Buff
    {

    public:
        CallOfTheNexus()
        {
            id = CALL_OF_THE_NEXUS;
            name = "Call of the Nexus";
            duration = 10;
        }

    };

}