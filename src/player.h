using namespace std;

class Player
{

public:
    const double base_mana = 2241;

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
        stats.mp5 = 0;
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
        stats.mp5 = 0;
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
        if (config->guardian_elixir == ELIXIR_DRAENIC_WISDOM) {
            stats.intellect+= 30;
            stats.spirit+= 30;
        }
        if (config->mark_of_the_wild) {
            stats.intellect+= 18;
            stats.spirit+= 18;
        }
        if (config->flask == FLASK_DISTILLED_WISDOM)
            stats.intellect+= 65;
        if (config->food == FOOD_SPELL_POWER || config->food == FOOD_SPELL_CRIT)
            stats.spirit+= 20;
        if (config->scroll_of_spirit)
            stats.spirit+= 30;
        if (config->kreegs) {
            stats.spirit+= 25;
            stats.intellect-= 5;
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

        // Mp5
        if (config->guardian_elixir == ELIXIR_MAJOR_MAGEBLOOD)
            stats.mp5+= 16;
        if (config->weapon_oil == OIL_SUPERIOR_MANA)
            stats.mp5+= 14;

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
            stats.spell_power+= 101.0;
        if (config->weapon_oil == OIL_BRILLIANT_WIZARD)
            stats.spell_power+= 36.0;
        if (config->weapon_oil == OIL_SUPERIOR_WIZARD)
            stats.spell_power+= 42.0;
        if (config->weapon_oil == OIL_BLESSED_WIZARD)
            stats.spell_power+= 60.0;
        if (config->food == FOOD_SPELL_POWER)
            stats.spell_power+= 23.0;
        if (config->flask == FLASK_SUPREME_POWER)
            stats.spell_power+= 70.0;
        if (config->flask == FLASK_BLINDING_LIGHT)
            stats.spell_power_arcane+= 80.0;
        if (config->flask == FLASK_PURE_DEATH) {
            stats.spell_power_fire+= 80.0;
            stats.spell_power_frost+= 80.0;
        }
        if (config->battle_elixir == ELIXIR_ADEPTS)
            stats.spell_power+= 24.0;
        if (config->battle_elixir == ELIXIR_GREATER_ARCANE)
            stats.spell_power+= 35.0;
        if (config->battle_elixir == ELIXIR_MAJOR_FIREPOWER)
            stats.spell_power_fire+= 55.0;
        if (config->atiesh_warlock)
            stats.spell_power+= 33.0;
        if (config->eye_of_the_night)
            stats.spell_power+= 34.0;
        if (config->jade_pendant_of_blasting)
            stats.spell_power+= 15.0;

        // Spell crit
        double critrating = 0;
        if (config->judgement_of_the_crusader)
            stats.crit+= 3.0;
        if (config->moonkin_aura)
            stats.crit+= 5.0;
        if (config->totem_of_wrath)
            stats.crit+= 3.0;
        if (config->molten_armor)
            stats.crit+= 3.0;
        if (config->chain_of_the_twilight_owl)
            stats.crit+= 2.0;
        if (config->battle_elixir == ELIXIR_ADEPTS)
            critrating+= 24;
        if (config->weapon_oil == OIL_BRILLIANT_WIZARD)
            critrating+= 14;
        if (config->food == FOOD_SPELL_CRIT)
            critrating+= 20;
        if (config->atiesh_mage)
            critrating+= 28;
        if (critrating > 0)
            stats.crit+= critRatingToChance(critrating);
        if (talents.arcane_instability)
            stats.crit+= 1.0 * talents.arcane_instability;
        stats.crit+= stats.intellect/80.0;

        // Spell hit
        if (config->totem_of_wrath)
            stats.hit+= 3.0;
        if (race == RACE_DRAENEI || config->inspiring_presence)
            stats.hit+= 1.0;
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
        int tree = 0, t = 0, p = 0;

        for (int i=0; i<str.length(); i++) {
            if (str[i] == '-') {
                tree++;
                t = 0;
            }
            else {
                p = str[i] - '0';

                if (tree == 0 && t == 1) talents.arcane_focus = p;
                else if (tree == 0 && t == 5) talents.clearcast = p;
                else if (tree == 0 && t == 7) talents.arcane_impact = p;
                else if (tree == 0 && t == 11) talents.arcane_meditation = p;
                else if (tree == 0 && t == 13) talents.presence_of_mind = p;
                else if (tree == 0 && t == 14) talents.arcane_mind = p;
                else if (tree == 0 && t == 16) talents.arcane_instability = p;
                else if (tree == 0 && t == 17) talents.arcane_potency = p;
                else if (tree == 0 && t == 18) talents.empowered_arcane_missiles = p;
                else if (tree == 0 && t == 19) talents.arcane_power = p;
                else if (tree == 0 && t == 20) talents.spell_power = p;
                else if (tree == 0 && t == 21) talents.mind_mastery = p;
                else if (tree == 1 && t == 0) talents.imp_fireball = p;
                else if (tree == 1 && t == 2) talents.ignite = p;
                else if (tree == 1 && t == 5) talents.incinerate = p;
                else if (tree == 1 && t == 7) talents.pyroblast = p;
                else if (tree == 1 && t == 9) talents.imp_scorch = p;
                else if (tree == 1 && t == 11) talents.master_of_elements = p;
                else if (tree == 1 && t == 12) talents.playing_with_fire = p;
                else if (tree == 1 && t == 13) talents.critical_mass = p;
                else if (tree == 1 && t == 16) talents.fire_power = p;
                else if (tree == 1 && t == 17) talents.pyromaniac = p;
                else if (tree == 1 && t == 18) talents.combustion = p;
                else if (tree == 1 && t == 19) talents.molten_fury = p;
                else if (tree == 1 && t == 20) talents.empowered_fireball = p;
                else if (tree == 2 && t == 1) talents.imp_frostbolt = p;
                else if (tree == 2 && t == 2) talents.elemental_precision = p;
                else if (tree == 2 && t == 3) talents.ice_shards = p;
                else if (tree == 2 && t == 7) talents.piercing_ice = p;
                else if (tree == 2 && t == 8) talents.icy_veins = p;
                else if (tree == 2 && t == 11) talents.frost_channeling = p;
                else if (tree == 2 && t == 14) talents.cold_snap = p;
                else if (tree == 2 && t == 17) talents.winters_chill = p;
                else if (tree == 2 && t == 19) talents.arctic_winds = p;
                else if (tree == 2 && t == 20) talents.empowered_frostbolt = p;

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

        return mps;
    }

    double maxMana()
    {
        return base_mana + stats.intellect*15;
    }

    Faction faction()
    {
        if (race == RACE_GNOME || race == RACE_HUMAN || race == RACE_DRAENEI)
            return FACTION_ALLIANCE;
        return FACTION_HORDE;
    }

};