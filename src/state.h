#pragma once

#include <map>
#include <vector>

class State
{

public:
    double t;
    double mana;
    int dmg;
    int regen_cycle;
    int innervates;
    int mana_emerald;
    int mana_ruby;

    std::map<cooldown::ID, bool> cooldowns;
    std::map<buff::ID, buff::Buff*> buffs;
    std::vector<std::string> log;

    Settings *settings;

    State(Settings *_settings)
    {
        settings = _settings;
        reset();
    }

    void reset()
    {
        t = 0;
        mana = 0;
        dmg = 0;
        regen_cycle = 0;
        innervates = settings->innervate;
        mana_emerald = 3;
        mana_ruby = 1;

        cooldowns.clear();
        buffs.clear();
        log.clear();
    }

    double dps()
    {
        if (t == 0)
            return 0;

        return dmg / t;
    }

    bool hasCooldown(cooldown::ID id)
    {
        return cooldowns.find(id) != cooldowns.end();
    }

    void addCooldown(cooldown::ID id)
    {
        cooldowns[id] = true;
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

    int addBuff(buff::Buff *buff)
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

};