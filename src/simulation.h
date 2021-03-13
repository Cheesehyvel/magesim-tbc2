#include <list>
#include <math.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <memory>

using namespace std;

class Event
{

public:
    double t;
    double mana;
    string source;
    EventType type;
    shared_ptr<spell::Spell> spell;
    shared_ptr<buff::Buff> buff;
    cooldown::ID cooldown_id;

};

class Simulation
{

public:
    bool logging = true;
    list<shared_ptr<Event>> queue;
    vector<shared_ptr<LogEntry>> log;
    shared_ptr<State> state;
    shared_ptr<Player> player;
    shared_ptr<Config> config;

    Simulation(shared_ptr<Config> _config, shared_ptr<Player> _player)
    {
        config = _config;
        player = _player;
        state = make_shared<State>(config);
    }

    void reset()
    {
        clearLog();
        state->reset();
        state->mana = player->maxMana();
    }

    SimulationsResult runMultiple(int iterations)
    {
        SimulationResult r;
        SimulationsResult result;

        logging = false;

        for (int i=0; i<iterations; i++) {
            r = run();
            if (i == 0 || r.dps < result.min_dps)
                result.min_dps = r.dps;
            if (i == 0 || r.dps > result.max_dps)
                result.max_dps = r.dps;
            result.avg_dps+= ((r.dps - result.avg_dps) / (i+1));
        }

        result.iterations = iterations;

        return result;
    }

    SimulationResult run()
    {
        reset();

        pushManaRegen();

        if (config->vampiric_touch)
            pushVampiricTouch(config->vampiric_touch_regen);
        if (config->bloodlust)
            pushBuffGain(make_shared<buff::Bloodlust>(), config->bloodlust_at);
        if (config->mana_tide)
            pushBuffGain(make_shared<buff::ManaTide>(), config->mana_tide_at);

        cast(defaultSpell());

        work();

        SimulationResult result;
        result.dmg = state->dmg;
        result.t = state->t;
        result.dps = state->dmg/state->t;

        if (logging)
            result.log = jsonLog();

        return result;
    }

    void work()
    {
        shared_ptr<Event> event;

        while (true) {
            event = queue.front();
            queue.pop_front();

            if (event->t >= config->duration) {
                state->t = config->duration;
                break;
            }

            tick(event);
        }

        queue.clear();
    }

    void tick(shared_ptr<Event> event)
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

    void push(shared_ptr<Event> event)
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

    void pushCast(shared_ptr<spell::Spell> spell, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CAST;
        event->spell = spell;
        event->t = t;

        push(event);
    }

    void pushSpell(shared_ptr<spell::Spell> spell, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL;
        event->spell = spell;
        event->t = t;

        push(event);
    }

