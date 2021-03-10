#pragma once

#include <list>
#include <math.h>
#include <random>
#include <sstream>
#include <iomanip>

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
};

class Event
{

public:
    double t;
    double mana;
    std::string source;
    EventType type;
    spell::Spell *spell;
    buff::Buff *buff;
    cooldown::ID cooldown_id;

};

class Simulation
{

public:
    bool logging = true;
    std::list<Event*> queue;
    std::list<LogEntry*> log;
    State *state;
    Player *player;
    Settings *settings;

    Simulation()
    {
        settings = new Settings();
        state = new State(settings);
        player = new Player(settings);
    }

    void reset()
    {
        player->ready();
        state->reset();
        state->mana = player->maxMana();
    }

    SimulationResult run()
    {
        reset();

        pushManaRegen();

        if (settings->vampiric_touch)
            pushVampiricTouch(settings->vampiric_touch_regen);
        if (settings->bloodlust)
            pushBuffGain(new buff::Bloodlust(), settings->bloodlust_at);
        if (settings->mana_tide)
            pushBuffGain(new buff::ManaTide(), settings->mana_tide_at);

        cast(defaultSpell());

        work();

        SimulationResult result;
        result.dmg = state->dmg;
        result.t = state->t;
        result.dps = state->dmg/state->t;

        return result;
    }

    void work()
    {
        Event *event;

        while (true) {
            event = queue.front();
            queue.pop_front();

            if (event->t >= settings->duration) {
                state->t = settings->duration;
                break;
            }

            tick(event);
        }

        queue.clear();
    }

    void tick(Event *event)
    {
        state->t = event->t;

        if (event->type == EVENT_CAST)
            cast(event->spell);
        else if (event->type == EVENT_SPELL)
            onCast(event->spell);
        else if (event->type == EVENT_MANA_REGEN)
            onManaRegen();
        else if (event->type == EVENT_MANA_GAIN)
            onManaGain(event->mana, event->source);
        else if (event->type == EVENT_BUFF_GAIN)
            onBuffGain(event->buff);
        else if (event->type == EVENT_BUFF_EXPIRE)
            onBuffExpire(event->buff);
        else if (event->type == EVENT_VAMPIRIC_TOUCH)
            onVampiricTouch(event->mana);
        else if (event->type == EVENT_WAIT)
            onWait();
    }

    void push(Event *event)
    {
        event->t+= state->t;

        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if (event->t < (*itr)->t) {
                queue.insert(itr, event);
                return;
            }
        }

