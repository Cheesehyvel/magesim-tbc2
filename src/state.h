using namespace std;

class State
{

public:
    double t;
    double t_gcd;
    double t_mana_spent;
    double mana;
    int dmg;
    int regen_cycle;
    bool regen_active;
    int innervates;
    int mana_emerald;
    int mana_ruby;
    int combustion;
    double duration;
    bool cc_snapshot;
    bool cc_queue;
    bool used_pot;

    double evocated_at = -1;
    double regened_at = -1;
    double t_gcd_capped = 0;
    bool was_instant = false;

    map<cooldown::ID, shared_ptr<cooldown::Cooldown>> cooldowns;
    map<buff::ID, shared_ptr<buff::Buff>> buffs;
    map<debuff::ID, shared_ptr<debuff::Debuff>> debuffs;
    map<spell::ID, SpellStats> spells;

    shared_ptr<Config> config;

    State(shared_ptr<Config> _config)
    {
        config = _config;
        reset();
    }

    void reset()
    {
        t = 0;
        t_gcd = 0;
        t_mana_spent = -10;
        mana = 0;
        dmg = 0;
        regen_cycle = 0;
        regen_active = false;
        innervates = config->innervate;
        mana_emerald = 3;
        mana_ruby = 1;
        combustion = 0;
        duration = config->duration;
        duration+= -config->duration_variance + random<double>(0, config->duration_variance*2);
        cc_snapshot = false;
        cc_queue = false;
        used_pot = false;

        evocated_at = -1;
        regened_at = -1;
        t_gcd_capped = 0;
        was_instant = false;

        buffs.clear();
        debuffs.clear();
        cooldowns.clear();
        spells.clear();
    }

    double dps()
    {
        if (t == 0)
            return 0;

        return dmg / t;
    }

    bool hasManaGem()
    {
        return mana_emerald > 0 || mana_ruby > 0;
    }

    bool hasCooldown(cooldown::ID id)
    {
        return cooldowns.find(id) != cooldowns.end();
    }

    void addCooldown(shared_ptr<cooldown::Cooldown> cooldown)
    {
        cooldowns[cooldown->id] = cooldown;
    }

    void removeCooldown(cooldown::ID id)
    {
        cooldowns.erase(id);
    }

    int buffStacks(buff::ID id)
    {
        if (hasBuff(id))
            return buffs[id]->stacks;
        return 0;
    }

    bool hasBuff(buff::ID id)
    {
        return buffs.find(id) != buffs.end();
    }

    int addBuff(shared_ptr<buff::Buff> buff)
    {
        if (hasBuff(buff->id))
            return buffs[buff->id]->addStack();
        else
            buffs[buff->id] = buff;

        return 1;
    }

    void removeBuff(buff::ID id)
    {
        buffs.erase(id);
    }

    int debuffStacks(debuff::ID id)
    {
        if (hasDebuff(id))
            return debuffs[id]->stacks;
        return 0;
    }

    bool hasDebuff(debuff::ID id)
    {
        return debuffs.find(id) != debuffs.end();
    }

    int addDebuff(shared_ptr<debuff::Debuff> debuff)
    {
        if (hasDebuff(debuff->id))
            return debuffs[debuff->id]->addStack();
        else
            debuffs[debuff->id] = debuff;

        return 1;
    }

    void removeDebuff(debuff::ID id)
    {
        debuffs.erase(id);
    }

};