    void pushManaRegen()
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_MANA_REGEN;
        event->t = 2;
        push(event);
    }

    void pushVampiricTouch(double mana)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_VAMPIRIC_TOUCH;
        event->t = 1;
        event->mana = mana;

        push(event);
    }

    void pushManaGain(double t, double mana, string source = "")
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_MANA_GAIN;
        event->t = t;
        event->mana = mana;
        event->source = source;

        push(event);
    }

    void pushBuffGain(shared_ptr<buff::Buff> buff, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_GAIN;
        event->t = t;
        event->buff = buff;

        push(event);
    }

    void pushBuffExpire(shared_ptr<buff::Buff> buff)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_EXPIRE;
        event->t = buff->duration;
        event->buff = buff;

        push(event);
    }

    void pushCooldownExpire(cooldown::ID id, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CD_EXPIRE;
        event->t = t;
        event->cooldown_id = id;

        push(event);
    }

    void pushWait(double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_WAIT;
        event->t = t;

        push(event);

        addLog(LOG_WAIT, "Out of mana, waiting...");
    }

    void cast(shared_ptr<spell::Spell> spell)
    {
        if (canCast(spell)) {
            if (state->t_gcd > state->t) {
                pushCast(spell, state->t_gcd - state->t);
            }
            else {
                state->t_gcd = state->t + gcd();
                if (spell->channeling)
                    onCast(spell);
                else
                    pushSpell(spell, castTime(spell));
            }
        }
        else {
            pushWait(1);
        }
    }

    void onCast(shared_ptr<spell::Spell> spell)
    {
        shared_ptr<spell::Spell> next = NULL;

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
            if (!spell->done || spell->proc)
                return;

            next = onCastComplete(spell);
        }
        else {
            next = defaultSpell();
        }

        if (next != NULL)
            cast(next);
    }

    double onCastSuccess(shared_ptr<spell::Spell> spell)
    {
        spell->actual_cost = manaCost(spell);
        state->mana-= spell->actual_cost;

        if (state->hasBuff(buff::CLEARCAST))
            onBuffExpire(make_shared<buff::Clearcast>());
        clearcast();

        // 5% proc rate
        if (config->meta_gem == META_INSIGHTFUL_EARTHSTORM && random<int>(0, 19) == 0)
            onManaGain(300, "Mana Restore (meta)");

        return spell->actual_cost;
    }

    void onCastDmg(shared_ptr<spell::Spell> spell)
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

        if (spell->proc)
            return;

        if (!spell->channeling)
            onCastSuccess(spell);

        if (spell->result == spell::MISS) {
            if (hasTrinket(TRINKET_EYE_OF_MAGTHERIDON))
                onBuffGain(make_shared<buff::EyeOfMagtheridon>());
        }
        else {
            // 5% proc rate ?
            if (hasTrinket(TRINKET_QUAGMIRRANS_EYE) && random<int>(0, 19) == 0)
                onBuffGain(make_shared<buff::QuagmirransEye>());
            // 5% proc rate ?
            if (config->spellstrike_set && random<int>(0, 19) == 0)
                onBuffGain(make_shared<buff::Spellstrike>());
            // 50% proc rate
            if (config->judgement_of_wisdom && random<int>(0, 1) == 1)
                onManaGain(74, "Judgement of Wisdom");

            if (spell->result == spell::CRIT) {
                // 20% proc rate
                if (hasTrinket(TRINKET_UNSTABLE_CURRENTS) && random<int>(0, 4) == 0)
                    onBuffGain(make_shared<buff::UnstableCurrents>());
                // 100% proc rate
                if (hasTrinket(TRINKET_LIGHTNING_CAPACITOR))
                    onBuffGain(make_shared<buff::LightningCapacitor>());
            }
        }

        if (spell->channeling) {
            spell->tick++;
            spell->done = spell->tick == spell->ticks;
        }
    }

    shared_ptr<spell::Spell> onCastComplete(shared_ptr<spell::Spell> spell)
    {
        shared_ptr<spell::Spell> next = NULL;

        if (spell->id == spell::ARCANE_BLAST) {
            onBuffGain(make_shared<buff::ArcaneBlast>());
            if (config->tirisfal_4set && spell->result == spell::CRIT)
                onBuffGain(make_shared<buff::ArcaneMadness>());
        }

        if (!state->hasCooldown(cooldown::PERSONAL) && config->cooldowns_at <= state->t)
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
            double regen_at = config->regen_mana_at;
            if (state->hasBuff(buff::BLOODLUST))
                regen_at = min(regen_at, 10.0);

            int end = 3;
            if (config->regen_rotation == ROTATION_AMFB)
                end = 2;

            if (state->regen_cycle == end) {
                state->regen_cycle = 0;
            }
            else if (state->regen_cycle || manaPercent() <= regen_at && state->buffStacks(buff::ARCANE_BLAST) == 3) {
                if (config->regen_rotation == ROTATION_AMFB && state->regen_cycle == 0)
                    next = make_shared<spell::ArcaneMissiles>();
                else
                    next = make_shared<spell::Frostbolt>();
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

    void onManaGain(double mana, string source = "")
    {
        state->mana = min(player->maxMana(), state->mana + mana);
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

    void onBuffGain(shared_ptr<buff::Buff> buff)
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

        if (buff->id == buff::LIGHTNING_CAPACITOR && stacks == 3) {
            onBuffExpire(buff);
            fireLightningCapacitor();
        }
    }

    void onBuffExpire(shared_ptr<buff::Buff> buff)
    {
        logBuffExpire(buff);
        state->removeBuff(buff->id);
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

        if (hasTrinket(TRINKET_SERPENT_COIL))
            mana*= 1.25;

        state->addCooldown(cooldown::MANA_GEM);
        onManaGain(mana, "Mana Gem");
        pushCooldownExpire(cooldown::MANA_GEM, 120);

        if (hasTrinket(TRINKET_SERPENT_COIL))
            onBuffGain(make_shared<buff::SerpentCoil>());
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
        if (config->mage_armor)
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

    shared_ptr<spell::Spell> defaultSpell()
    {
        if (player->spec == SPEC_ARCANE)
            return make_shared<spell::ArcaneBlast>();

        return NULL;
    }

    void useCooldowns()
    {
        state->addCooldown(cooldown::PERSONAL);

        if (player->talents.arcane_power)
            onBuffGain(make_shared<buff::ArcanePower>());
        if (player->talents.icy_veins)
            onBuffGain(make_shared<buff::IcyVeins>());
        if (player->race == RACE_TROLL)
            onBuffGain(make_shared<buff::Berserking>());
        if (hasTrinket(TRINKET_SILVER_CRESCENT))
            onBuffGain(make_shared<buff::SilverCrescent>());
        if (hasTrinket(TRINKET_RESTRAINED_ESSENCE))
            onBuffGain(make_shared<buff::RestrainedEssence>());
    }

    void useColdSnap()
    {
        state->addCooldown(cooldown::COLD_SNAP);
        addLog(LOG_NONE, "Casted Cold Snap");

        if (player->talents.icy_veins)
            onBuffGain(make_shared<buff::IcyVeins>());
    }

    bool hasTrinket(Trinket trinket)
    {
        return config->trinket1 == trinket || config->trinket2 == trinket;
    }

    void removeBuffExpiration(shared_ptr<buff::Buff> buff)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_BUFF_EXPIRE && (*itr)->buff->id == buff->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    bool canCast(shared_ptr<spell::Spell> spell)
    {
        return state->mana >= manaCost(spell);
    }

    double manaCost(shared_ptr<spell::Spell> spell)
    {
        if (state->hasBuff(buff::CLEARCAST))
            return 0;

        double multi = 1;

        if (spell->id == spell::ARCANE_BLAST) {
            multi+= 0.75 * state->buffStacks(buff::ARCANE_BLAST);
            if (config->tirisfal_2set)
                multi+= 0.2;
        }

        if (spell->school == SCHOOL_FROST && player->talents.frost_channeling)
            multi-= player->talents.frost_channeling*0.05;

        if (spell->id == spell::ARCANE_MISSILES && player->talents.empowered_arcane_missiles)
            multi+= player->talents.empowered_arcane_missiles * 0.02;

        return round(spell->cost * multi);
    }

    double gcd()
    {
        double t = 1.5;

        t*= castHaste();

        if (t < 1.0)
            t = 1.0;

        return t;
    }

    double castTime(shared_ptr<spell::Spell> spell)
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
        double phaste = player->stats.haste;
        double rating = 0;

        if (state->hasBuff(buff::QUAGMIRRANS_EYE))
            rating+= 320;

        if (rating)
            phaste+= hasteRatingToHaste(rating);

        if (phaste)
            haste*= (100.0 - phaste)/100.0;

        if (state->hasBuff(buff::BLOODLUST))
            haste*= 0.7;
        if (state->hasBuff(buff::ICY_VEINS))
            haste*= 0.8;
        if (state->hasBuff(buff::BERSERKING))
            haste*= 0.9;

        return haste;
    }

    double hitChance(shared_ptr<spell::Spell> spell)
    {
        if (spell->proc)
            return 99.0;

        double hit = 83.0 + player->stats.hit;

        if (spell->school == SCHOOL_ARCANE && player->talents.arcane_focus)
            hit+= player->talents.arcane_focus*2.0;
        if (spell->school == SCHOOL_FROST && player->talents.elemental_precision)
            hit+= player->talents.elemental_precision;

        return hit;
    }

    double critChance(shared_ptr<spell::Spell> spell)
    {
        double crit = player->stats.crit;
        double rating = 0;

        if (state->hasBuff(buff::UNSTABLE_CURRENTS))
            rating+= 190;

        if (rating)
            crit+= critRatingToChance(rating);

        if (spell->proc)
            return crit;

        if (spell->id == spell::ARCANE_BLAST && player->talents.arcane_impact)
            crit+= player->talents.arcane_impact*2.0;

        if (state->hasBuff(buff::CLEARCAST) && player->talents.arcane_potency)
            crit+= player->talents.arcane_potency*10.0;

        if (config->judgement_of_the_crusader)
            crit+= 3;
        if (config->moonkin_aura)
            crit+= 5;

        return crit;
    }

    double critMultiplier(shared_ptr<spell::Spell> spell)
    {
        double multi = 1.5;

        if (spell->proc)
            return multi;

        if (player->talents.spell_power)
            multi+= player->talents.spell_power*0.125;

        if (config->meta_gem == META_CHAOTIC_SKYFIRE)
            multi+= 0.03;

        if (spell->school == SCHOOL_FROST && player->talents.ice_shards)
            multi+= player->talents.ice_shards*0.1;

        return multi;
    }

    double dmgMultiplier(shared_ptr<spell::Spell> spell)
    {
        double multi = 1;

        if (spell->proc)
            return multi;

        if (config->misery)
            multi*= 1.05;
        if (config->curse_of_elements)
            multi*= 1.1;

        if (player->talents.arcane_instability)
            multi*= 1 + (player->talents.arcane_instability * 0.01);
        if (player->talents.piercing_ice && spell->school == SCHOOL_FROST)
            multi*= 1 + (player->talents.piercing_ice * 0.02);

        if (state->hasBuff(buff::ARCANE_POWER))
            multi*= 1.3;

        if (spell->id == spell::ARCANE_BLAST && config->tirisfal_2set)
            multi*= 1.2;

        return multi;
    }

    double spellDmg(shared_ptr<spell::Spell> spell)
    {
        double dmg = random<double>(spell->min_dmg, spell->max_dmg);

        if (spell->coeff) {
            double sp = player->stats.spell_power;
            double coeff = spell->coeff;

            if (spell->school == SCHOOL_ARCANE)
                sp+= player->stats.spell_power_arcane;
            if (spell->school == SCHOOL_FROST)
                sp+= player->stats.spell_power_frost;
            if (spell->school == SCHOOL_FIRE)
                sp+= player->stats.spell_power_fire;

            if (state->hasBuff(buff::ARCANE_MADNESS))
                sp+= 70.0;
            if (state->hasBuff(buff::SILVER_CRESCENT))
                sp+= 155.0;
            if (state->hasBuff(buff::SERPENT_COIL))
                sp+= 225.0;
            if (state->hasBuff(buff::SPELLSTRIKE))
                sp+= 92.0;
            if (state->hasBuff(buff::EYE_OF_MAGTHERIDON))
                sp+= 170.0;
            if (state->hasBuff(buff::RESTRAINED_ESSENCE))
                sp+= 130.0;

            if (spell->id == spell::ARCANE_MISSILES && player->talents.empowered_arcane_missiles)
                coeff+= player->talents.empowered_arcane_missiles * 0.15;

            if (spell->channeling)
                coeff/= spell->ticks;

            dmg+= sp*coeff;
        }

        return dmg * dmgMultiplier(spell);
    }

    spell::Result spellRoll(shared_ptr<spell::Spell> spell)
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
        onBuffGain(make_shared<buff::Innervate>());
    }

    void evocate()
    {
        double haste = castHaste();

        state->regen_cycle = 0;
        state->addCooldown(cooldown::EVOCATION);

        for (double i=1; i<=4; i++)
            pushManaGain(i * haste * 2.0, player->maxMana()*0.15, "Evocation");

        shared_ptr<Event> event(new Event());
        event->type = EVENT_CAST;
        event->t = 8.0 * haste;
        event->spell = defaultSpell();
        push(event);
    }

    void clearcast()
    {
        double chance = player->talents.clearcast * 2;
        if (random<double>(0, 100) <= chance)
            onBuffGain(make_shared<buff::Clearcast>());
    }

    void fireLightningCapacitor()
    {
        cast(make_shared<spell::LightningCapacitor>());
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

        if (hasTrinket(TRINKET_SERPENT_COIL))
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

    void logSpellDmg(shared_ptr<spell::Spell> spell)
    {
        if (!logging)
            return;

        ostringstream s;

        s << spell->name;
        if (spell->result == spell::MISS)
            s << " was resisted";
        else if (spell->result == spell::CRIT)
            s << " crit for " << spell->dmg;
        else
            s << " hit for " << spell->dmg;

        addLog(LOG_SPELL, s.str());
    }

    void logBuffGain(shared_ptr<buff::Buff> buff, int stacks = 1)
    {
        if (!logging)
            return;

        ostringstream s;

        s << "Gained " << buff->name;
        if (stacks > 1)
            s << " (" << stacks << ")";

        addLog(LOG_BUFF, s.str());
    }

    void logBuffExpire(shared_ptr<buff::Buff> buff)
    {
        if (!logging)
            return;

        ostringstream s;

        s << "Lost " << buff->name;

        addLog(LOG_BUFF, s.str());
    }

    void logManaGain(double mana, string source)
    {
        if (!logging)
            return;

        ostringstream s;

        s << fixed << setprecision(0);
        s << "Gained " << mana << " mana from " << source;

        addLog(LOG_MANA, s.str());
    }

    string jsonLog()
    {
        ostringstream s;

        s << "[";

        for (int i=0; i<log.size(); i++) {
            if (i > 0)
                s << ",";
            s << "{";
            s << "\"text\":\"" << log[i]->text << "\"";
            s << ",\"t\":" << log[i]->t;
            s << ",\"type\":" << log[i]->type;
            s << ",\"dmg\":" << log[i]->dmg;
            s << ",\"mana\":" << log[i]->mana;
            s << ",\"mana_percent\":" << log[i]->mana_percent;
            s << "}";
        }

        s << "]";

        return s.str();
    }

    void addLog(LogType type, string text)
    {
        if (!logging)
            return;

        shared_ptr<LogEntry> entry(new LogEntry);
        entry->type = type;
        entry->text = text;
        entry->t = state->t;
        entry->dmg = state->dmg;
        entry->mana = state->mana;
        entry->mana_percent = manaPercent();

        log.push_back(entry);
    }

    void printLog(bool show_mana = false)
    {
        for (auto itr = log.begin(); itr != log.end(); itr++) {
            if ((*itr)->type == LOG_MANA && show_mana)
                continue;
            printf("%.2f %s\n", (*itr)->t, (*itr)->text.c_str());
        }
    }

    void clearLog()
    {
        log.clear();
    }

};