        queue.push_back(event);
    }

    void pushSpell(spell::Spell *spell, double t)
    {
        Event *event = new Event();
        event->type = EVENT_SPELL;
        event->spell = spell;
        event->t = t;

        push(event);
    }

    void pushManaRegen()
    {
        Event *event = new Event();
        event->type = EVENT_MANA_REGEN;
        event->t = 2;
        push(event);
    }

    void pushVampiricTouch(double mana)
    {
        Event *event = new Event();
        event->type = EVENT_VAMPIRIC_TOUCH;
        event->t = 1;
        event->mana = mana;

        push(event);
    }

    void pushManaGain(double t, double mana, std::string source = "")
    {
        Event *event = new Event();
        event->type = EVENT_MANA_GAIN;
        event->t = t;
        event->mana = mana;
        event->source = source;

        push(event);
    }

    void pushBuffGain(buff::Buff *buff, double t)
    {
        Event *event = new Event();
        event->type = EVENT_BUFF_GAIN;
        event->t = t;
        event->buff = buff;

        push(event);
    }

    void pushBuffExpire(buff::Buff *buff)
    {
        Event *event = new Event();
        event->type = EVENT_BUFF_EXPIRE;
        event->t = buff->duration;
        event->buff = buff;

        push(event);
    }

    void pushCooldownExpire(cooldown::ID id, double t)
    {
        Event *event = new Event();
        event->type = EVENT_CD_EXPIRE;
        event->t = t;
        event->cooldown_id = id;

        push(event);
    }

    void pushWait(double t)
    {
        Event *event = new Event();
        event->type = EVENT_WAIT;
        event->t = t;

        push(event);
    }

    void cast(spell::Spell *spell)
    {
        if (canCast(spell)) {
            if (spell->channeling)
                onCast(spell);
            else
                pushSpell(spell, castTime(spell));
        }
        else {
            pushWait(1);
        }
    }

    void onCast(spell::Spell *spell)
    {
        spell::Spell *next = NULL;

        if (spell->tick || canCast(spell)) {
            if (spell->channeling && !spell->tick) {
                onCastSuccess(spell);

                double cast_time = castTime(spell);
                for (int i=1; i<=4; i++)
                    pushSpell(spell, cast_time / spell->ticks * i);

                spell->tick++;
                return;
            }

            onCastDmg(spell);
            if (!spell->done)
                return;

            next = onCastComplete(spell);
        }
        else {
            next = defaultSpell();
        }

        if (next != NULL)
            cast(next);
    }

    double onCastSuccess(spell::Spell *spell)
    {
        spell->actual_cost = manaCost(spell);
        state->mana-= spell->actual_cost;

        if (state->hasBuff(buff::CLEARCAST))
            onBuffExpire(new buff::Clearcast());
        clearcast();

        return spell->actual_cost;
    }

    void onCastDmg(spell::Spell *spell)
    {
        spell->done = true;
        spell->result = spellRoll(spell);

        if (spell->result != spell::MISS) {
            spell->dmg = spellDmg(spell);

            if (spell->result == spell::CRIT)
                spell->dmg*= critMultiplier(spell);

            spell->dmg = round(spell->dmg);
            state->dmg+= spell->dmg;
        }

        logSpellDmg(spell);

        if (!spell->channeling)
            onCastSuccess(spell);

        if (settings->judgement_of_wisdom && random<int>(0, 1) == 1)
            onManaGain(74, "Judgement of Wisdom");

        if (spell->channeling) {
            spell->tick++;
            spell->done = spell->tick == spell->ticks;
        }
    }

    spell::Spell* onCastComplete(spell::Spell *spell)
    {
        spell::Spell *next = NULL;

        if (spell->id == spell::ARCANE_BLAST) {
            onBuffGain(new buff::ArcaneBlast());
            if (settings->t5_4set && spell->result == spell::CRIT)
                onBuffGain(new buff::ArcaneMadness());
        }

        if (!state->hasCooldown(cooldown::PERSONAL) && settings->cooldowns_at <= state->t)
            useCooldowns();
        else if (state->hasCooldown(cooldown::PERSONAL) && !state->hasCooldown(cooldown::COLD_SNAP) && player->talents.cold_snap && !state->hasBuff(buff::ICY_VEINS))
            useColdSnap();

        if (shouldUseManaGem())
            useManaGem();
        if (shouldUseManaPotion())
            useManaPotion();

        if (shouldEvocate()) {
            evocate();
            return NULL;
        }

        if (shouldInnervate()) {
            innervate();
        }
        else if (player->spec == SPEC_ARCANE) {
            double regen_at = settings->regen_mana_at;
            if (state->hasBuff(buff::BLOODLUST))
                regen_at = std::min(regen_at, 10.0);

            int end = 3;
            if (settings->regen_rotation == ROTATION_AMFB)
                end = 2;

            if (state->regen_cycle == end) {
                state->regen_cycle = 0;
            }
            else if (state->regen_cycle || manaPercent() <= regen_at && state->buffStacks(buff::ARCANE_BLAST) == 3) {
                if (settings->regen_rotation == ROTATION_AMFB && state->regen_cycle == 0)
                    next = new spell::ArcaneMissiles();
                else
                    next = new spell::Frostbolt();
                state->regen_cycle++;
            }
        }

        if (next == NULL)
            next = defaultSpell();

        return next;
    }

    void onManaRegen()
    {
        onManaGain(totalManaPerTick(), "Mana Regen");
        pushManaRegen();
    }

    void onManaGain(double mana, std::string source = "")
    {
        state->mana = std::min(player->maxMana(), state->mana + mana);
        logManaGain(mana, source);
    }

    void onVampiricTouch(double mana)
    {
        onManaGain(mana, "Vampiric Touch");
        pushVampiricTouch(mana);
    }

    void onWait()
    {
        cast(defaultSpell());
    }

    void onBuffGain(buff::Buff *buff)
    {
        int stacks = state->addBuff(buff);
        removeBuffExpiration(buff);
        pushBuffExpire(buff);

        if (buff->id == buff::MANA_TIDE) {
            for (double t=3; t<=12; t+= 3)
                pushManaGain(t, player->maxMana() * 0.06, "Mana Tide");
        }

        if (stacks)
            logBuffGain(buff, stacks);
    }

    void onBuffExpire(buff::Buff *buff)
    {
        state->removeBuff(buff->id);

        logBuffExpire(buff);
    }

    void onCooldownExpire(cooldown::ID id)
    {
        state->removeCooldown(id);
    }

    void useManaPotion()
    {
        double mana = round(random<double>(1800, 3000));

        state->addCooldown(cooldown::POTION);
        onManaGain(mana, "Mana Potion");
        pushCooldownExpire(cooldown::POTION, 120);
    }

    void useManaGem()
    {
        double mana = 0;

        if (state->mana_emerald > 0) {
            mana = round(random<double>(2340, 2460));
            state->mana_emerald--;
        }
        else if (state->mana_ruby > 0) {
            mana = round(random<double>(1073, 1127));
            state->mana_ruby--;
        }

        if (settings->serpent_coil)
            mana*= 1.25;

        state->addCooldown(cooldown::MANA_GEM);
        onManaGain(mana, "Mana Gem");
        pushCooldownExpire(cooldown::MANA_GEM, 120);

        if (settings->serpent_coil)
            onBuffGain(new buff::SerpentCoil());
    }

    double manaPercent()
    {
        return state->mana / player->maxMana() * 100.0;
    }

    double totalManaPerSecond()
    {
        double mps = player->staticManaPerSecond();

        double while_casting = 0;
        if (player->talents.arcane_meditation)
            while_casting+= player->talents.arcane_meditation*0.1;
        if (settings->mage_armor)
            while_casting+= 0.3;
        if (state->hasBuff(buff::INNERVATE))
            while_casting = 4;
        mps+= while_casting * player->spiritManaPerSecond();

        return mps;
    }

    double totalManaPerTick()
    {
        return totalManaPerSecond() * 2;
    }

    spell::Spell* defaultSpell()
    {
        if (player->spec == SPEC_ARCANE)
            return new spell::ArcaneBlast();

        return NULL;
    }

    void useCooldowns()
    {
        state->cooldowns[cooldown::PERSONAL] = true;

        if (player->talents.arcane_power)
            onBuffGain(new buff::ArcanePower());
        if (player->talents.icy_veins)
            onBuffGain(new buff::IcyVeins());
        if (settings->silver_crescent)
            onBuffGain(new buff::SilverCrescent());
    }

    void useColdSnap()
    {
        state->cooldowns[cooldown::COLD_SNAP] = true;
        if (player->talents.icy_veins)
            onBuffGain(new buff::IcyVeins());
    }

    void removeBuffExpiration(buff::Buff *buff)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_BUFF_EXPIRE && (*itr)->buff->id == buff->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    bool canCast(spell::Spell *spell)
    {
        return state->mana >= manaCost(spell);
    }

    double manaCost(spell::Spell *spell)
    {
        if (state->hasBuff(buff::CLEARCAST))
            return 0;

        double multi = 1;

        if (spell->id == spell::ARCANE_BLAST) {
            multi+= 0.75 * state->buffStacks(buff::ARCANE_BLAST);
            if (settings->t5_2set)
                multi+= 0.2;
        }

        if (spell->school == SCHOOL_FROST && player->talents.frost_channeling)
            multi-= player->talents.frost_channeling*0.05;

        return round(spell->cost * multi);
    }

    double castTime(spell::Spell *spell)
    {
        double t = spell->cast_time;

        if (spell->id == spell::ARCANE_BLAST && state->hasBuff(buff::ARCANE_BLAST))
            t-= (state->buffStacks(buff::ARCANE_BLAST)/3.0);

        if (spell->id == spell::FROSTBOLT && player->talents.imp_frostbolt)
            t-= player->talents.imp_frostbolt*0.1;

        return t * castHaste();
    }

    double castHaste()
    {
        double haste = 1;

        if (player->stats.haste)
            haste*= (100.0 - player->stats.haste)/100.0;

        if (state->hasBuff(buff::BLOODLUST))
            haste*= 0.7;
        if (state->hasBuff(buff::ICY_VEINS))
            haste*= 0.8;

        return haste;
    }

    double hitChance(spell::Spell *spell)
    {
        double hit = 83.0 + player->stats.hit;

        if (spell->school == SCHOOL_ARCANE && player->talents.arcane_focus)
            hit+= player->talents.arcane_focus*2.0;
        if (spell->school == SCHOOL_FROST && player->talents.elemental_precision)
            hit+= player->talents.elemental_precision;

        return hit;
    }

    double critChance(spell::Spell *spell)
    {
        double crit = player->stats.crit;

        if (spell->id == spell::ARCANE_BLAST && player->talents.arcane_impact)
            crit+= player->talents.arcane_impact*2.0;

        if (state->hasBuff(buff::CLEARCAST) && player->talents.arcane_potency)
            crit+= player->talents.arcane_potency*10.0;

        if (settings->judgement_of_the_crusader)
            crit+= 3;
        if (settings->moonkin_aura)
            crit+= 5;

        return crit;
    }

    double critMultiplier(spell::Spell *spell)
    {
        double multi = 1.5;

        if (player->talents.spell_power)
            multi+= player->talents.spell_power*0.125;

        if (settings->chaotic_skyfire)
            multi+= 0.03;

        if (spell->school == SCHOOL_FROST && player->talents.ice_shards)
            multi+= player->talents.ice_shards*0.1;

        return multi;
    }

    double dmgMultiplier(spell::Spell *spell)
    {
        double multi = spell->coeff;

        if (settings->misery)
            multi*= 1.05;
        if (settings->curse_of_elements)
            multi*= 1.1;

        if (player->talents.arcane_instability)
            multi*= 1 + (player->talents.arcane_instability * 0.01);

        if (state->hasBuff(buff::ARCANE_POWER))
            multi*= 1.3;

        if (spell->id == spell::ARCANE_BLAST && settings->t5_2set)
            multi*= 1.2;

        return multi;
    }

    double spellDmg(spell::Spell *spell)
    {
        double dmg = random<double>(spell->min_dmg, spell->max_dmg);
        dmg+= player->stats.spell_power;

        if (spell->school == SCHOOL_ARCANE)
            dmg+= player->stats.spell_power_arcane;
        if (spell->school == SCHOOL_FROST)
            dmg+= player->stats.spell_power_frost;

        if (state->hasBuff(buff::ARCANE_MADNESS))
            dmg+= 70.0;
        if (state->hasBuff(buff::SILVER_CRESCENT))
            dmg+= 155.0;
        if (state->hasBuff(buff::SERPENT_COIL))
            dmg+= 225.0;

        return dmg * dmgMultiplier(spell);
    }

    spell::Result spellRoll(spell::Spell *spell)
    {
        if (random<double>(0, 100) > hitChance(spell))
            return spell::MISS;

        if (random<double>(0, 100) <= critChance(spell))
            return spell::CRIT;

        return spell::HIT;
    }

    void innervate()
    {
        state->innervates--;
        onBuffGain(new buff::Innervate());
    }

    void evocate()
    {
        double haste = castHaste();

        state->regen_cycle = 0;
        state->addCooldown(cooldown::EVOCATION);

        for (double i=1; i<=4; i++)
            pushManaGain(i * haste * 2.0, player->maxMana()*0.15, "Evocation");

        Event *event = new Event();
        event->type = EVENT_CAST;
        event->t = 8.0 * haste;
        event->spell = defaultSpell();
        push(event);
    }

    void clearcast()
    {
        double chance = player->talents.clearcast * 2;
        if (random<double>(0, 100) <= chance)
            onBuffGain(new buff::Clearcast());
    }

    bool shouldInnervate()
    {
        if (!state->innervates || state->hasBuff(buff::INNERVATE))
            return false;

        if (manaPercent() < 70.0 && state->hasCooldown(cooldown::POTION) && state->hasCooldown(cooldown::MANA_GEM))
            return true;

        if (manaPercent() < 30.0)
            return true;

        return false;
    }

    bool shouldEvocate()
    {
        if (state->hasCooldown(cooldown::EVOCATION) || state->hasBuff(buff::INNERVATE) || state->hasBuff(buff::MANA_TIDE))
            return false;

        if (manaPercent() > 20.0)
            return false;

        if (state->hasBuff(buff::BLOODLUST) && manaPercent() > 10.0)
            return false;

        return true;
    }

    bool shouldUseManaGem()
    {
        if (state->hasCooldown(cooldown::MANA_GEM) || state->hasBuff(buff::INNERVATE))
            return false;

        double max = 0;
        if (state->mana_emerald > 0)
            max = 2460;
        else if (state->mana_ruby > 0)
            max = 1127;
        else
            return false;

        if (settings->serpent_coil)
            max*= 1.25;

        if (state->hasBuff(buff::MANA_TIDE))
            max+= player->maxMana() * 0.06;

        return player->maxMana() - state->mana >= max;
    }

    bool shouldUseManaPotion()
    {
        if (state->hasCooldown(cooldown::POTION) || !state->hasCooldown(cooldown::MANA_GEM) || state->hasBuff(buff::INNERVATE))
            return false;

        double max = 3000;

        if (state->hasBuff(buff::MANA_TIDE))
            max+= player->maxMana() * 0.06;

        return player->maxMana() - state->mana >= max;
    }

    void logSpellDmg(spell::Spell *spell)
    {
        if (!logging)
            return;

        std::ostringstream s;

        s << spell->name;
        if (spell->result == spell::MISS)
            s << " was resisted";
        else if (spell->result == spell::CRIT)
            s << " crit for " << spell->dmg;
        else
            s << " hit for " << spell->dmg;

        addLog(LOG_SPELL, s.str());
    }

    void logBuffGain(buff::Buff *buff, int stacks = 1)
    {
        if (!logging)
            return;

        std::ostringstream s;

        s << "Gained " << buff->name;
        if (stacks > 1)
            s << " (" << stacks << ")";

        addLog(LOG_BUFF, s.str());
    }

    void logBuffExpire(buff::Buff *buff)
    {
        if (!logging)
            return;

        std::ostringstream s;

        s << "Lost " << buff->name;

        addLog(LOG_BUFF, s.str());
    }

    void logManaGain(double mana, std::string source)
    {
        if (!logging)
            return;

        std::ostringstream s;

        s << std::fixed << std::setprecision(0);
        s << "Gained " << mana << " mana from " << source;

        addLog(LOG_MANA, s.str());
    }

    void addLog(LogType type, std::string text)
    {
        if (!logging)
            return;

        LogEntry *entry = new LogEntry();
        entry->type = type;
        entry->text = text;
        entry->t = state->t;
        entry->dmg = state->dmg;
        entry->mana = state->mana;
        entry->mana_percent = manaPercent();

        log.push_back(entry);
    }

};