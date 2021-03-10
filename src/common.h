#include <string>
#include <random>

enum RegenRotation : int
{
    ROTATION_FB,
    ROTATION_AMFB,
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
    SCHOOL_ARCANE,
    SCHOOL_FROST,
    SCHOOL_FIRE
};

enum Spec : int
{
    SPEC_ARCANE
};

enum EventType : int
{
    EVENT_CAST,
    EVENT_SPELL,
    EVENT_MANA_REGEN,
    EVENT_MANA_GAIN,
    EVENT_BUFF_GAIN,
    EVENT_BUFF_EXPIRE,
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
    LOG_DEBUG
};

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
};

struct SimulationsResult
{
    double min_dps, avg_dps, max_dps;
    int iterations;
};

class LogEntry
{

public:
    LogType type;
    std::string text;
    double t;
    double mana;
    double mana_percent;
    int dmg;

};

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