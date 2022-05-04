const double CC_SNAPSHOT_WINDOW = 0.01;

enum RegenRotation : int
{
    REGEN_ROTATION_FB,
    REGEN_ROTATION_AMFB,
    REGEN_ROTATION_SC,
    REGEN_ROTATION_SCFB,
    REGEN_ROTATION_AMSC,
    REGEN_ROTATION_AMAM,
    REGEN_ROTATION_FB11,
};

enum MainRotation: int
{
    MAIN_ROTATION_AB,
    MAIN_ROTATION_AM,
    MAIN_ROTATION_SC,
    MAIN_ROTATION_FIB,
    MAIN_ROTATION_FRB,
    MAIN_ROTATION_AE,
};

enum Race : int
{
    RACE_BLOOD_ELF,
    RACE_DRAENEI,
    RACE_GNOME,
    RACE_HUMAN,
    RACE_TROLL,
    RACE_UNDEAD
};

enum Faction : int
{
    FACTION_HORDE,
    FACTION_ALLIANCE,
};

enum School : int
{
    SCHOOL_NONE,
    SCHOOL_ARCANE,
    SCHOOL_FROST,
    SCHOOL_FIRE,
    SCHOOL_NATURE,
    SCHOOL_SHADOW,
};

enum MetaGem : int {
    META_NONE = 0,
    META_CHAOTIC_SKYFIRE = 34220,
    META_EMBER_SKYFIRE = 35503,
    META_INSIGHTFUL_EARTHSTORM = 25901,
    META_MYSTICAL_SKYFIRE = 25893,
};

enum Trinket : int
{
    TRINKET_NONE = 0,
    TRINKET_SERPENT_COIL = 30720,
    TRINKET_SILVER_CRESCENT = 29370,
    TRINKET_ESSENCE_MARTYR = 29376,
    TRINKET_EYE_OF_MAGTHERIDON = 28789,
    TRINKET_RESTRAINED_ESSENCE = 23046,
    TRINKET_QUAGMIRRANS_EYE = 27683,
    TRINKET_XIRIS_GIFT = 29179,
    TRINKET_UNSTABLE_CURRENTS = 30626,
    TRINKET_LIGHTNING_CAPACITOR = 28785,
    TRINKET_MQG = 19339,
    TRINKET_BLUE_DRAGON = 19288,
    TRINKET_MARK_OF_DEFIANCE = 27924,
    TRINKET_SCRYERS_BLOODGEM = 29132,
    TRINKET_CRYSTAL_TALISMAN = 25620,
    TRINKET_PENDANT_VIOLET_EYE = 28727,
    TRINKET_VENGEANCE_ILLIDARI = 28040,
    TRINKET_SKULL_GULDAN = 32483,
    TRINKET_CRIMSON_SERPENT = 35700,
    TRINKET_SHRUNKEN_HEAD = 33829,
    TRINKET_SORCERERS_ALCHEMIST_STONE = 35749,
    TRINKET_ALCHEMIST_STONE = 13503,
    TRINKET_NAARU_SLIVER = 34429,
    TRINKET_DARKMOON_CRUSADE = 31856,
    TRINKET_ASHTONGUE_TALISMAN = 32488,
    TRINKET_NEXUS_HORN = 28418,
    TRINKET_BURST_OF_KNOWLEDGE = 11832,
    TRINKET_SMOKING_PIPE = 38290,
    TRINKET_TIMBALS_FOCUSING_CRYSTAL = 34470,
};

enum Food : int
{
    FOOD_NONE = 0,
    FOOD_SPELL_POWER = 27657,
    FOOD_SPELL_CRIT = 33825,
};

enum Flask : int
{
    FLASK_NONE = 0,
    FLASK_SUPREME_POWER = 13512,
    FLASK_BLINDING_LIGHT = 22861,
    FLASK_PURE_DEATH = 22866,
    FLASK_DISTILLED_WISDOM = 13511,
    FLASK_CHROMATIC_WONDER = 33208,
};

enum Elixir : int
{
    ELIXIR_NONE = 0,
    ELIXIR_MAJOR_MAGEBLOOD = 22840,
    ELIXIR_DRAENIC_WISDOM = 32067,
    ELIXIR_GREATER_ARCANE = 13454,
    ELIXIR_ADEPTS = 28103,
    ELIXIR_MAJOR_FIREPOWER = 22833,
    ELIXIR_MASTERY = 28104,
};

enum Drums : int
{
    DRUMS_NONE = 0,
    DRUMS_OF_WAR = 29528,
    DRUMS_OF_RESTORATION = 29531,
    DRUMS_OF_BATTLE = 29529,
};

enum WeaponOil : int
{
    OIL_NONE = 0,
    OIL_BRILLIANT_WIZARD = 20749,
    OIL_SUPERIOR_WIZARD = 22522,
    OIL_BLESSED_WIZARD = 23123,
    OIL_SUPERIOR_MANA = 22521,
};

enum Potion : int
{
    POTION_NONE = 0,
    POTION_MANA = 22832,
    POTION_DESTRUCTION = 22839,
    POTION_FEL_MANA = 31677,
};

enum Conjured : int
{
    CONJURED_NONE = 0,
    CONJURED_MANA_GEM = 22044,
    CONJURED_FLAME_CAP = 22788,
};

enum EventType : int
{
    EVENT_CAST,
    EVENT_SPELL,
    EVENT_MANA_REGEN,
    EVENT_MANA_GAIN,
    EVENT_BUFF_GAIN,
    EVENT_BUFF_EXPIRE,
    EVENT_DEBUFF_GAIN,
    EVENT_DEBUFF_EXPIRE,
    EVENT_DOT,
    EVENT_CD_GAIN,
    EVENT_CD_EXPIRE,
    EVENT_VAMPIRIC_TOUCH,
    EVENT_DRUMS,
    EVENT_INNERVATE,
    EVENT_POWER_INFUSION,
    EVENT_MANA_SPRING,
    EVENT_WAIT
};

enum LogType : int
{
    LOG_NONE,
    LOG_SPELL,
    LOG_MANA,
    LOG_BUFF,
    LOG_DOT,
    LOG_DEBUG,
    LOG_WAIT,
    LOG_GCD_CAP,
};

struct LogEntry
{
    LogType type;
    std::string text;
    double t;
    double mana;
    double mana_percent;
    int dmg;
};

struct SpellStats
{
    std::string name;
    int casts = 0;
    int misses = 0;
    int hits = 0;
    int crits = 0;
    double dmg = 0;
    double min_dmg = 0;
    double max_dmg = 0;
};

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
    double evocated_at;
    double regened_at;
    double t_gcd_capped;
    std::string log;
    std::string spells;
};

struct SimulationsResult
{
    double min_dps, avg_dps, max_dps;
    int iterations;
    std::string histogram;
    std::string all_results;
    std::string stats;
};

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
    return rating / 15.77;
}

// Global rng generator
thread_local static std::mt19937 g_rng(std::random_device{}());

template<typename Numeric>
Numeric random(Numeric from, Numeric to)
{
    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(g_rng, typename dist_type::param_type{from, to});
}

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
