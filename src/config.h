struct Config
{

    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool gcd_unlocked = false;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    int targets = 1;

    bool misery = true;
    bool curse_of_elements = true;
    bool malediction = false;
    bool judgement_of_the_crusader = false;
    bool judgement_of_wisdom = true;
    bool vampiric_touch = true;
    int vampiric_touch_regen = 50;

    bool totem_of_wrath = false;
    bool wrath_of_air = true;
    bool mana_spring = true;
    bool improved_mana_spring = false;
    bool arcane_intellect = true;
    bool divine_spirit = true;
    bool improved_divine_spirit = false;
    bool blessing_of_kings = true;
    bool blessing_of_wisdom = true;
    bool mark_of_the_wild = true;
    bool moonkin_aura = false;
    bool mage_armor = true;
    bool molten_armor = false;
    bool inspiring_presence = false;
    bool fire_vulnerability = false;
    bool maintain_fire_vulnerability = false;
    bool winters_chill = false;
    bool drums_friend = false;
    bool atiesh_mage = false;
    bool atiesh_warlock = false;
    bool eye_of_the_night = false;
    bool chain_of_the_twilight_owl = false;
    bool jade_pendant_of_blasting = false;
    bool scroll_of_spirit = false;
    bool kreegs = false;
    bool bloodthistle = false;
    bool scourgebane = false;
    bool imp_sanctity = false;
    int ferocious_inspiration = 0;

    Food food;
    Flask flask;
    Elixir battle_elixir;
    Elixir guardian_elixir;
    WeaponOil weapon_oil;
    Drums drums;
    Potion potion;
    Potion first_potion;
    Conjured conjured;

    bool tirisfal_2set = true;
    bool tirisfal_4set = true;
    bool tempest_2set = false;
    bool tempest_4set = false;
    bool spellfire_set = false;
    bool spellstrike_set = false;
    bool eternal_sage = false;
    bool wrath_of_cenarius = false;
    bool mana_etched_4set = false;
    bool blade_of_wizardry = false;
    bool robe_elder_scribes = false;
    bool blade_of_eternal_darkness = false;
    bool sunwell_neck_aldor = false;
    bool sunwell_neck_scryer = false;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    int innervate = 0;
    int evo_ticks = 0;
    bool mana_tide = true;
    bool bloodlust = true;
    bool power_infusion = false;
    bool symbol_of_hope = false;

    MainRotation main_rotation = MAIN_ROTATION_AB;
    RegenRotation regen_rotation = REGEN_ROTATION_FB;
    double regen_mana_at = 20;
    double regen_stop_at = 30;
    int regen_ab_count = 3;
    double ab_haste_stop = 0;
    bool fire_blast_weave = false;

    bool cc_am_queue = false;
    bool cc_am_repeat = false;

    bool bis_ashtongue = false;

    std::vector<double> trinket1_t;
    std::vector<double> trinket2_t;
    std::vector<double> presence_of_mind_t;
    std::vector<double> arcane_power_t;
    std::vector<double> icy_veins_t;
    std::vector<double> cold_snap_t;
    std::vector<double> combustion_t;
    std::vector<double> berserking_t;
    std::vector<double> mana_tide_t;
    std::vector<double> bloodlust_t;
    std::vector<double> power_infusion_t;
    std::vector<double> drums_t;
    std::vector<double> innervate_t;
    std::vector<double> potion_t;
    std::vector<double> conjured_t;
    std::vector<double> filler_start_t;
    std::vector<double> filler_end_t;

    double symbol_of_hope_at = 0;
    double evocation_at = 0;

};
