#include <string>
#include <math.h>
#include <memory>

using namespace std;

class Player
{

public:
    const double base_mana = 1961;

    Stats stats;
    Talents talents;
    shared_ptr<Config> config;
    Race race = RACE_UNDEAD;
    Spec spec = SPEC_ARCANE;

    Player(shared_ptr<Config> _config)
    {
        config = _config;

        setBaseStats();
    }

    // Stats without gear or talents
    void setBaseStats()
    {
        // Undead default
        stats.intellect = 149;
        stats.spirit = 150;
        stats.crit = 0.91;
        stats.hit = 0;
        stats.haste = 0;
        stats.spell_power = 0;
        stats.spell_power_arcane = 0;
        stats.spell_power_frost = 0;

        if (race == RACE_TROLL) {
            stats.intellect = 147;
            stats.spirit = 146;
        }
        if (race == RACE_BLOOD_ELF) {
            stats.intellect = 155;
            stats.spirit = 144;
        }
        if (race == RACE_DRAENEI) {
            stats.intellect = 152;
            stats.spirit = 147;
        }
        if (race == RACE_GNOME) {
            stats.intellect = 155;
            stats.spirit = 145;
        }
        if (race == RACE_HUMAN) {
            stats.intellect = 151;
            stats.spirit = 145;
        }
    }

    // Stats with gear but without talents
    void setDefaultStats()
    {
        stats.intellect = 465;
        stats.spirit = 285;
        stats.crit = 20;
        stats.hit = 6;
        stats.haste = 0;
        stats.spell_power = 1000;
        stats.spell_power_arcane = 50;
        stats.spell_power_frost = 0;
    }

    void quickReady()
    {
        setDefaultTalents();
        setDefaultStats();
        setConfigStats();
    }

    void ready()
    {
        setConfigStats();
    }

    void setStats(Stats _stats)
    {
        stats = _stats;
    }

    void setConfigStats()
    {
        // Attribute additions
        if (config->arcane_intellect)
            stats.intellect+= 40;
        if (config->divine_spirit)
            stats.spirit+= 40;
        if (config->elixir_of_draenic_wisdom) {
            stats.intellect+= 30;
            stats.spirit+= 30;
        }
        if (config->mark_of_the_wild) {
            stats.intellect+= 18;
            stats.spirit+= 18;
        }

        // Attribute multipliers
        if (talents.arcane_mind)
            stats.intellect*= 1.0 + talents.arcane_mind*0.03;
        if (race == RACE_GNOME)
            stats.intellect*= 1.05;
        if (race == RACE_HUMAN)
            stats.spirit*= 1.1;
        if (config->blessing_of_kings) {
            stats.intellect*= 1.1;
            stats.spirit*= 1.1;
        }
        if (config->meta_gem == META_EMBER_SKYFIRE)
            stats.intellect*= 1.02;
        stats.intellect = round(stats.intellect);
        stats.spirit = round(stats.spirit);

        // Spell power
        double int_multi = 0;
        if (talents.mind_mastery)
            int_multi+= talents.mind_mastery*0.05;
        if (config->spellfire_set)
            int_multi+= 0.07;
        if (int_multi > 0)
            stats.spell_power+= round(stats.intellect * int_multi);

        if (config->improved_divine_spirit)
            stats.spell_power+= stats.spirit*0.1;
        if (config->wrath_of_air)
            stats.spell_power+= 102;
        if (config->brilliant_wizard_oil)
            stats.spell_power+= 36;
        if (config->spell_dmg_food)
            stats.spell_power+= 23;
        if (config->flask_of_supreme_power)
            stats.spell_power+= 70;
        if (config->flask_of_blinding_light)
            stats.spell_power_arcane+= 80;
        if (config->adepts_elixir)
            stats.spell_power+= 24;

        // Spell crit
        double critrating = 0;
        if (config->judgement_of_the_crusader)
            stats.crit+= 3;
        if (config->moonkin_aura)
            stats.crit+= 5;
        if (config->totem_of_wrath)
            stats.crit+= 3;
        if (config->molten_armor)
            stats.crit+= 3;
        if (config->adepts_elixir)
            critrating+= 24;
        if (config->brilliant_wizard_oil)
            critrating+= 14;
        if (critrating > 0)
            stats.crit+= critRatingToChance(critrating);
        if (talents.arcane_instability)
            stats.crit+= 3;

        // Spell hit
        if (config->totem_of_wrath)
            stats.hit+= 3;
        if (race == RACE_DRAENEI)
            stats.hit+= 1;
    }

    void setDefaultTalents()
    {
        if (spec == SPEC_ARCANE) {
            talents.arcane_focus = 5;
            talents.arcane_impact = 3;
            talents.arcane_instability = 3;
            talents.clearcast = 5;
            talents.arcane_meditation = 3;
            talents.arcane_potency = 3;
            talents.arcane_mind = 5;
            talents.spell_power = 2;
            talents.presence_of_mind = 1;
            talents.arcane_power = 1;
            talents.mind_mastery = 5;
            talents.empowered_arcane_missiles = 0;
            talents.elemental_precision = 3;
            talents.imp_frostbolt = 5;
            talents.icy_veins = 1;
            talents.cold_snap = 1;
            talents.ice_shards = 5;
            talents.piercing_ice = 3;
            talents.frost_channeling = 3;
        }
    }

    void loadTalentsFromString(std::string str)
    {
        int size = 67;
        int points[size];
        int fire = 23, frost = 45;

        for (int i=0; i<size; i++)
            points[i] = 0;

        int t = 0, p = 0;
        for (int i=0; i<str.length(); i++) {
            if (str[i] == '-') {
                if (t < fire)
                    t = fire;
                else if (t < frost)
                    t = frost;
                else
                    break;
            }
            else {
                p = str[i] - '0';

                if (t == 1) talents.arcane_focus = p;
                else if (t == 5) talents.clearcast = p;
                else if (t == 7) talents.arcane_impact = p;
                else if (t == 11) talents.arcane_meditation = p;
                else if (t == 13) talents.presence_of_mind = p;
                else if (t == 14) talents.arcane_mind = p;
                else if (t == 16) talents.arcane_instability = p;
                else if (t == 17) talents.arcane_potency = p;
                else if (t == 18) talents.empowered_arcane_missiles = p;
                else if (t == 19) talents.arcane_power = p;
                else if (t == 20) talents.spell_power = p;
                else if (t == 21) talents.mind_mastery = p;
                else if (t == 46) talents.imp_frostbolt = p;
                else if (t == 47) talents.elemental_precision = p;
                else if (t == 48) talents.ice_shards = p;
                else if (t == 52) talents.piercing_ice = p;
                else if (t == 53) talents.icy_veins = p;
                else if (t == 56) talents.frost_channeling = p;
                else if (t == 59) talents.cold_snap = p;

                t++;
            }
        }
    }

    double spiritManaPerSecond()
    {
        return 0.001 + stats.spirit*0.009327 * sqrt(stats.intellect);
    }

    double staticManaPerSecond()
    {
        double mps = 0;

        if (stats.mp5)
            mps+= stats.mp5/5.0;
        if (config->blessing_of_wisdom)
            mps+= 49/5.0;
        if (config->mana_spring)
            mps+= 50/5.0;

        return mps;
    }

    double maxMana()
    {
        return base_mana + stats.intellect*15;
    }

};