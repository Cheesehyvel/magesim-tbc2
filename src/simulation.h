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
    shared_ptr<debuff::Debuff> debuff;
    shared_ptr<cooldown::Cooldown> cooldown;
    shared_ptr<dot::Dot> dot;

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

        double bin_size = 20;
        int bin;
        map<int, int> histogram;
        ostringstream results;

        double evocated = 0;
        double evocated_at = 0;
        double regened = 0;
        double regened_at = 0;
        double t_gcd_capped = 0;

        if (config->additional_data)
            results << "DPS,Duration\n";

        for (int i=0; i<iterations; i++) {
            if (config->rng_seed)
                setRNGSeed(config->rng_seed+i);

            r = run();

            if (i == 0 || r.dps < result.min_dps)
                result.min_dps = r.dps;
            if (i == 0 || r.dps > result.max_dps)
                result.max_dps = r.dps;
            result.avg_dps+= (r.dps - result.avg_dps) / (i+1);

            if (r.evocated_at != -1) {
                evocated++;
                evocated_at+= (r.evocated_at - evocated_at) / evocated;
            }
            if (r.regened_at != -1) {
                regened++;
                regened_at+= (r.regened_at - regened_at) / regened;
            }

            t_gcd_capped+= (r.t_gcd_capped - t_gcd_capped) / (i+1);

            bin = floor(r.dps/bin_size)*bin_size;
            if (histogram.find(bin) != histogram.end())
                histogram[bin]++;
            else
                histogram[bin] = 1;

            if (config->additional_data)
                results << r.dps << "," << r.t << "\n";
        }

        result.iterations = iterations;

        if (config->additional_data)
            result.all_results = results.str();

        // Histogram json string
        ostringstream ss;
        ss << "{";
        for (auto itr = histogram.begin(); itr != histogram.end(); itr++) {
            if (itr != histogram.begin())
                ss << ",";
            ss << "\"" << itr->first << "\":" << itr->second;
        }
        ss << "}";
        result.histogram = ss.str();

        // Stats json string
        ss.str("");
        ss.clear();
        ss << "{";
        ss << "\"evocated\":{\"t\":" << evocated_at << ",\"n\":" << evocated << "},";
        ss << "\"regened\":{\"t\":" << regened_at << ",\"n\":" << regened << "},";
        ss << "\"t_gcd_capped\":" << t_gcd_capped;
        ss << "}";
        result.stats = ss.str();

        return result;
    }

    SimulationResult run()
    {
        reset();

        pushManaRegen();

        if (config->vampiric_touch)
            pushVampiricTouch(config->vampiric_touch_regen);
        if (config->mana_spring)
            pushManaSpring();

        if (config->innervate) {
            for (int i=0; i<config->innervate_t.size() && i<config->innervate; i++)
                pushInnervate(config->innervate_t.at(i));
        }
        if (config->bloodlust) {
            for (int i=0; i<config->bloodlust_t.size(); i++)
                pushBuffGain(make_shared<buff::Bloodlust>(), config->bloodlust_t.at(i));
        }
        if (config->power_infusion) {
            for (int i=0; i<config->power_infusion_t.size(); i++)
                pushBuffGain(make_shared<buff::PowerInfusion>(), config->power_infusion_t.at(i));
        }
        if (config->mana_tide) {
            for (int i=0; i<config->mana_tide_t.size(); i++)
                pushBuffGain(make_shared<buff::ManaTide>(), config->mana_tide_t.at(i));
        }
        if (config->drums && config->drums_friend) {
            double t = 0;
            for (int i=0; i<config->drums_t.size(); i++) {
                if (config->drums_t.at(i) >= t)
                    t = config->drums_t.at(i);
                pushDrums(t);
            }
            for (t+= 120; t<state->duration; t+= 120)
                pushDrums(t);
        }

        if (config->fire_vulnerability) {
            for (double t=1.5; t<state->duration;) {
                pushDebuffGain(make_shared<debuff::FireVulnerability>(), t);
                if (t < 7.5)
                    t+= 1.5;
                else
                    t+= 25;
            }
        }

        if (config->winters_chill) {
            for (double t=2.5; t<state->duration;) {
                pushDebuffGain(make_shared<debuff::WintersChill>(), t);
                if (t < 12.5)
                    t+= 2.5;
                else
                    t+= 12;
            }
        }

        useCooldowns();
        workCurrent();

        cast(nextSpell());

        work();

        SimulationResult result;
        result.dmg = state->dmg;
        result.t = state->t;
        result.dps = state->dmg/state->t;
        result.evocated_at = state->evocated_at;
        result.regened_at = state->regened_at;
        result.t_gcd_capped = state->t_gcd_capped;

        if (logging) {
            result.log = jsonLog();
            result.spells = spellStats();
        }

        return result;
    }

    void workCurrent()
    {
        shared_ptr<Event> event;

        while (true) {
            event = queue.front();
            if (event->t != state->t)
                return;
            queue.pop_front();
            tick(event);
        }
    }

    void work()
    {
        shared_ptr<Event> event;

        while (true) {
            event = queue.front();
            queue.pop_front();

            if (event->t >= state->duration) {
                state->t = state->duration;
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
        else if (event->type == EVENT_DEBUFF_GAIN)
            onDebuffGain(event->debuff);
        else if (event->type == EVENT_DEBUFF_EXPIRE)
            onDebuffExpire(event->debuff);
        else if (event->type == EVENT_DOT)
            onDot(event->dot);
        else if (event->type == EVENT_CD_GAIN)
            onCooldownGain(event->cooldown);
        else if (event->type == EVENT_CD_EXPIRE)
            onCooldownExpire(event->cooldown);
        else if (event->type == EVENT_MANA_SPRING)
            onManaSpring();
        else if (event->type == EVENT_VAMPIRIC_TOUCH)
            onVampiricTouch(event->mana);
        else if (event->type == EVENT_DRUMS)
            useDrums();
        else if (event->type == EVENT_INNERVATE)
            innervate();
        else if (event->type == EVENT_POWER_INFUSION)
            powerInfusion();
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

    void pushManaSpring(double t = 2)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_MANA_SPRING;
        event->t = t;
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

    void pushBuffExpire(shared_ptr<buff::Buff> buff, double t = 0)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_EXPIRE;
        event->t = t == 0 ? buff->duration : t;
        event->buff = buff;

        push(event);
    }

    void pushDebuffGain(shared_ptr<debuff::Debuff> debuff, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_DEBUFF_GAIN;
        event->t = t;
        event->debuff = debuff;

        push(event);
    }

    void pushDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_DEBUFF_EXPIRE;
        event->t = debuff->duration;
        event->debuff = debuff;

        push(event);
    }

    void pushDot(shared_ptr<dot::Dot> dot)
    {
        if (!dot->stackable && dot->tick == 0)
            removeDot(dot);

        shared_ptr<Event> event(new Event());
        event->type = EVENT_DOT;
        event->t = dot->t_interval;
        event->dot = dot;

        push(event);
    }

    void pushCooldownGain(shared_ptr<cooldown::Cooldown> cooldown, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CD_GAIN;
        event->t = t;
        event->cooldown = cooldown;

        push(event);
    }

    void pushCooldownExpire(shared_ptr<cooldown::Cooldown> cooldown)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CD_EXPIRE;
        event->t = cooldown->duration;
        event->cooldown = cooldown;

        push(event);
    }

    void pushDrums(double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_DRUMS;
        event->t = t;

        push(event);
    }

    void pushInnervate(double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_INNERVATE;
        event->t = t;

        push(event);
    }

    void pushPowerInfusion(double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_POWER_INFUSION;
        event->t = t;

        push(event);
    }

    void pushWait(double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_WAIT;
        event->t = t;

        push(event);

        ostringstream s;
        s << std::fixed << std::setprecision(2);
        s << "Out of mana, waiting " << t << " seconds...";
        addLog(LOG_WAIT, s.str());
    }

    void cast(shared_ptr<spell::Spell> spell)
    {
        if (canCast(spell)) {
            if (state->t_gcd > state->t) {
                pushCast(spell, state->t_gcd - state->t);
                if (!state->was_instant && !spell->proc) {
                    state->t_gcd_capped+= state->t_gcd - state->t;
                    logGCD(state->t_gcd - state->t);
                }
            }
            else {
                useCooldowns();

                if (!spell->proc)
                    state->t_gcd = state->t + gcd();

                if (spell->channeling)
                    onCast(spell);
                else
                    pushSpell(spell, castTime(spell));
            }
        }
        else {
            pushWait(0.5);
        }
    }

    void onCast(shared_ptr<spell::Spell> spell)
    {
        shared_ptr<spell::Spell> next = NULL;

        if (spell->tick || canCast(spell)) {
            if (spell->channeling && !spell->tick) {
                onCastSuccess(spell);

                double cast_time = castTime(spell);
                for (int i=1; i<=spell->ticks; i++)
                    pushSpell(spell, cast_time / spell->ticks * i);

                spell->tick++;
                return;
            }

            if (spell->aoe) {
                for (int i=0; i<config->targets; i++)
                    onCastDmg(spell);
            }
            else {
                onCastDmg(spell);
            }

            if (!spell->channeling)
                onCastSuccess(spell);

            if (!spell->done || spell->proc)
                return;

            next = onCastComplete(spell);
        }
        else {
            next = defaultSpell();
        }

        if (next != NULL) {
            // Drums 1 sec cast
            if (config->drums && isTimerReady(config->drums_t) && !state->hasCooldown(cooldown::DRUMS) && !config->drums_friend) {
                useDrums();
                pushCast(next, 1.0);
            }
            else {
                cast(next);
            }
        }
    }

    void onCastSuccess(shared_ptr<spell::Spell> spell)
    {
        if (spell->proc)
            return;

        spell->actual_cost = manaCost(spell);
        state->mana-= spell->actual_cost;
        if (spell->actual_cost > 0)
            state->t_mana_spent = state->t;

        if (spell->cast_time == 0 || state->hasBuff(buff::PRESENCE_OF_MIND))
            state->was_instant = true;
        else if (state->was_instant)
            state->was_instant = false;

        if (state->spells.find(spell->id) == state->spells.end())
            state->spells[spell->id].name = spell->name;
        state->spells[spell->id].casts++;

        /**
         * Clearcast mechanics
         * */
        bool has_cc = state->hasBuff(buff::CLEARCAST);

        if (has_cc && spell->channeling)
            state->cc_snapshot = true;
        else if (state->cc_snapshot)
            state->cc_snapshot = false;

        if (has_cc && !spell->channeling && state->t_gcd - state->t < CC_SNAPSHOT_WINDOW)
            state->cc_queue = true;
        else if (state->cc_queue)
            state->cc_queue = false;

        // Expire clearcast 10ms after cast
        if (has_cc)
            pushBuffExpire(make_shared<buff::Clearcast>(), CC_SNAPSHOT_WINDOW);

        clearcast();

        if (state->hasBuff(buff::PRESENCE_OF_MIND))
            onBuffExpire(make_shared<buff::PresenceOfMind>());

        if (spell->actual_cost > 0 && state->hasBuff(buff::PENDANT_VIOLET_EYE))
            onBuffGain(make_shared<buff::Enlightenment>());

        if (spell->id == spell::FIRE_BLAST)
            onCooldownGain(make_shared<cooldown::FireBlast>(player->talents.imp_fire_blast));

        // 5% proc rate
        if (config->meta_gem == META_INSIGHTFUL_EARTHSTORM && random<int>(0, 19) == 0 && !state->hasCooldown(cooldown::INSIGHTFUL_EARTHSTORM)) {
            onCooldownGain(make_shared<cooldown::InsightfulEarthstorm>());
            onManaGain(300, "Mana Restore (meta)");
        }
        // 15% proc rate
        if (config->meta_gem == META_MYSTICAL_SKYFIRE && random<int>(0, 19) < 3 && !state->hasCooldown(cooldown::MYSTICAL_SKYFIRE)) {
            onCooldownGain(make_shared<cooldown::MysticalSkyfire>());
            onBuffGain(make_shared<buff::MysticalSkyfire>());
        }
        // 2% proc rate
        if (hasTrinket(TRINKET_BLUE_DRAGON) && random<int>(0, 49) == 0)
            onBuffGain(make_shared<buff::BlueDragon>());
        // 10% proc rate
        if (hasTrinket(TRINKET_QUAGMIRRANS_EYE) && !state->hasCooldown(cooldown::QUAGMIRRANS_EYE) && random<int>(0, 9) == 0) {
            onCooldownGain(make_shared<cooldown::QuagmirransEye>());
            onBuffGain(make_shared<buff::QuagmirransEye>());
        }
    }

    void onCastDmg(shared_ptr<spell::Spell> spell)
    {
        if (state->spells.find(spell->id) == state->spells.end())
            state->spells[spell->id].name = spell->name;

        spell->done = true;
        spell->result = spellRoll(spell);
        if (spell->result == spell::MISS) {
            spell->misses++;
            state->spells[spell->id].misses++;
        }
        else if (spell->result == spell::CRIT) {
            spell->crits++;
            state->spells[spell->id].crits++;
        }
        else {
            spell->hits++;
            state->spells[spell->id].hits++;
        }

        if (spell->result != spell::MISS) {
            spell->dmg = spellDmg(spell);

            if (spell->result == spell::CRIT)
                spell->dmg*= critMultiplier(spell);

            spell->resist = spellDmgResist(spell);
            spell->dmg-= spell->resist;

            spell->resist = round(spell->resist);
            spell->dmg = round(spell->dmg);

            state->dmg+= spell->dmg;

            state->spells[spell->id].dmg+= spell->dmg;
            if (spell->dmg > state->spells[spell->id].max_dmg)
                state->spells[spell->id].max_dmg = spell->dmg;
            if (spell->dmg < state->spells[spell->id].min_dmg || state->spells[spell->id].min_dmg == 0)
                state->spells[spell->id].min_dmg = spell->dmg;
        }

        logSpellDmg(spell);

        if (spell->id == spell::ENGULFING_SHADOWS)
            onManaGain(100, "Engulfing Shadows");

        if (spell->proc)
            return;

        if (spell->result == spell::MISS) {
            if (hasTrinket(TRINKET_EYE_OF_MAGTHERIDON))
                onBuffGain(make_shared<buff::EyeOfMagtheridon>());
        }
        else {
            if (spell->school == SCHOOL_FIRE && state->hasBuff(buff::COMBUSTION)) {
                if (spell->result == spell::CRIT)
                    state->combustion++;
                if (state->combustion == 3) {
                    onCooldownGain(make_shared<cooldown::Combustion>());
                    onBuffExpire(make_shared<buff::Combustion>());
                    state->combustion = 0;
                }
                else {
                    onBuffGain(make_shared<buff::Combustion>());
                }
            }

            if (spell->id == spell::SCORCH && player->talents.imp_scorch) {
                if (player->talents.imp_scorch == 3 || random<int>(0, 2) < player->talents.imp_scorch) {
                    onDebuffGain(make_shared<debuff::FireVulnerability>());

                    // Sources say scorch procs trinket twice, I think it's because of fire vuln.
                    if (hasTrinket(TRINKET_DARKMOON_CRUSADE))
                        onBuffGain(make_shared<buff::DarkmoonCrusade>());
                }
            }

            if (spell->school == SCHOOL_FROST && player->talents.winters_chill) {
                if (player->talents.winters_chill == 5 || random<int>(0, 4) < player->talents.winters_chill)
                    onDebuffGain(make_shared<debuff::WintersChill>());
            }

            if (spell->id == spell::FIREBALL)
                pushDot(make_shared<dot::Fireball>());
            if (spell->id == spell::PYROBLAST)
                pushDot(make_shared<dot::Pyroblast>());

            // 10% proc rate
            if (config->blade_of_eternal_darkness && random<int>(0, 9) == 0) {
                cast(make_shared<spell::EngulfingShadows>());
            }
            // 15% proc rate
            if (hasTrinket(TRINKET_MARK_OF_DEFIANCE) && !state->hasCooldown(cooldown::MARK_OF_DEFIANCE) && random<int>(0, 99) < 15) {
                onCooldownGain(make_shared<cooldown::MarkOfDefiance>());
                onManaGain(random<double>(128, 173), "Mana Restore (Mark of Defiance)");
            }
            // 5% proc rate ?
            if (config->spellstrike_set && random<int>(0, 19) == 0)
                onBuffGain(make_shared<buff::Spellstrike>());
            // 10% proc rate
            if (config->eternal_sage && !state->hasCooldown(cooldown::ETERNAL_SAGE) && random<int>(0, 9) == 0) {
                onCooldownGain(make_shared<cooldown::EternalSage>());
                onBuffGain(make_shared<buff::EternalSage>());
            }
            // 15% proc rate
            if (config->blade_of_wizardry && !state->hasCooldown(cooldown::FORGOTTEN_KNOWLEDGE) && random<int>(0, 19) < 3) {
                onCooldownGain(make_shared<cooldown::ForgottenKnowledge>());
                onBuffGain(make_shared<buff::ForgottenKnowledge>());
            }
            // 20% proc rate
            if (config->robe_elder_scribes && !state->hasCooldown(cooldown::POWER_OF_ARCANAGOS) && random<int>(0, 4) == 0) {
                onCooldownGain(make_shared<cooldown::PowerOfArcanagos>());
                onBuffGain(make_shared<buff::PowerOfArcanagos>());
            }
            // 5% proc rate, cannot refresh itself while up
            if (config->wrath_of_cenarius && !state->hasCooldown(cooldown::SPELL_BLASTING) && random<int>(0, 19) == 0) {
                onCooldownGain(make_shared<cooldown::SpellBlasting>());
                onBuffGain(make_shared<buff::SpellBlasting>());
            }
            // 2% proc rate, mana-etched 4-set bonus
            if (config->mana_etched_4set && random<int>(0, 49) == 0) {
                onBuffGain(make_shared<buff::SpellPowerBonus>());
            }
            // 50% proc rate
            if (config->judgement_of_wisdom && random<int>(0, 1) == 1)
                onManaGain(74, "Judgement of Wisdom");

            if (hasTrinket(TRINKET_DARKMOON_CRUSADE))
                onBuffGain(make_shared<buff::DarkmoonCrusade>());

            // 15% proc rate
            if (config->sunwell_neck_scryer && !state->hasCooldown(cooldown::ARCANE_BOLT) && random<int>(0, 19) < 3) {
                onCooldownGain(make_shared<cooldown::ArcaneBolt>());
                cast(make_shared<spell::ArcaneBolt>());
            }

            // 15% proc rate
            if (config->sunwell_neck_aldor && !state->hasCooldown(cooldown::LIGHTS_WRATH) && random<int>(0, 19) < 3) {
                onBuffGain(make_shared<buff::LightsWrath>());
                onCooldownGain(make_shared<cooldown::LightsWrath>());
            }

            if (spell->result == spell::CRIT) {
                // 20% proc rate
                if (hasTrinket(TRINKET_UNSTABLE_CURRENTS) && !state->hasCooldown(cooldown::UNSTABLE_CURRENTS) && random<int>(0, 4) == 0) {
                    onCooldownGain(make_shared<cooldown::UnstableCurrents>());
                    onBuffGain(make_shared<buff::UnstableCurrents>());
                }
                // 20% proc rate
                if (hasTrinket(TRINKET_NEXUS_HORN) && !state->hasCooldown(cooldown::CALL_OF_THE_NEXUS) && random<int>(0, 4) == 0) {
                    onCooldownGain(make_shared<cooldown::CallOfTheNexus>());
                    onBuffGain(make_shared<buff::CallOfTheNexus>());
                }
                // 100% proc rate
                if (hasTrinket(TRINKET_LIGHTNING_CAPACITOR) && !state->hasCooldown(cooldown::LIGHTNING_CAPACITOR))
                    onBuffGain(make_shared<buff::LightningCapacitor>());
                // 50% proc rate
                if (hasTrinket(TRINKET_ASHTONGUE_TALISMAN) && random<int>(0, 1) == 0) {
                    if (!config->bis_ashtongue)
                        pushBuffGain(make_shared<buff::AshtongueTalisman>(), 0.01);
                    else
                        onBuffGain(make_shared<buff::AshtongueTalisman>());
                }

                if (config->tirisfal_4set)
                    onBuffGain(make_shared<buff::ArcaneMadness>());

                if (spell->school == SCHOOL_FIRE && player->talents.ignite)
                    addIgnite(spell);
                if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROST) && player->talents.master_of_elements)
                    onManaGain(spell->cost * 0.1 * player->talents.master_of_elements, "Master of Elements");
            }
        }

        if (spell->channeling) {
            spell->done = spell->tick == spell->ticks;
            spell->tick++;
        }
    }

    shared_ptr<spell::Spell> onCastComplete(shared_ptr<spell::Spell> spell)
    {
        shared_ptr<spell::Spell> next = NULL;

        if (spell->id == spell::ARCANE_BLAST)
            onBuffGain(make_shared<buff::ArcaneBlast>());

        if (shouldUseManaGem())
            useManaGem();
        if (shouldUseManaPotion())
            useManaPotion();

        if (shouldEvocate()) {
            evocate();
            return NULL;
        }

        if (shouldInnervate())
            innervate();

        if (shouldSymbolOfHope()) {
            onCooldownGain(make_shared<cooldown::SymbolOfHope>());
            onBuffGain(make_shared<buff::SymbolOfHope>());
        }

        next = nextSpell(spell);

        return next;
    }

    void onManaRegen()
    {
        onManaGain(manaPerTick(), "Mana Regen");
        pushManaRegen();
    }

    void onManaGain(double mana, string source = "")
    {
        state->mana = min(player->maxMana(), state->mana + mana);
        logManaGain(mana, source);
    }

    void onManaSpring()
    {
        onManaGain(manaSpringPerTick(), "Mana Spring");
        pushManaSpring();
    }

    void onVampiricTouch(double mana)
    {
        onManaGain(mana, "Vampiric Touch");
        pushVampiricTouch(mana);
    }

    void onDot(shared_ptr<dot::Dot> dot)
    {
        state->dmg+= dot->dmg;
        logDotDmg(dot);

        if (hasTrinket(TRINKET_TIMBALS_FOCUSING_CRYSTAL) && !state->hasCooldown(cooldown::TIMBALS_SHADOW_BOLT) && random<int>(0, 9) == 0) {
            onCooldownGain(make_shared<cooldown::TimbalsShadowBolt>());
            cast(make_shared<spell::TimbalsShadowBolt>());
        }

        dot->onTick();

        if (dot->tick < dot->ticks)
            pushDot(dot);
    }

    void onWait()
    {
        shared_ptr<spell::Spell> spell = defaultSpell();

        if (spell->id == spell::ARCANE_BLAST && state->hasBuff(buff::ARCANE_BLAST)) {
            double t = buffDuration(buff::ARCANE_BLAST) - castTime(spell);
            double stacks = state->buffStacks(buff::ARCANE_BLAST);
            if (stacks > 1 && t > 0.0 && timeRemain() > 5.0) {
                t+= 0.1;
                pushCast(spell, t);

                ostringstream s;
                s << std::fixed << std::setprecision(2);
                s << "AB stacks up, waiting another " << t << " seconds...";
                addLog(LOG_WAIT, s.str());

                return;
            }
        }

        cast(spell);
    }

    void onBuffGain(shared_ptr<buff::Buff> buff)
    {
        int stacks = state->addBuff(buff);
        removeBuffExpiration(buff);
        pushBuffExpire(buff);

        if (buff->id == buff::MANA_TIDE) {
            for (double t=3; t<=12; t+= 3)
                pushManaGain(t, player->maxMana() * 0.06, "Mana Tide");

            // Hold mana spring while mana tide is rolling
            removeManaSpring();
            pushManaSpring(14);
        }

        if (buff->id == buff::DRUMS_OF_RESTORATION) {
            for (double t = 3; t<=15; t+= 3)
                pushManaGain(t, 120, "Drums of Restoration");
        }

        if (buff->id == buff::SYMBOL_OF_HOPE) {
            for (double t = 5; t<=15; t+= 5)
                pushManaGain(t, 333, "Symbol of Hope");
        }

        if (stacks)
            logBuffGain(buff, stacks);

        if (buff->id == buff::ARCANE_POWER && state->hasBuff(buff::POWER_INFUSION))
            onBuffExpire(make_shared<buff::PowerInfusion>());

        if (buff->id == buff::LIGHTNING_CAPACITOR && stacks == 3) {
            onBuffExpire(buff);
            fireLightningCapacitor();
        }
    }

    void onBuffExpire(shared_ptr<buff::Buff> buff)
    {
        removeBuffExpiration(buff);
        logBuffExpire(buff);
        state->removeBuff(buff->id);

        if (buff->id == buff::PENDANT_VIOLET_EYE) {
            removeBuffExpiration(make_shared<buff::Enlightenment>());
            state->removeBuff(buff::ENLIGHTENMENT);
        }
    }

    void onDebuffGain(shared_ptr<debuff::Debuff> debuff)
    {
        int stacks = state->addDebuff(debuff);
        removeDebuffExpiration(debuff);
        pushDebuffExpire(debuff);

        if (stacks)
            logDebuffGain(debuff, stacks);
    }

    void onDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        removeDebuffExpiration(debuff);
        logDebuffExpire(debuff);
        state->removeDebuff(debuff->id);
    }

    void onCooldownGain(shared_ptr<cooldown::Cooldown> cooldown)
    {
        removeCooldownExpiration(cooldown);
        state->addCooldown(cooldown);
        pushCooldownExpire(cooldown);
        if (cooldown->id == cooldown::POTION && !state->used_pot)
            state->used_pot = true;
    }

    void onCooldownExpire(shared_ptr<cooldown::Cooldown> cooldown)
    {
        state->removeCooldown(cooldown->id);
    }

    void useManaPotion()
    {
        if (nextPotion() == POTION_FEL_MANA)
            useFelManaPotion();
        else
            useRegularManaPotion();
    }

    void useRegularManaPotion()
    {
        double mana = round(random<double>(1800, 3000));

        if (hasTrinket(TRINKET_SORCERERS_ALCHEMIST_STONE) || hasTrinket(TRINKET_ALCHEMIST_STONE))
            mana*= 1.4;

        onManaGain(mana, "Mana Potion");
        onCooldownGain(make_shared<cooldown::Potion>());
    }

    void useFelManaPotion()
    {
        double mana = 400; // 3200 over 8 sec

        if (hasTrinket(TRINKET_SORCERERS_ALCHEMIST_STONE) || hasTrinket(TRINKET_ALCHEMIST_STONE))
            mana*= 1.4;

        for (double t = 3; t<=24; t+= 3)
            pushManaGain(t, mana, "Fel Mana");

        onBuffGain(make_shared<buff::FelMana>());
        onBuffGain(make_shared<buff::FelAche>());
        onCooldownGain(make_shared<cooldown::Potion>());
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

        onManaGain(mana, "Mana Gem");
        onCooldownGain(make_shared<cooldown::Conjured>());

        if (hasTrinket(TRINKET_SERPENT_COIL))
            onBuffGain(make_shared<buff::SerpentCoil>());
    }

    double manaPercent()
    {
        return state->mana / player->maxMana() * 100.0;
    }

    double manaPerSecond()
    {
        double mps = player->staticManaPerSecond();
        double spi = player->spiritManaPerSecond();

        if (state->hasBuff(buff::ENLIGHTENMENT))
            mps+= 21.0/5.0 * state->buffStacks(buff::ENLIGHTENMENT);

        double while_casting = 0;
        if (state-> t - state->t_mana_spent >= 5.0) {
            while_casting = 1;
        }
        else if (state->hasBuff(buff::BLUE_DRAGON)) {
            while_casting = 1;
        }
        else if (state->hasBuff(buff::INNERVATE)) {
            while_casting = 1;
            spi*= 5;
        }
        else {
            if (player->talents.arcane_meditation)
                while_casting+= player->talents.arcane_meditation*0.1;
            if (config->mage_armor)
                while_casting+= 0.3;
        }

        mps+= while_casting * spi;

        return mps;
    }

    double manaPerTick()
    {
        return manaPerSecond() * 2;
    }

    double manaSpringPerTick()
    {
        double mpt = 20;

        if (config->improved_mana_spring)
            mpt*= 1.25;

        return mpt;
    }

    double totalManaPerSecond()
    {
        double mps = manaPerSecond();

        if (config->mana_spring)
            mps+= manaSpringPerTick() / 2.0;

        return mps;
    }

    bool canBlast()
    {
        // Use 3 stack AB as measure
        double cast_time = 1.5;
        double t_remain = timeRemain();
        double mps = totalManaPerSecond();
        double mana = state->mana;
        int stacks = 0;

        double mana_cost_extra = 0;
        if (config->tirisfal_2set)
            mana_cost_extra = 39;

        if (state->hasBuff(buff::ARCANE_BLAST) && buffDuration(buff::ARCANE_BLAST) > 1.5)
            stacks = state->buffStacks(buff::ARCANE_BLAST);

        if (stacks == 0) {
            // 1st cast
            mana-= 195 + mana_cost_extra;
            if (state->hasBuff(buff::ARCANE_BLAST)) {
                t_remain-= 1.5;
                mana+= mps*1.5;
            }
            else {
                t_remain-= 2.5;
                mana+= mps*2.5;
            }
            if (t_remain > 0 && mana < 0)
                return false;
        }

        if (stacks < 2) {
            // 2nd cast
            mana-= 341 + mana_cost_extra;
            mana+= mps * (2.5 - 1.0/3.0);
            t_remain-= 2.5 - 1.0/3.0;
            if (t_remain > 0 && mana < 0)
                return false;
        }

        if (stacks < 3) {
            // 3rd cast
            mana-= 487 + mana_cost_extra;
            mana+= mps * (2.5 - 2.0/3.0);
            t_remain-= 2.5 - 2.0/3.0;
            if (t_remain > 0 && mana < 0)
                return false;
        }

        double mana_cost = 633 + mana_cost_extra;
        double num_casts = floor(t_remain / cast_time);
        double t_lastcast = cast_time * (num_casts - 1);
        double total_mana_cost = num_casts*mana_cost;

        if (config->vampiric_touch)
            mps+= config->vampiric_touch_regen;
        if (state->hasBuff(buff::CLEARCAST))
            total_mana_cost-= mana_cost;

        // We subtract an extra mana cost because its better to go oom a little early than use another frostbolt
        total_mana_cost-= mana_cost;

        // Bet on another cleastcast
        if (num_casts >= 10)
            total_mana_cost-= mana_cost;

        double mana_lastcast = mana - total_mana_cost + mps*t_lastcast;

        return mana_lastcast >= 0;
    }

    double timeRemain()
    {
        return state->duration - state->t;
    }

    shared_ptr<spell::Spell> nextSpell(shared_ptr<spell::Spell> prev = NULL)
    {
        shared_ptr<spell::Spell> next = NULL;

        if (config->maintain_fire_vulnerability && player->talents.imp_scorch && shouldScorch())
            return make_shared<spell::Scorch>();

        if (isTimerReady(config->presence_of_mind_t) &&
            !state->hasCooldown(cooldown::PRESENCE_OF_MIND) &&
            player->talents.presence_of_mind &&
            player->talents.pyroblast)
        {
            return make_shared<spell::Pyroblast>();
        }

        // Cream
        if (config->cc_am_queue && state->hasBuff(buff::CLEARCAST)) {
            if (state->cc_queue)
                return make_shared<spell::ArcaneMissiles>();
            if (config->cc_am_repeat && prev != NULL && prev->id == spell::ARCANE_MISSILES)
                return make_shared<spell::ArcaneMissiles>();
        }

        if (config->fire_blast_weave && !state->hasCooldown(cooldown::FIRE_BLAST))
            return make_shared<spell::FireBlast>();

        if (config->main_rotation == MAIN_ROTATION_AB) {

            if (config->ab_haste_stop && 1.0 / (config->ab_haste_stop/100.0 + 1) >= castHaste()) {
                if (player->talents.imp_frostbolt < player->talents.imp_fireball)
                    return make_shared<spell::Fireball>();
                if (player->talents.empowered_arcane_missiles == 3)
                    return make_shared<spell::ArcaneMissiles>();
                return make_shared<spell::Frostbolt>();
            }

            if (canBlast())
                return defaultSpell();

            if (!state->regen_active) {
                bool regen_start = false;

                // Check timings
                for (int i=0; i<config->filler_start_t.size(); i++) {
                    if (state->t >= config->filler_start_t[i] && (config->filler_end_t.size() < i+1 || state->t < config->filler_end_t[i])) {
                        regen_start = true;
                        break;
                    }
                }

                // Check mana threshold
                if (!regen_start) {
                    // Check evocation timing
                    if (state->hasCooldown(cooldown::EVOCATION) || config->evocation_at > state->t+8 || manaPercent() < 8) {
                        if (state->buffStacks(buff::ARCANE_BLAST) >= min(3, config->regen_ab_count) && !state->hasBuff(buff::INNERVATE)) {
                            double regen_at = config->regen_mana_at;
                            if (state->hasBuff(buff::BLOODLUST))
                                regen_at = min(regen_at, 10.0);

                            if (regen_at >= manaPercent())
                                regen_start = true;
                        }
                    }
                }

                if (regen_start) {
                    state->regen_active = true;
                    state->regen_cycle = 0;
                    if (state->regened_at < 0)
                        state->regened_at = state->t;
                }
            }

            if (state->regen_active) {
                bool is_done = false;

                if (config->regen_rotation == REGEN_ROTATION_FB) {
                    if (state->regen_cycle == 3 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle < 3)
                        next = make_shared<spell::Frostbolt>();
                    else if (state->regen_cycle == config->regen_ab_count + 2)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_FB11) {
                    if (state->regen_cycle == 3 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle < 3)
                        next = make_shared<spell::Frostbolt11>();
                    else if (state->regen_cycle == config->regen_ab_count + 2)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_AMFB) {
                    if (state->regen_cycle == 2 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle == 0)
                        next = make_shared<spell::ArcaneMissiles>();
                    else if (state->regen_cycle == 1)
                        next = make_shared<spell::Frostbolt>();
                    else if (state->regen_cycle == config->regen_ab_count + 1)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_SC) {
                    if (state->regen_cycle == 5 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle < 5)
                        next = make_shared<spell::Scorch>();
                    else if (state->regen_cycle == config->regen_ab_count + 4)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_SCFB) {
                    if (state->regen_cycle == 3 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle == 0)
                        next = make_shared<spell::Scorch>();
                    else if (state->regen_cycle < 3)
                        next = make_shared<spell::Fireball>();
                    else if (state->regen_cycle == config->regen_ab_count + 2)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_AMSC) {
                    if (state->regen_cycle == 2 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle == 0)
                        next = make_shared<spell::ArcaneMissiles>();
                    else if (state->regen_cycle == 1)
                        next = make_shared<spell::Scorch>();
                    else if (state->regen_cycle == config->regen_ab_count + 1)
                        is_done = true;
                }
                else if (config->regen_rotation == REGEN_ROTATION_AMAM) {
                    if (state->regen_cycle == 2 && !willDropArcaneBlast())
                        state->regen_cycle--;
                    if (state->regen_cycle < 2)
                        next = make_shared<spell::ArcaneMissiles>();
                    else if (state->regen_cycle == config->regen_ab_count + 1)
                        is_done = true;
                }

                if (is_done) {
                    state->regen_cycle = 0;
                    if (config->regen_stop_at <= manaPercent())
                        state->regen_active = false;
                }
                else {
                    state->regen_cycle++;
                }
            }
        }

        if (config->main_rotation == MAIN_ROTATION_AE && !canCast(defaultSpell()))
            next = make_shared<spell::ArcaneExplosion1>();

        if (next == NULL)
            next = defaultSpell();

        return next;
    }

    bool willDropArcaneBlast()
    {
        return !state->hasBuff(buff::ARCANE_BLAST) || buffDuration(buff::ARCANE_BLAST) <= castTime(make_shared<spell::ArcaneBlast>());
    }

    shared_ptr<spell::Spell> defaultSpell()
    {
        if (config->main_rotation == MAIN_ROTATION_AB)
            return make_shared<spell::ArcaneBlast>();
        if (config->main_rotation == MAIN_ROTATION_AE)
            return make_shared<spell::ArcaneExplosion>();
        if (config->main_rotation == MAIN_ROTATION_AM)
            return make_shared<spell::ArcaneMissiles>();
        if (config->main_rotation == MAIN_ROTATION_SC)
            return make_shared<spell::Scorch>();
        if (config->main_rotation == MAIN_ROTATION_FIB)
            return make_shared<spell::Fireball>();
        if (config->main_rotation == MAIN_ROTATION_FRB)
            return make_shared<spell::Frostbolt>();

        return NULL;
    }

    void addIgnite(shared_ptr<spell::Spell> spell)
    {
        double dmg = round(spell->dmg * 0.04 * player->talents.ignite);

        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DOT && (*itr)->dot->id == dot::IGNITE) {
                (*itr)->dot->stack(dmg);
                (*itr)->t = state->t + (*itr)->dot->t_interval;
                return;
            }
        }

        pushDot(make_shared<dot::Ignite>(dmg));
    }

    void useCooldowns()
    {
        if (isTimerReady(config->arcane_power_t) && !state->hasCooldown(cooldown::ARCANE_POWER) && player->talents.arcane_power)
            useArcanePower();
        if (isTimerReady(config->presence_of_mind_t) && !state->hasCooldown(cooldown::PRESENCE_OF_MIND) && player->talents.presence_of_mind)
            usePresenceOfMind();
        if (isTimerReady(config->icy_veins_t) && !state->hasCooldown(cooldown::ICY_VEINS) && player->talents.icy_veins)
            useIcyVeins();
        if (isTimerReady(config->cold_snap_t) && !state->hasCooldown(cooldown::COLD_SNAP) && player->talents.cold_snap)
            useColdSnap();
        if (isTimerReady(config->combustion_t) && !state->hasCooldown(cooldown::COMBUSTION) && !state->hasBuff(buff::COMBUSTION) && player->talents.combustion)
            useCombustion();
        if (isTimerReady(config->berserking_t) && !state->hasCooldown(cooldown::BERSERKING) && player->race == RACE_TROLL)
            useBerserking();

        if (!state->hasCooldown(cooldown::POTION) && nextPotion() != POTION_NONE && nextPotion() != POTION_MANA && isTimerReady(config->potion_t))
            usePotion();

        if (!state->hasCooldown(cooldown::CONJURED) && config->conjured != CONJURED_NONE) {
            if (config->conjured == CONJURED_MANA_GEM && isTimerReadyExplicit(config->conjured_t) || config->conjured != CONJURED_MANA_GEM && isTimerReady(config->conjured_t))
                useConjured();
        }

        if (!state->hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config->trinket1) && isTimerReady(config->trinket1_t))
            useTrinket(config->trinket1, cooldown::TRINKET1);

        if (!state->hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config->trinket2) && isTimerReady(config->trinket2_t))
            useTrinket(config->trinket2, cooldown::TRINKET2);
    }

    void useTrinket(Trinket trinket_id, cooldown::ID cd)
    {
        double duration = 120; // Most trinkets are 2 min
        shared_ptr<buff::Buff> buff = NULL;

        if (trinket_id == TRINKET_RESTRAINED_ESSENCE)
            buff = make_shared<buff::RestrainedEssence>();
        if (trinket_id == TRINKET_SILVER_CRESCENT)
           buff = make_shared<buff::SilverCrescent>();
        if (trinket_id == TRINKET_SMOKING_PIPE)
           buff = make_shared<buff::DarkIronPipe>();
        if (trinket_id == TRINKET_ESSENCE_MARTYR)
           buff = make_shared<buff::EssenceMartyr>();
        if (trinket_id == TRINKET_CRYSTAL_TALISMAN)
           buff = make_shared<buff::CrystalTalisman>();
        if (trinket_id == TRINKET_PENDANT_VIOLET_EYE)
           buff = make_shared<buff::PendantVioletEye>();
        if (trinket_id == TRINKET_SKULL_GULDAN)
           buff = make_shared<buff::SkullGuldan>();
        if (trinket_id == TRINKET_CRIMSON_SERPENT)
           buff = make_shared<buff::CrimsonSerpent>();
        if (trinket_id == TRINKET_SHRUNKEN_HEAD)
           buff = make_shared<buff::ShrunkenHead>();

        if (trinket_id == TRINKET_SCRYERS_BLOODGEM || trinket_id == TRINKET_XIRIS_GIFT) {
            buff = make_shared<buff::SpellPower>();
            duration = 90;
        }
        if (trinket_id == TRINKET_MQG) {
            buff = make_shared<buff::MindQuickening>();
            duration = 300;
        }
        if (trinket_id == TRINKET_VENGEANCE_ILLIDARI) {
            buff = make_shared<buff::VengeanceIllidari>();
            duration = 90;
        }
        if (trinket_id == TRINKET_NAARU_SLIVER) {
            buff = make_shared<buff::NaaruSliver>();
            duration = 90;
        }
        if (trinket_id == TRINKET_BURST_OF_KNOWLEDGE) {
            buff = make_shared<buff::BurstOfKnowledge>();
            duration = 900;
        }

        if (buff != NULL) {
            onBuffGain(buff);
            onCooldownGain(make_shared<cooldown::Cooldown>(cd, duration));
            if (trinketSharesCD(trinket_id))
                onCooldownGain(make_shared<cooldown::TrinketShared>(buff->duration));
        }
    }

    bool isTrinketOnSharedCD(Trinket trinket_id)
    {
        if (!trinketSharesCD(trinket_id))
            return false;
        return state->hasCooldown(cooldown::TRINKET_SHARED);
    }

    bool trinketSharesCD(Trinket trinket_id)
    {
        if (trinket_id == TRINKET_ESSENCE_MARTYR)
            return false;
        if (trinket_id == TRINKET_BURST_OF_KNOWLEDGE)
            return false;
        if (trinket_id == TRINKET_PENDANT_VIOLET_EYE)
            return false;

        return true;
    }

    void useDrums()
    {
        shared_ptr<buff::Buff> buff = NULL;

        if (config->drums == DRUMS_OF_BATTLE)
            buff = make_shared<buff::DrumsOfBattle>();
        else if (config->drums == DRUMS_OF_WAR)
            buff = make_shared<buff::DrumsOfWar>();
        else if (config->drums == DRUMS_OF_RESTORATION)
            buff = make_shared<buff::DrumsOfRestoration>();
        else
            return;

        if (config->drums_friend) {
            onCooldownGain(make_shared<cooldown::Drums>());
            onBuffGain(buff);
        }
        else {
            pushCooldownGain(make_shared<cooldown::Drums>(), 1.0);
            pushBuffGain(buff, 1.0);
        }
    }

    void usePotion()
    {
        if (nextPotion() == POTION_DESTRUCTION)
            onBuffGain(make_shared<buff::DestructionPotion>());
        else
            return;

        onCooldownGain(make_shared<cooldown::Potion>());
    }

    void useConjured()
    {
        double cd = 120;

        if (config->conjured == CONJURED_FLAME_CAP) {
            cd = 180;
            onBuffGain(make_shared<buff::FlameCap>());
        }
        else if (config->conjured == CONJURED_MANA_GEM) {
            useManaGem();
        }
        else {
            return;
        }

        onCooldownGain(make_shared<cooldown::Conjured>(cd));
    }

    void useArcanePower()
    {
        onCooldownGain(make_shared<cooldown::ArcanePower>());
        onBuffGain(make_shared<buff::ArcanePower>());
    }

    void usePresenceOfMind()
    {
        onCooldownGain(make_shared<cooldown::PresenceOfMind>());
        onBuffGain(make_shared<buff::PresenceOfMind>());
    }

    void useIcyVeins()
    {
        state->mana-= player->base_mana * 0.03;
        onCooldownGain(make_shared<cooldown::IcyVeins>());
        onBuffGain(make_shared<buff::IcyVeins>());
    }

    void useColdSnap()
    {
        onCooldownGain(make_shared<cooldown::ColdSnap>());
        addLog(LOG_NONE, "Casted Cold Snap");

        if (player->talents.icy_veins)
            useIcyVeins();
    }

    void useCombustion()
    {
        onBuffGain(make_shared<buff::Combustion>());
    }

    void useBerserking()
    {
        state->mana-= player->base_mana * 0.06;
        onCooldownGain(make_shared<cooldown::Berserking>());
        onBuffGain(make_shared<buff::Berserking>());
    }

    bool hasTrinket(Trinket trinket)
    {
        return config->trinket1 == trinket || config->trinket2 == trinket;
    }

    void removeManaSpring()
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_MANA_SPRING) {
                queue.erase(itr);
                return;
            }
        }
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

    void removeDebuffExpiration(shared_ptr<debuff::Debuff> debuff)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DEBUFF_EXPIRE && (*itr)->debuff->id == debuff->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    void removeCooldownExpiration(shared_ptr<cooldown::Cooldown> cooldown)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_CD_EXPIRE && (*itr)->cooldown->id == cooldown->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    void removeDot(shared_ptr<dot::Dot> dot)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DOT && (*itr)->dot->id == dot->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    double buffDuration(buff::ID id)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_BUFF_EXPIRE && (*itr)->buff->id == id)
                return (*itr)->t - state->t;
        }

        return 0;
    }

    double debuffDuration(debuff::ID id)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DEBUFF_EXPIRE && (*itr)->debuff->id == id)
                return (*itr)->t - state->t;
        }

        return 0;
    }

    bool canCast(shared_ptr<spell::Spell> spell)
    {
        return state->mana >= manaCost(spell);
    }

    double manaCost(shared_ptr<spell::Spell> spell, bool ignore_clearcast = false)
    {
        if (state->hasBuff(buff::CLEARCAST) && !ignore_clearcast)
            return 0;

        double multi = 1;
        double discount = 0;
        double cost;

        if (spell->id == spell::ARCANE_BLAST) {
            multi+= 0.75 * state->buffStacks(buff::ARCANE_BLAST);
            if (config->tirisfal_2set)
                multi+= 0.2;
        }

        if (state->hasBuff(buff::BURST_OF_KNOWLEDGE))
            discount+= 100;

        /*
         * Additive multipliers
         */
        if (spell->school == SCHOOL_FROST && player->talents.frost_channeling)
            multi-= player->talents.frost_channeling*0.05;

        if (spell->school == SCHOOL_FIRE && player->talents.pyromaniac)
            multi-= player->talents.pyromaniac*0.01;

        if (spell->id == spell::ARCANE_MISSILES && player->talents.empowered_arcane_missiles)
            multi+= player->talents.empowered_arcane_missiles * 0.02;

        if (state->hasBuff(buff::ARCANE_POWER))
            multi+= 0.3;

        cost = (spell->cost - discount) * multi;

        /*
         * Multiplicative multipliers
         */
        if ((spell->school == SCHOOL_FROST || spell->school == SCHOOL_FIRE) && player->talents.elemental_precision)
            cost*= (1.0 - player->talents.elemental_precision*0.01);

        if (state->hasBuff(buff::POWER_INFUSION))
            cost*= 0.8;

        return round(cost);
    }

    double gcd()
    {
        double t = 1.5;
        double cap = 1.0;

        t*= castHaste();

        if (t < cap && !config->gcd_unlocked)
            t = cap;

        return t;
    }

    double castTime(shared_ptr<spell::Spell> spell, bool ignore_pom = false)
    {
        if (state->hasBuff(buff::PRESENCE_OF_MIND) && !ignore_pom)
            return 0;

        double t = spell->cast_time;

        if (spell->id == spell::ARCANE_BLAST && state->hasBuff(buff::ARCANE_BLAST))
            t-= (state->buffStacks(buff::ARCANE_BLAST)/3.0);

        if (spell->id == spell::FROSTBOLT && player->talents.imp_frostbolt)
            t-= player->talents.imp_frostbolt*0.1;

        if (spell->id == spell::FIREBALL && player->talents.imp_fireball)
            t-= player->talents.imp_fireball*0.1;

        return t * castHaste();
    }

    double castHaste()
    {
        double haste = 1;
        double phaste = player->stats.haste;
        double rating = 0;

        if (state->hasBuff(buff::SKULL_GULDAN))
            rating+= 175;
        if (state->hasBuff(buff::QUAGMIRRANS_EYE))
            rating+= 320;
        if (state->hasBuff(buff::FORGOTTEN_KNOWLEDGE))
            rating+= 280;
        if (state->hasBuff(buff::MYSTICAL_SKYFIRE))
            rating+= 320;
        if (state->hasBuff(buff::MQG))
            rating+= 330;
        if (state->hasBuff(buff::ASHTONGUE_TALISMAN))
            rating+= 145;
        if (state->hasBuff(buff::DRUMS_OF_BATTLE))
            rating+= 80;

        if (rating)
            phaste+= hasteRatingToHaste(rating);

        haste+= phaste/100.0;

        if (state->hasBuff(buff::BLOODLUST))
            haste*= 1.3;
        if (state->hasBuff(buff::POWER_INFUSION))
            haste*= 1.2;
        if (state->hasBuff(buff::ICY_VEINS))
            haste*= 1.2;
        if (state->hasBuff(buff::BERSERKING))
            haste*= 1.1;

        return 1.0 / haste;
    }

    double hitChance(shared_ptr<spell::Spell> spell)
    {
        double hit = 83.0 + player->stats.hit;

        if (config->targets > 1)
            hit+= 11;

        if (spell->school == SCHOOL_ARCANE && player->talents.arcane_focus)
            hit+= player->talents.arcane_focus*2.0;

        // This is supposedly bugged for binary spells to give 2% hit each point
        // They say it was actually that way in TBC so we'll keep it like this for now
        if (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROST) {
            if (spell->binary && player->talents.elemental_precision)
                hit+= player->talents.elemental_precision*2.0;
            else if (player->talents.elemental_precision)
                hit+= player->talents.elemental_precision;
        }

        return min(hit, 99.0);
    }

    double critChance(shared_ptr<spell::Spell> spell)
    {
        double crit = player->stats.crit;
        double rating = 0;

        if (rating)
            crit+= critRatingToChance(rating);

        if (spell->id == spell::ARCANE_BLAST && player->talents.arcane_impact)
            crit+= player->talents.arcane_impact*2.0;
        if (spell->id == spell::SCORCH && player->talents.incinerate)
            crit+= player->talents.incinerate*2.0;
        if (spell->id == spell::FROSTBOLT && player->talents.empowered_frostbolt)
            crit+= player->talents.empowered_frostbolt*1.0;

        // Clearcast
        if (player->talents.arcane_potency && !spell->proc) {
            // Normal clearcast
            if (state->hasBuff(buff::CLEARCAST))
                crit+= player->talents.arcane_potency*10.0;
            // Snapshotted cc
            if (state->cc_snapshot && spell->channeling)
                crit+= player->talents.arcane_potency*10.0;
        }

        if (state->hasBuff(buff::COMBUSTION) && spell->school == SCHOOL_FIRE)
            crit+= state->buffStacks(buff::COMBUSTION)*10.0;
        if (state->hasBuff(buff::DESTRUCTION_POTION))
            crit+= 2.0;

        if (spell->school == SCHOOL_FIRE && player->talents.critical_mass)
            crit+= player->talents.critical_mass*2.0;
        if (spell->school == SCHOOL_FIRE && player->talents.pyromaniac)
            crit+= player->talents.pyromaniac*1.0;
        if (spell->school == SCHOOL_FROST && state->hasDebuff(debuff::WINTERS_CHILL))
            crit+= state->debuffStacks(debuff::WINTERS_CHILL)*2;

        return crit;
    }

    double critMultiplier(shared_ptr<spell::Spell> spell)
    {
        double base = 1.5;
        double talents = 1;

        if (config->meta_gem == META_CHAOTIC_SKYFIRE)
            base*= 1.03;

        if (spell->proc)
            return base;

        if (player->talents.spell_power)
            talents+= player->talents.spell_power*0.25;

        if (spell->school == SCHOOL_FROST && player->talents.ice_shards)
            talents+= player->talents.ice_shards*0.2;

        return (base - 1) * talents + 1;
    }

    double buffDmgMultiplier(shared_ptr<spell::Spell> spell)
    {
        double multi = 1;

        if (config->imp_sanctity)
            multi*= 1.02;
        for (int i=0; i<config->ferocious_inspiration; i++)
            multi*= 1.03;

        if (player->talents.arcane_instability)
            multi*= 1 + (player->talents.arcane_instability * 0.01);
        if (player->talents.playing_with_fire)
            multi*= 1 + (player->talents.playing_with_fire * 0.01);
        if (player->talents.piercing_ice && spell->school == SCHOOL_FROST)
            multi*= 1 + (player->talents.piercing_ice * 0.02);
        if (player->talents.arctic_winds && spell->school == SCHOOL_FROST)
            multi*= 1 + (player->talents.arctic_winds * 0.01);
        if (player->talents.fire_power && spell->school == SCHOOL_FIRE)
            multi*= 1 + (player->talents.fire_power * 0.02);
        // Below 20% - We'll estimate that to last 20% of duration
        if (player->talents.molten_fury && state->t / state->duration >= 0.8)
            multi*= 1 + (player->talents.molten_fury * 0.1);

        if (state->hasBuff(buff::ARCANE_POWER) && !spell->proc)
            multi*= 1.3;

        if (spell->id == spell::ARCANE_BLAST && config->tirisfal_2set)
            multi*= 1.2;

        if ((spell->id == spell::ARCANE_MISSILES || spell->id == spell::FROSTBOLT || spell->id == spell::FIREBALL) && config->tempest_4set)
            multi*= 1.05;

        return multi;
    }

    double debuffDmgMultiplier(shared_ptr<spell::Spell> spell)
    {
        double multi = 1;

        if (config->misery)
            multi*= 1.05;

        if (config->curse_of_elements && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FIRE || spell->school == SCHOOL_ARCANE)) {
            if (config->malediction)
                multi*= 1.13;
            else
                multi*= 1.1;
        }

        if (spell->school == SCHOOL_FIRE && state->hasDebuff(debuff::FIRE_VULNERABILITY))
            multi*= (1 + state->debuffStacks(debuff::FIRE_VULNERABILITY) * 0.03);

        return multi;
    }

    double spellDmg(shared_ptr<spell::Spell> spell)
    {
        double dmg;

        if (config->avg_spell_dmg)
            dmg = spell->avgDmg();
        else
            dmg = random<double>(spell->min_dmg, spell->max_dmg);

        if (spell->coeff) {
            double sp = player->stats.spell_power;
            double coeff = spell->coeff;

            if (spell->school == SCHOOL_ARCANE)
                sp+= player->stats.spell_power_arcane;
            if (spell->school == SCHOOL_FROST)
                sp+= player->stats.spell_power_frost;
            if (spell->school == SCHOOL_FIRE)
                sp+= player->stats.spell_power_fire;

            if (spell->school == SCHOOL_FIRE && state->hasBuff(buff::FLAME_CAP))
                sp+= 80.0;

            if (state->hasBuff(buff::ARCANE_MADNESS))
                sp+= 70.0;
            if (state->hasBuff(buff::SILVER_CRESCENT))
                sp+= 155.0;
            if (state->hasBuff(buff::DARK_IRON_PIPE))
                sp+= 155.0;
            if (state->hasBuff(buff::ESSENCE_MARTYR))
                sp+= 99.0;
            if (state->hasBuff(buff::SPELL_POWER))
                sp+= 150.0;
            if (state->hasBuff(buff::CRYSTAL_TALISMAN))
                sp+= 104.0;
            if (state->hasBuff(buff::VENGEANCE_ILLIDARI))
                sp+= 120.0;
            if (state->hasBuff(buff::SERPENT_COIL))
                sp+= 225.0;
            if (state->hasBuff(buff::SPELLSTRIKE))
                sp+= 92.0;
            if (state->hasBuff(buff::UNSTABLE_CURRENTS))
                sp+= 190.0;
            if (state->hasBuff(buff::EYE_OF_MAGTHERIDON))
                sp+= 170.0;
            if (state->hasBuff(buff::RESTRAINED_ESSENCE))
                sp+= 130.0;
            if (state->hasBuff(buff::CALL_OF_THE_NEXUS))
                sp+= 225.0;
            if (state->hasBuff(buff::ETERNAL_SAGE))
                sp+= 95.0;
            if (state->hasBuff(buff::SPELL_BLASTING))
                sp+= 132.0;
            if (state->hasBuff(buff::SPELL_POWER_BONUS))
                sp+= 110.0;
            if (state->hasBuff(buff::CRIMSON_SERPENT))
                sp+= 150.0;
            if (state->hasBuff(buff::SHRUNKEN_HEAD))
                sp+= 211.0;
            if (state->hasBuff(buff::NAARU_SLIVER))
                sp+= 320.0;
            if (state->hasBuff(buff::POWER_OF_ARCANAGOS))
                sp+= 130.0;
            if (state->hasBuff(buff::DRUMS_OF_WAR))
                sp+= 30.0;
            if (state->hasBuff(buff::DESTRUCTION_POTION))
                sp+= 120.0;
            if (state->hasBuff(buff::DARKMOON_CRUSADE))
                sp+= state->buffStacks(buff::DARKMOON_CRUSADE) * 8.0;
            if (state->hasBuff(buff::LIGHTS_WRATH))
                sp+= 120.0;

            if (state->hasBuff(buff::FEL_ACHE))
                sp-= 25.0;

            if (spell->id == spell::ARCANE_MISSILES && player->talents.empowered_arcane_missiles)
                coeff+= player->talents.empowered_arcane_missiles * 0.15;
            if (spell->id == spell::FIREBALL && player->talents.empowered_fireball)
                coeff+= player->talents.empowered_fireball * 0.03;
            if (spell->id == spell::FROSTBOLT && player->talents.empowered_frostbolt)
                coeff+= player->talents.empowered_frostbolt * 0.02;

            if (spell->channeling)
                coeff/= spell->ticks;

            dmg+= sp*coeff;
        }

        dmg*= buffDmgMultiplier(spell);

        if (spell->aoe && spell->aoe_cap > 0 && dmg > spell->aoe_cap/config->targets)
            dmg = spell->aoe_cap/config->targets;

        dmg*= debuffDmgMultiplier(spell);

        return dmg;
    }

    double spellDmgResist(shared_ptr<spell::Spell> spell)
    {
        if (spell->binary)
            return 0.0;

        // No confirmed formulas or resistance tables can be found
        // This resistance table is based on data from Karazhan in TBC Beta uploaded to WCL
        // It results in about 6% mitigation

        int resist[4] = {83, 11, 5, 1};
        int roll = random<int>(0, 99);

        double resistance_multiplier = 0.0;
        for (int i=0; i<4; i++) {
            if (roll < resist[i]) {
                resistance_multiplier = ((float) i) * 0.25;
                break;
            }

            roll-= resist[i];
        }

        if (!resistance_multiplier)
            return 0.0;

        return spell->dmg * resistance_multiplier;
    }

    spell::Result spellRoll(shared_ptr<spell::Spell> spell)
    {
        if (random<double>(0, 100) > hitChance(spell))
            return spell::MISS;

        if (random<double>(0, 100) <= critChance(spell))
            return spell::CRIT;

        return spell::HIT;
    }

    void powerInfusion()
    {
        if (state->hasBuff(buff::ARCANE_POWER)) {
            pushPowerInfusion(buffDuration(buff::ARCANE_POWER));
            return;
        }

        onCooldownGain(make_shared<cooldown::PowerInfusion>());
        onBuffGain(make_shared<buff::PowerInfusion>());
    }

    void innervate()
    {
        state->innervates--;
        onBuffGain(make_shared<buff::Innervate>());
    }

    void evocate()
    {
        double haste = castHaste();
        int ticks = 4;

        if (config->tempest_2set)
            ticks++;

        if (config->evo_ticks > 0 && config->evo_ticks < ticks)
            ticks = config->evo_ticks;

        state->regen_cycle = 0;
        state->regen_active = false;
        onCooldownGain(make_shared<cooldown::Evocation>());
        onBuffGain(make_shared<buff::Evocation>(castHaste(), ticks));

        for (double i=1; i<=ticks; i++)
            pushManaGain(i * haste * 2.0, player->maxMana()*0.15, "Evocation");

        shared_ptr<Event> event(new Event());
        event->type = EVENT_CAST;
        event->t = ticks * 2.0 * haste;
        event->spell = defaultSpell();
        push(event);

        state->evocated_at = state->t;
    }

    void clearcast()
    {
        double chance = player->talents.clearcast * 2;
        if (random<double>(0, 100) <= chance)
            onBuffGain(make_shared<buff::Clearcast>());
    }

    void fireLightningCapacitor()
    {
        onCooldownGain(make_shared<cooldown::LightningCapacitor>());
        cast(make_shared<spell::LightningCapacitor>());
    }

    bool shouldScorch()
    {
        if (!player->talents.imp_scorch)
            return false;

        int stacks = state->debuffStacks(debuff::FIRE_VULNERABILITY);

        if (config->fire_vulnerability)
            return stacks < 4;

        if (stacks < 5)
            return true;

        // Could calculate cast time for fb + scorch and check if we can scorch in time
        // but lets be realstic...
        return debuffDuration(debuff::FIRE_VULNERABILITY) <= 5.0;
    }

    bool shouldInnervate()
    {
        if (!state->innervates || state->hasBuff(buff::INNERVATE))
            return false;

        // If we haven't used all the timed innervates
        if (config->innervate - state->innervates < config->innervate_t.size())
            return false;

        if (manaPercent() < 40.0 && state->hasBuff(buff::ARCANE_POWER))
            return true;

        if (manaPercent() < 30.0)
            return true;

        return false;
    }

    bool shouldSymbolOfHope()
    {
        if (player->faction() != FACTION_ALLIANCE || !config->symbol_of_hope)
            return false;

        if (state->hasCooldown(cooldown::SYMBOL_OF_HOPE))
            return false;

        if (config->symbol_of_hope_at)
           return config->symbol_of_hope_at <= state->t;

        if (state->hasBuff(buff::INNERVATE) || state->hasBuff(buff::MANA_TIDE))
            return false;

        if (manaPercent() < 60.0 && state->hasCooldown(cooldown::POTION) && state->hasCooldown(cooldown::CONJURED))
            return true;

        if (manaPercent() < 40.0)
            return true;

        return false;
    }

    bool shouldEvocate()
    {
        if (state->hasCooldown(cooldown::EVOCATION))
            return false;

        if (config->evocation_at)
           return config->evocation_at <= state->t;

        if (state->hasBuff(buff::INNERVATE) || state->hasBuff(buff::MANA_TIDE))
            return false;

        if (manaPercent() > 20.0)
            return false;

        if (config->tempest_2set && manaPercent() > 10.0)
            return false;

        if (state->hasBuff(buff::BLOODLUST) && manaPercent() > 10.0)
            return false;

        if (config->main_rotation == MAIN_ROTATION_AB && canBlast())
            return false;

        return true;
    }

    bool shouldUseManaGem()
    {
        if (config->conjured != CONJURED_MANA_GEM || state->hasCooldown(cooldown::CONJURED) || state->hasBuff(buff::INNERVATE))
            return false;

        // Check for planned mana gem timings
        for (int i=0; i<config->conjured_t.size(); i++) {
            if (state->t - config->conjured_t.at(i) < 10)
                return false;
        }

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
        if (nextPotion() != POTION_MANA && nextPotion() != POTION_FEL_MANA)
            return false;

        if (state->hasCooldown(cooldown::POTION) || state->hasBuff(buff::INNERVATE))
            return false;

        if (!state->hasCooldown(cooldown::CONJURED) && config->conjured == CONJURED_MANA_GEM && config->conjured_t.size() > 0 && state->hasManaGem())
            return false;

        double max = 3000;

        if (state->hasBuff(buff::MANA_TIDE))
            max+= player->maxMana() * 0.06;

        // If gem is configured to be used within 15 sec, count with the mana gain to avoid overcapping
        if (!state->hasCooldown(cooldown::CONJURED) && config->conjured == CONJURED_MANA_GEM) {
            bool gem_soon = false;
            for (int i=0; i<config->conjured_t.size(); i++) {
                // 10 second margin for when it was supposed to be used
                if (state->t - config->conjured_t.at(i) < 10 && config->conjured_t.at(i) - state->t < 15) {
                    gem_soon = true;
                    break;
                }
            }

            if (gem_soon) {
                double gem = 2460;
                if (hasTrinket(TRINKET_SERPENT_COIL))
                    gem*= 1.25;
                max+= gem;
            }
        }

        return player->maxMana() - state->mana >= max;
    }

    Potion nextPotion()
    {
        if (state->used_pot || config->first_potion == POTION_NONE)
            return config->potion;
        return config->first_potion;
    }

    bool isTimerReady(vector<double>& v, double t = -1)
    {
        if (t == -1)
            t = state->t;

        for (int i=0; i<v.size(); i++) {
            if (v.at(i) > t)
                return false;
            // We give it a 20 second window to pop, otherwise it's an old timer
            if (v.at(i) <= t && v.at(i) + 20 > t)
                return true;
        }

        return true;
    }

    bool isTimerReadyExplicit(vector<double>& v, double t = -1)
    {
        if (t == -1)
            t = state->t;

        for (int i=0; i<v.size(); i++) {
            if (v.at(i) > t)
                return false;
            // We give it a 20 second window to pop, otherwise it's an old timer
            if (v.at(i) <= t && v.at(i) + 20 > t)
                return true;
        }

        return false;
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

        if (spell->resist)
            s << " (" << spell->resist << " resisted)";

        addLog(LOG_SPELL, s.str());
    }

    void logDotDmg(shared_ptr<dot::Dot> dot)
    {
        if (!logging)
            return;

        ostringstream s;

        s << dot->name << " ticks for " << dot->dmg;

        addLog(LOG_DOT, s.str());
    }

    void logBuffGain(shared_ptr<buff::Buff> buff, int stacks = 1)
    {
        if (!logging || buff->hidden)
            return;

        ostringstream s;

        s << "Gained " << buff->name;
        if (buff->max_stacks > 1)
            s << " (" << stacks << ")";

        addLog(LOG_BUFF, s.str());
    }

    void logBuffExpire(shared_ptr<buff::Buff> buff)
    {
        if (!logging || buff->hidden)
            return;

        ostringstream s;

        s << "Lost " << buff->name;

        addLog(LOG_BUFF, s.str());
    }

    void logDebuffGain(shared_ptr<debuff::Debuff> debuff, int stacks = 1)
    {
        if (!logging || debuff->hidden)
            return;

        ostringstream s;

        s << "Target gained " << debuff->name;
        if (debuff->max_stacks > 1)
            s << " (" << stacks << ")";

        addLog(LOG_BUFF, s.str());
    }

    void logDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        if (!logging || debuff->hidden)
            return;

        ostringstream s;

        s << "Target lost " << debuff->name;

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

    void logGCD(double t)
    {
        if (!logging)
            return;

        ostringstream s;

        s << fixed << setprecision(2);
        s << "Waited " << t << "s due to GCD cap.";

        addLog(LOG_GCD_CAP, s.str());
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

    string spellStats()
    {
        ostringstream s;

        s << "[";

        for (auto itr = state->spells.begin(); itr != state->spells.end(); itr++) {
            if (itr != state->spells.begin())
                s << ",";
            s << "{";
            s << "\"id\":" << itr->first << ",";
            s << "\"name\":\"" << itr->second.name << "\",";
            s << "\"casts\":" << itr->second.casts << ",";
            s << "\"misses\":" << itr->second.misses << ",";
            s << "\"hits\":" << itr->second.hits << ",";
            s << "\"crits\":" << itr->second.crits << ",";
            s << "\"min_dmg\":" << itr->second.min_dmg << ",";
            s << "\"max_dmg\":" << itr->second.max_dmg << ",";
            s << "\"dmg\":" << itr->second.dmg;
            s << "}";
        }

        s << "]";

        return s.str();
    }

};
