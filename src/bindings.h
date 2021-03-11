#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>


std::shared_ptr<Config> allocConfig()
{
    return std::make_shared<Config>();
}

std::shared_ptr<Player> allocPlayer(std::shared_ptr<Config> config)
{
    return std::make_shared<Player>(config);
}

SimulationResult runSimulation(std::shared_ptr<Config> config, std::shared_ptr<Player> player)
{
    std::shared_ptr<Simulation> sim(new Simulation(config, player));
    player->ready();
    sim->logging = true;

    return sim->run();
}

SimulationsResult runSimulations(std::shared_ptr<Config> config, std::shared_ptr<Player> player, int iterations)
{
    std::shared_ptr<Simulation> sim(new Simulation(config, player));
    player->ready();

    return sim->runMultiple(iterations);
}

EMSCRIPTEN_BINDINGS(my_module) {

    emscripten::enum_<RegenRotation>("RegenRotation")
        .value("ROTATION_FB", ROTATION_FB)
        .value("ROTATION_AMFB", ROTATION_AMFB);

    emscripten::enum_<Spec>("Spec")
        .value("SPEC_ARCANE", SPEC_ARCANE);

    emscripten::enum_<Race>("Race")
        .value("RACE_BLOOD_ELF", RACE_BLOOD_ELF)
        .value("RACE_DRAENEI", RACE_DRAENEI)
        .value("RACE_GNOME", RACE_GNOME)
        .value("RACE_HUMAN", RACE_HUMAN)
        .value("RACE_TROLL", RACE_TROLL)
        .value("RACE_UNDEAD", RACE_UNDEAD);

    emscripten::enum_<LogType>("LogType")
        .value("LOG_NONE", LOG_NONE)
        .value("LOG_SPELL", LOG_SPELL)
        .value("LOG_MANA", LOG_MANA)
        .value("LOG_BUFF", LOG_BUFF)
        .value("LOG_DEBUG", LOG_DEBUG);

    emscripten::class_<Config>("Config")
        .smart_ptr<std::shared_ptr<Config>>("Config")
        .property("duration", &Config::duration)
        .property("vampiric_touch_regen", &Config::vampiric_touch_regen)
        .property("misery", &Config::misery)
        .property("curse_of_elements", &Config::curse_of_elements)
        .property("judgement_of_the_crusader", &Config::judgement_of_the_crusader)
        .property("judgement_of_wisdom", &Config::judgement_of_wisdom)
        .property("vampiric_touch", &Config::vampiric_touch)

        .property("totem_of_wrath", &Config::totem_of_wrath)
        .property("wrath_of_air", &Config::wrath_of_air)
        .property("mana_spring", &Config::mana_spring)
        .property("arcane_intellect", &Config::arcane_intellect)
        .property("divine_spirit", &Config::divine_spirit)
        .property("improved_divine_spirit", &Config::improved_divine_spirit)
        .property("blessing_of_kings", &Config::blessing_of_kings)
        .property("blessing_of_wisdom", &Config::blessing_of_wisdom)
        .property("mark_of_the_wild", &Config::mark_of_the_wild)
        .property("moonkin_aura", &Config::moonkin_aura)
        .property("mage_armor", &Config::mage_armor)
        .property("molten_armor", &Config::molten_armor)

        .property("spell_dmg_food", &Config::spell_dmg_food)
        .property("spell_crit_food", &Config::spell_crit_food)
        .property("brilliant_wizard_oil", &Config::brilliant_wizard_oil)
        .property("flask_of_supreme_power", &Config::flask_of_supreme_power)
        .property("flask_of_blinding_light", &Config::flask_of_blinding_light)
        .property("adepts_elixir", &Config::adepts_elixir)
        .property("elixir_of_draenic_wisdom", &Config::elixir_of_draenic_wisdom)
        .property("drums", &Config::drums)

        .property("t5_2set", &Config::t5_2set)
        .property("t5_4set", &Config::t5_4set)
        .property("spellfire_set", &Config::spellfire_set)
        .property("chaotic_skyfire", &Config::chaotic_skyfire)
        .property("serpent_coil", &Config::serpent_coil)
        .property("silver_crescent", &Config::silver_crescent)

        .property("innervate", &Config::innervate)
        .property("mana_tide", &Config::mana_tide)
        .property("bloodlust", &Config::bloodlust)

        .property("regen_mana_at", &Config::regen_mana_at)
        .property("regen_rotation", &Config::regen_rotation)
        .property("mana_tide_at", &Config::mana_tide_at)
        .property("bloodlust_at", &Config::bloodlust_at)
        .property("cooldowns_at", &Config::cooldowns_at)
        ;

    emscripten::function("allocConfig", &allocConfig);

    emscripten::value_object<Stats>("Stats")
        .field("intellect", &Stats::intellect)
        .field("spirit", &Stats::spirit)
        .field("crit", &Stats::crit)
        .field("hit", &Stats::hit)
        .field("haste", &Stats::haste)
        .field("spell_power", &Stats::spell_power)
        .field("spell_power_arcane", &Stats::spell_power_arcane)
        .field("spell_power_frost", &Stats::spell_power_frost)
        .field("spell_power_fire", &Stats::spell_power_fire)
        ;

    emscripten::class_<Player>("Player")
        .smart_ptr<std::shared_ptr<Player>>("Player")
        .property("stats", &Player::stats)
        .property("spec", &Player::spec)
        .property("race", &Player::race)
        .function("setStats", &Player::setStats)
        .function("loadTalentsFromString", &Player::loadTalentsFromString);

    emscripten::function("allocPlayer", &allocPlayer);

    emscripten::value_object<SimulationResult>("SimulationResult")
        .field("dmg", &SimulationResult::dmg)
        .field("t", &SimulationResult::t)
        .field("dps", &SimulationResult::dps)
        .field("log", &SimulationResult::log)
        ;

    emscripten::value_object<SimulationsResult>("SimulationsResult")
        .field("min_dps", &SimulationsResult::min_dps)
        .field("avg_dps", &SimulationsResult::avg_dps)
        .field("max_dps", &SimulationsResult::max_dps)
        .field("iterations", &SimulationsResult::iterations);

    emscripten::function("runSimulation", &runSimulation);
    emscripten::function("runSimulations", &runSimulations);
}

#endif