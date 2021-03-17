#include <string>
#include <random>

enum RegenRotation : int
{
    ROTATION_FB,
    ROTATION_AMFB,
    ROTATION_SC,
    ROTATION_SCFB,
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

enum School : int
{
    SCHOOL_NONE,
    SCHOOL_ARCANE,
    SCHOOL_FROST,
    SCHOOL_FIRE,
    SCHOOL_NATURE,
};

enum Spec : int
{
    SPEC_ARCANE,
    SPEC_FIRE,
};

enum MetaGem : int {
    META_NONE = 0,
    META_CHAOTIC_SKYFIRE = 34220,
    META_EMBER_SKYFIRE = 35503,
    META_INSIGHTFUL_EARTHSTORM = 25901,
};

enum Trinket : int
{
    TRINKET_NONE = 0,
    TRINKET_SERPENT_COIL = 30720,
    TRINKET_SILVER_CRESCENT = 29370,
    TRINKET_EYE_OF_MAGTHERIDON = 28789,
    TRINKET_RESTRAINED_ESSENCE = 23046,
    TRINKET_QUAGMIRRANS_EYE = 27683,
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
    TRINKET_SHRUNKEN_HEAD = 33829,
    TRINKET_SORCERERS_ALCHEMIST_STONE = 35749,
    TRINKET_ALCHEMIST_STONE = 13503,
    TRINKET_NAARU_SLIVER = 34429,
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
};

enum Elixir : int
{
    ELIXIR_NONE = 0,
    ELIXIR_MAJOR_MAGEBLOOD = 22840,
    ELIXIR_DRAENIC_WISDOM = 32067,
    ELIXIR_GREATER_ARCANE = 13454,
    ELIXIR_ADEPTS = 28103,
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
    OIL_SUPERIOR_MANA = 22521,
};

enum Potion : int
{
    POTION_NONE = 0,
    POTION_MANA = 22832,
    POTION_DESTRUCTION = 22839,
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
    EVENT_CD_EXPIRE,
    EVENT_VAMPIRIC_TOUCH,
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
    LOG_WAIT
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

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
    std::string log;
};

struct SimulationsResult
{
    double min_dps, avg_dps, max_dps;
    int iterations;
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
    return rating / 15.75;
}

template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to)
{
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}