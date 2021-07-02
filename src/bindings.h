#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>


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
        .value("ROTATION_AMFB", ROTATION_AMFB)
        .value("ROTATION_SC", ROTATION_SC)
        .value("ROTATION_SCFB", ROTATION_SCFB)
        .value("ROTATION_AMSC", ROTATION_AMSC)
        ;

    emscripten::enum_<Spec>("Spec")
        .value("SPEC_ARCANE", SPEC_ARCANE)
        .value("SPEC_FIRE", SPEC_FIRE)
        .value("SPEC_FROST", SPEC_FROST)
        ;

    emscripten::enum_<Race>("Race")
        .value("RACE_BLOOD_ELF", RACE_BLOOD_ELF)
        .value("RACE_DRAENEI", RACE_DRAENEI)
        .value("RACE_GNOME", RACE_GNOME)
        .value("RACE_HUMAN", RACE_HUMAN)
        .value("RACE_TROLL", RACE_TROLL)
        .value("RACE_UNDEAD", RACE_UNDEAD);

    emscripten::enum_<Trinket>("Trinket")
        .value("TRINKET_NONE", TRINKET_NONE)
        .value("TRINKET_SERPENT_COIL", TRINKET_SERPENT_COIL)
        .value("TRINKET_SILVER_CRESCENT", TRINKET_SILVER_CRESCENT)
        .value("TRINKET_EYE_OF_MAGTHERIDON", TRINKET_EYE_OF_MAGTHERIDON)
        .value("TRINKET_RESTRAINED_ESSENCE", TRINKET_RESTRAINED_ESSENCE)
        .value("TRINKET_QUAGMIRRANS_EYE", TRINKET_QUAGMIRRANS_EYE)
        .value("TRINKET_XIRIS_GIFT", TRINKET_XIRIS_GIFT)
        .value("TRINKET_UNSTABLE_CURRENTS", TRINKET_UNSTABLE_CURRENTS)
        .value("TRINKET_LIGHTNING_CAPACITOR", TRINKET_LIGHTNING_CAPACITOR)
        .value("TRINKET_MQG", TRINKET_MQG)
        .value("TRINKET_BLUE_DRAGON", TRINKET_BLUE_DRAGON)
        .value("TRINKET_MARK_OF_DEFIANCE", TRINKET_MARK_OF_DEFIANCE)
        .value("TRINKET_SCRYERS_BLOODGEM", TRINKET_SCRYERS_BLOODGEM)
        .value("TRINKET_CRYSTAL_TALISMAN", TRINKET_CRYSTAL_TALISMAN)
        .value("TRINKET_PENDANT_VIOLET_EYE", TRINKET_PENDANT_VIOLET_EYE)
        .value("TRINKET_VENGEANCE_ILLIDARI", TRINKET_VENGEANCE_ILLIDARI)
        .value("TRINKET_SKULL_GULDAN", TRINKET_SKULL_GULDAN)
        .value("TRINKET_SHRUNKEN_HEAD", TRINKET_SHRUNKEN_HEAD)
        .value("TRINKET_SORCERERS_ALCHEMIST_STONE", TRINKET_SORCERERS_ALCHEMIST_STONE)
        .value("TRINKET_ALCHEMIST_STONE", TRINKET_ALCHEMIST_STONE)
        .value("TRINKET_NAARU_SLIVER", TRINKET_NAARU_SLIVER)
        .value("TRINKET_DARKMOON_CRUSADE", TRINKET_DARKMOON_CRUSADE)
        .value("TRINKET_ASHTONGUE_TALISMAN", TRINKET_ASHTONGUE_TALISMAN)
        .value("TRINKET_NEXUS_HORN", TRINKET_NEXUS_HORN)
        ;

    emscripten::enum_<MetaGem>("MetaGem")
        .value("META_NONE", META_NONE)
        .value("META_CHAOTIC_SKYFIRE", META_CHAOTIC_SKYFIRE)
        .value("META_EMBER_SKYFIRE", META_EMBER_SKYFIRE)
        .value("META_INSIGHTFUL_EARTHSTORM", META_INSIGHTFUL_EARTHSTORM)
        .value("META_MYSTICAL_SKYFIRE", META_MYSTICAL_SKYFIRE)
        ;

    emscripten::enum_<Food>("Food")
        .value("FOOD_NONE", FOOD_NONE)
        .value("FOOD_SPELL_POWER", FOOD_SPELL_POWER)
        .value("FOOD_SPELL_CRIT", FOOD_SPELL_CRIT);

    emscripten::enum_<Flask>("Flask")
        .value("FLASK_NONE", FLASK_NONE)
        .value("FLASK_SUPREME_POWER", FLASK_SUPREME_POWER)
        .value("FLASK_BLINDING_LIGHT", FLASK_BLINDING_LIGHT)
        .value("FLASK_PURE_DEATH", FLASK_PURE_DEATH)
        .value("FLASK_DISTILLED_WISDOM", FLASK_DISTILLED_WISDOM);

    emscripten::enum_<Elixir>("Elixir")
        .value("ELIXIR_NONE", ELIXIR_NONE)
        .value("ELIXIR_MAJOR_MAGEBLOOD", ELIXIR_MAJOR_MAGEBLOOD)
        .value("ELIXIR_DRAENIC_WISDOM", ELIXIR_DRAENIC_WISDOM)
        .value("ELIXIR_GREATER_ARCANE", ELIXIR_GREATER_ARCANE)
        .value("ELIXIR_ADEPTS", ELIXIR_ADEPTS)
        .value("ELIXIR_MAJOR_FIREPOWER", ELIXIR_MAJOR_FIREPOWER)
        ;

    emscripten::enum_<Drums>("Drums")
        .value("DRUMS_NONE", DRUMS_NONE)
        .value("DRUMS_OF_WAR", DRUMS_OF_WAR)
        .value("DRUMS_OF_RESTORATION", DRUMS_OF_RESTORATION)
        .value("DRUMS_OF_BATTLE", DRUMS_OF_BATTLE);

    emscripten::enum_<WeaponOil>("WeaponOil")
        .value("OIL_NONE", OIL_NONE)
        .value("OIL_BRILLIANT_WIZARD", OIL_BRILLIANT_WIZARD)
        .value("OIL_SUPERIOR_WIZARD", OIL_SUPERIOR_WIZARD)
        .value("OIL_BLESSED_WIZARD", OIL_BLESSED_WIZARD)
        .value("OIL_SUPERIOR_MANA", OIL_SUPERIOR_MANA);

    emscripten::enum_<Potion>("Potion")
        .value("POTION_NONE", POTION_NONE)
        .value("POTION_MANA", POTION_MANA)
        .value("POTION_DESTRUCTION", POTION_DESTRUCTION);

    emscripten::enum_<Conjured>("Conjured")
        .value("CONJURED_NONE", CONJURED_NONE)
        .value("CONJURED_MANA_GEM", CONJURED_MANA_GEM)
        .value("CONJURED_FLAME_CAP", CONJURED_FLAME_CAP);

    emscripten::enum_<LogType>("LogType")
        .value("LOG_NONE", LOG_NONE)
        .value("LOG_SPELL", LOG_SPELL)
        .value("LOG_MANA", LOG_MANA)
        .value("LOG_BUFF", LOG_BUFF)
        .value("LOG_DEBUG", LOG_DEBUG);

    emscripten::class_<Config>("Config")
        .smart_ptr<std::shared_ptr<Config>>("Config")
        .property("duration", &Config::duration)
        .property("duration_variance", &Config::duration_variance)
        .property("rng_seed", &Config::rng_seed)
        .property("gcd_unlocked", &Config::gcd_unlocked)
        .property("avg_spell_dmg", &Config::avg_spell_dmg)
        .property("additional_data", &Config::additional_data)

        .property("misery", &Config::misery)
        .property("curse_of_elements", &Config::curse_of_elements)
        .property("malediction", &Config::malediction)
        .property("judgement_of_the_crusader", &Config::judgement_of_the_crusader)
        .property("judgement_of_wisdom", &Config::judgement_of_wisdom)
        .property("vampiric_touch", &Config::vampiric_touch)
        .property("vampiric_touch_regen", &Config::vampiric_touch_regen)

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
        .property("inspiring_presence", &Config::inspiring_presence)
        .property("fire_vulnerability", &Config::fire_vulnerability)
        .property("maintain_fire_vulnerability", &Config::maintain_fire_vulnerability)
        .property("winters_chill", &Config::winters_chill)
        .property("drums_friend", &Config::drums_friend)
        .property("atiesh_mage", &Config::atiesh_mage)
        .property("atiesh_warlock", &Config::atiesh_warlock)
        .property("eye_of_the_night", &Config::eye_of_the_night)
        .property("chain_of_the_twilight_owl", &Config::chain_of_the_twilight_owl)
        .property("jade_pendant_of_blasting", &Config::jade_pendant_of_blasting)
        .property("scroll_of_spirit", &Config::scroll_of_spirit)
        .property("kreegs", &Config::kreegs)

        .property("food", &Config::food)
        .property("flask", &Config::flask)
        .property("battle_elixir", &Config::battle_elixir)
        .property("guardian_elixir", &Config::guardian_elixir)
        .property("weapon_oil", &Config::weapon_oil)
        .property("drums", &Config::drums)
        .property("potion", &Config::potion)
        .property("conjured", &Config::conjured)

        .property("tirisfal_2set", &Config::tirisfal_2set)
        .property("tirisfal_4set", &Config::tirisfal_4set)
        .property("tempest_2set", &Config::tempest_2set)
        .property("tempest_4set", &Config::tempest_4set)
        .property("spellfire_set", &Config::spellfire_set)
        .property("spellstrike_set", &Config::spellstrike_set)
        .property("eternal_sage", &Config::eternal_sage)
        .property("wrath_of_cenarius", &Config::wrath_of_cenarius)
        .property("mana_etched_4set", &Config::mana_etched_4set)
        .property("blade_of_wizardry", &Config::blade_of_wizardry)
        .property("robe_elder_scribes", &Config::robe_elder_scribes)

        .property("trinket1", &Config::trinket1)
        .property("trinket2", &Config::trinket2)
        .property("meta_gem", &Config::meta_gem)

        .property("innervate", &Config::innervate)
        .property("mana_tide", &Config::mana_tide)
        .property("bloodlust", &Config::bloodlust)
        .property("power_infusion", &Config::power_infusion)
        .property("symbol_of_hope", &Config::symbol_of_hope)

        .property("regen_mana_at", &Config::regen_mana_at)
        .property("regen_stop_at", &Config::regen_stop_at)
        .property("regen_ab_count", &Config::regen_ab_count)
        .property("regen_rotation", &Config::regen_rotation)
        .property("ab_haste_stop", &Config::ab_haste_stop)

        .property("mana_tide_at", &Config::mana_tide_at)
        .property("bloodlust_at", &Config::bloodlust_at)
        .property("power_infusion_at", &Config::power_infusion_at)
        .property("symbol_of_hope_at", &Config::symbol_of_hope_at)
        .property("icy_veins_at", &Config::icy_veins_at)
        .property("cold_snap_at", &Config::cold_snap_at)
        .property("trinket1_at", &Config::trinket1_at)
        .property("trinket1_reuse_at", &Config::trinket1_reuse_at)
        .property("trinket2_at", &Config::trinket2_at)
        .property("trinket2_reuse_at", &Config::trinket2_reuse_at)
        .property("evocation_at", &Config::evocation_at)
        .property("berserking_at", &Config::berserking_at)
        .property("arcane_power_at", &Config::arcane_power_at)
        .property("presence_of_mind_at", &Config::presence_of_mind_at)
        .property("combustion_at", &Config::combustion_at)
        .property("drums_at", &Config::drums_at)
        .property("potion_at", &Config::potion_at)
        .property("potion_reuse_at", &Config::potion_reuse_at)
        .property("conjured_at", &Config::conjured_at)
        .property("conjured_reuse_at", &Config::conjured_reuse_at)
        ;

    emscripten::function("allocConfig", &allocConfig);

    emscripten::value_object<Stats>("Stats")
        .field("intellect", &Stats::intellect)
        .field("spirit", &Stats::spirit)
        .field("mp5", &Stats::mp5)
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
        .field("iterations", &SimulationsResult::iterations)
        .field("histogram", &SimulationsResult::histogram)
        .field("all_results", &SimulationsResult::all_results)
        ;

    emscripten::function("runSimulation", &runSimulation);
    emscripten::function("runSimulations", &runSimulations);
}

#endif
