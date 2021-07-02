struct Config
{

    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool gcd_unlocked = false;
    bool avg_spell_dmg = false;
    bool additional_data = false;

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

    Food food;
    Flask flask;
    Elixir battle_elixir;
    Elixir guardian_elixir;
    WeaponOil weapon_oil;
    Drums drums;
    Potion potion;
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

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    int innervate = 0;
    bool mana_tide = true;
    bool bloodlust = true;
    bool power_infusion = false;
    bool symbol_of_hope = false;

    double regen_mana_at = 20;
    double regen_stop_at = 30;
    int regen_ab_count = 3;
    RegenRotation regen_rotation = ROTATION_FB;
    double ab_haste_stop = 0;

    double mana_tide_at = 10;
    double bloodlust_at = 20;
    double power_infusion_at = 20;
    double symbol_of_hope_at = 0;
    double icy_veins_at = 1;
    double cold_snap_at = 20;
    double trinket1_at = 1;
    double trinket1_reuse_at = 0;
    double trinket2_at = 20;
    double trinket2_reuse_at = 0;
    double evocation_at = 0;
    double berserking_at = 1;
    double arcane_power_at = 1;
    double presence_of_mind_at = 0;
    double combustion_at = 0;
    double drums_at = 1;
    double potion_at = 21;
    double potion_reuse_at = 0;
    double conjured_at = 21;
    double conjured_reuse_at = 0;

};
