#pragma once

#include <string>
#include <math.h>

class Player
{

public:
    const double base_mana = 1961;

    Stats baseStats;
    Stats stats;
    Talents talents;
    Settings *settings;
    Race race = RACE_UNDEAD;
    Spec spec = SPEC_ARCANE;

    Player(Settings *_settings)
    {
        settings = _settings;

        setDefault();
    }

    // Stats without gear or talents
    void setBase()
    {
        // Undead default
        baseStats.intellect = 149;
        baseStats.spirit = 150;
        baseStats.crit = 0.91;
        baseStats.hit = 0;
        baseStats.haste = 0;
        baseStats.spell_power = 0;
        baseStats.spell_power_arcane = 0;
        baseStats.spell_power_frost = 0;

        if (race == RACE_TROLL) {
            baseStats.intellect = 147;
            baseStats.spirit = 146;
        }
        if (race == RACE_BLOOD_ELF) {
            baseStats.intellect = 155;
            baseStats.spirit = 144;
        }
        if (race == RACE_DRAENEI) {
            baseStats.intellect = 152;
            baseStats.spirit = 147;
        }
        if (race == RACE_GNOME) {
            baseStats.intellect = 155;
            baseStats.spirit = 145;
        }
        if (race == RACE_HUMAN) {
            baseStats.intellect = 151;
            baseStats.spirit = 145;
        }
    }

    // Stats with gear but without talents
    void setDefault()
    {
        baseStats.intellect = 465;
        baseStats.spirit = 285;
        baseStats.crit = 20;
        baseStats.hit = 6;
        baseStats.haste = 0;
        baseStats.spell_power = 1000;
        baseStats.spell_power_arcane = 50;
        baseStats.spell_power_frost = 0;
    }

    void ready()
    {
        setTalents();
        setStats();
    }

    void copyBaseStats()
    {
        stats.intellect = baseStats.intellect;
        stats.spirit = baseStats.spirit;
        stats.crit = baseStats.crit;
        stats.hit = baseStats.hit;
        stats.haste = baseStats.haste;
        stats.spell_power = baseStats.spell_power;
        stats.spell_power_arcane = baseStats.spell_power_arcane;
        stats.spell_power_frost = baseStats.spell_power_frost;
    }

    void setStats()
    {
        copyBaseStats();

        // Attribute additions
        if (settings->arcane_intellect)
            stats.intellect+= 40;
        if (settings->divine_spirit)
            stats.spirit+= 40;
        if (settings->elixir_of_draenic_wisdom) {
            stats.intellect+= 30;
            stats.spirit+= 30;
        }
        if (settings->mark_of_the_wild) {
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
        if (settings->blessing_of_kings) {
            stats.intellect*= 1.1;
            stats.spirit*= 1.1;
        }
        stats.intellect = round(stats.intellect);
        stats.spirit = round(stats.spirit);

        // Spell power
        double int_multi = 0;
        if (talents.mind_mastery)
            int_multi+= talents.mind_mastery*0.05;
        if (settings->spellfire_set)
            int_multi+= 0.07;
        if (int_multi > 0)
            stats.spell_power+= round(stats.intellect * int_multi);

        if (settings->improved_divine_spirit)
            stats.spell_power+= stats.spirit*0.1;
        if (settings->wrath_of_air)
            stats.spell_power+= 102;
        if (settings->brilliant_wizard_oil)
            stats.spell_power+= 36;
        if (settings->spell_dmg_food)
            stats.spell_power+= 23;
        if (settings->flask_of_supreme_power)
            stats.spell_power+= 70;
        if (settings->flask_of_blinding_light)
            stats.spell_power_arcane+= 80;
        if (settings->adepts_elixir)
            stats.spell_power+= 24;

        // Spell crit
        double critrating = 0;
        if (settings->judgement_of_the_crusader)
            stats.crit+= 3;
        if (settings->moonkin_aura)
            stats.crit+= 5;
        if (settings->totem_of_wrath)
            stats.crit+= 3;
        if (settings->molten_armor)
            stats.crit+= 3;
        if (settings->adepts_elixir)
            critrating+= 24;
        if (settings->brilliant_wizard_oil)
            critrating+= 14;
        if (critrating > 0)
            stats.crit+= critRatingToChance(critrating);
        if (talents.arcane_instability)
            stats.crit+= 3;
        // stats.crit+= stats.intellect/80.0;

        // Spell hit
        if (settings->totem_of_wrath)
            stats.hit+= 3;
    }

    void setTalents()
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
            talents.elemental_precision = 3;
            talents.imp_frostbolt = 5;
            talents.icy_veins = 1;
            talents.cold_snap = 1;
            talents.ice_shards = 5;
            talents.piercing_ice = 3;
            talents.frost_channeling = 3;
        }
    }

    double spiritManaPerSecond()
    {
        return 0.001 + stats.spirit*0.009327 * sqrt(stats.intellect);
    }

    double staticManaPerSecond()
    {
        double mps = 0;

        if (settings->blessing_of_wisdom)
            mps+= 49/5;
        if (settings->mana_spring)
            mps+= 50/5;

        return mps;
    }

    double maxMana()
    {
        return base_mana + stats.intellect*15;
    }

    double critRatingToChance(double rating)
    {
        return rating / 22.08;
    }

    double hitRatingToChance(double rating)
    {
        return rating / 12.62;
    }

    double hasteRatingToHaste(double rating)
    {
        return rating / 15.75;
    }

};