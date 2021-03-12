struct Config
{

    int duration = 200;
    int vampiric_touch_regen = 50;

    bool misery = true;
    bool curse_of_elements = true;
    bool judgement_of_the_crusader = false;
    bool judgement_of_wisdom = true;
    bool vampiric_touch = true;

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

    bool spell_dmg_food = true;
    bool spell_crit_food = false;
    bool brilliant_wizard_oil = true;
    bool flask_of_supreme_power = true;
    bool flask_of_blinding_light = false;
    bool adepts_elixir = false;
    bool elixir_of_draenic_wisdom = false;
    bool drums = true;

    bool tirisfal_2set = true;
    bool tirisfal_4set = true;
    bool spellfire_set = false;
    bool spellstrike_set = false;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    int innervate = 0;
    bool mana_tide = true;
    bool bloodlust = true;

    double regen_mana_at = 20;
    RegenRotation regen_rotation = ROTATION_FB;
    double mana_tide_at = 10;
    double bloodlust_at = 10;
    double cooldowns_at = 10;

};