<template>
    <div id="app">
        <div class="main">
            <div class="actions">
                <div class="btn" @click="configToggle" :class="[is_running ? 'disabled' : '']">Config</div>
                <div class="btn" @click="runSingle" :class="[is_running ? 'disabled' : '']">Run</div>
                <div class="btn" @click="runMultiple" :class="[is_running ? 'disabled' : '']">Run {{ config.iterations }} times</div>
            </div>
            <template v-if="result && result.iterations">
                <div class="stats mt-2">
                    <div>DPS: {{ round(result.avg_dps) }} ({{ round(result.min_dps) }} - {{ round(result.max_dps) }})</div>
                </div>
            </template>
            <template v-else-if="result && result.t">
                <div class="stats mt-2" v-if="result.t">
                    <div>Damage: {{ result.dmg }}</div>
                    <div>DPS: {{ result.dps }}</div>
                </div>
                <div class="log mt-1" v-if="result.log">
                    <div class="form-item">
                        <label><input type="checkbox" v-model="log_filter.mana_ticks"> Show mana ticks</label>
                    </div>
                    <div class="log-wrapper">
                        <table>
                            <thead>
                                <th>T</th>
                                <th>Mana</th>
                                <th>DPS</th>
                                <th>Event</th>
                            </thead>
                            <tbody>
                                <tr v-for="log in result.log" v-if="showLog(log)">
                                    <td>{{ timeFormat(log.t) }}</td>
                                    <td>{{ round(log.mana) }} ({{ round(log.mana_percent) }}%)</td>
                                    <td>{{ round(log.dmg/log.t) }}</td>
                                    <td :class="[log.class ? log.class : '']">{{ log.text }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            </template>
        </div>

        <div class="config" v-if="config_open">
            <div class="fieldsets">
                <fieldset>
                    <legend>Basics</legend>
                    <div class="form-item">
                        <label>Race</label>
                        <select v-model="config.race">
                            <option value="RACE_BLOOD_ELF">Blood elf</option>
                            <option value="RACE_DRAENEI">Draenei</option>
                            <option value="RACE_GNOME">Gnome</option>
                            <option value="RACE_HUMAN">Human</option>
                            <option value="RACE_TROLL">Troll</option>
                            <option value="RACE_UNDEAD">Undead</option>
                        </select>
                    </div>
                    <div class="form-item">
                        <label>Spec</label>
                        <select v-model="config.spec">
                            <option value="SPEC_ARCANE">Arcane</option>
                        </select>
                    </div>
                    <div class="form-item">
                        <label>Talents (<a :href="talentsLink" target="_blank">link</a>)</label>
                        <input type="text" v-model="config.talents">
                    </div>
                    <div class="form-item" v-if="config.spec == 'arcane'">
                        <label>Regen rotation</label>
                        <select v-model="config.regen_rotation">
                            <option value="ROTATION_FB">3AB, 3FrB</option>
                            <option value="ROTATION_AMFB">3AB, AM, FrB</option>
                        </select>
                    </div>
                    <div class="form-item">
                        <label>Fight duration (sec)</label>
                        <input type="input" v-model.number="config.duration">
                    </div>
                    <div class="form-item">
                        <label>Number of sims</label>
                        <input type="input" v-model.number="config.iterations">
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Stats without talents</legend>
                    <div class="form-item">
                        <label>Intellect</label>
                        <input type="input" v-model.number="config.stats.intellect">
                    </div>
                    <div class="form-item">
                        <label>Spirit</label>
                        <input type="input" v-model.number="config.stats.spirit">
                    </div>
                    <div class="form-item">
                        <label>Critical Strike %</label>
                        <input type="input" v-model.number="config.stats.crit">
                    </div>
                    <div class="form-item">
                        <label>Hit %</label>
                        <input type="input" v-model.number="config.stats.hit">
                    </div>
                    <div class="form-item">
                        <label>Haste %</label>
                        <input type="input" v-model.number="config.stats.haste">
                    </div>
                    <div class="form-item">
                        <label>Spell Power</label>
                        <input type="input" v-model.number="config.stats.spell_power">
                    </div>
                    <div class="form-item">
                        <label>Extra Arcane Spell Power</label>
                        <input type="input" v-model.number="config.stats.spell_power_arcane">
                    </div>
                    <div class="form-item">
                        <label>Extra Frost Spell Power</label>
                        <input type="input" v-model.number="config.stats.spell_power_frost">
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Debuffs</legend>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.misery"> Misery</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.curse_of_elements"> Curse of Elements</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.judgement_of_the_crusader"> Imp. Judgement of the Crusader</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.judgement_of_wisdom"> Judgement of Wisdom</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.vampiric_touch"> Vampiric Touch</label>
                    </div>
                    <div class="form-item" v-if="config.vampiric_touch">
                        <label>Vampiric Touch mana/sec</label>
                        <input type="input" v-model.number="config.vampiric_touch_regen">
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Buffs</legend>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.arcane_intellect"> Arcane Intellect</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.mage_armor" @input="dontStack($event, 'buffs.molten_armor')"> Mage Armor</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.molten_armor" @input="dontStack($event, 'buffs.mage_armor')"> Molten Armor</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.divine_spirit"> Divine Spirit</label>
                    </div>
                    <div class="form-item" v-if="config.divine_spirit">
                        <label><input type="checkbox" v-model="config.improved_divine_spirit"> Imp. Divine Spirit</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.mark_of_the_wild"> Mark of the Wild</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.totem_of_wrath"> Totem of Wrath</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.wrath_of_air"> Wrath of Air Totem</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.mana_spring"> Mana Spring Totem</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.blessing_of_kings"> Blessing of Kings</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.blessing_of_wisdom"> Blessing of Wisdom</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.moonkin_aura"> Moonkin Aura</label>
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Cooldowns</legend>
                    <div class="form-item">
                        <label>Personal CDs at</label>
                        <input type="input" v-model.number="config.cooldowns_at">
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.bloodlust"> Bloodlust <template v-if="config.bloodlust">at</template></label>
                        <input type="input" v-model.number="config.bloodlust_at" v-if="config.bloodlust">
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.mana_tide"> Mana Tide <template v-if="config.mana_tide">at</template></label>
                        <input type="input" v-model.number="config.mana_tide_at" v-if="config.mana_tide">
                    </div>
                    <div class="form-item">
                        <label>Innervates</label>
                        <input type="input" v-model.number="config.innervate">
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Consumes</legend>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.spell_dmg_food" @input="dontStack($event, 'consumes.spell_crit_food')"> Spell damage food</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.spell_crit_food" @input="dontStack($event, 'consumes.spell_dmg_food')"> Spell crit food</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.brilliant_wizard_oil"> Brilliant Wizard Oil</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.flask_of_supreme_power" @input="dontStack($event, ['consumes.flask_of_blinding_light', 'consumes.adepts_elixir', 'consumes.elixir_of_draenic_wisdom'])"> Flask of Supreme Power</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.flask_of_blinding_light" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.adepts_elixir', 'consumes.elixir_of_draenic_wisdom'])"> Flask of Blinding Light</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.adepts_elixir" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.flask_of_blinding_light'])"> Adept's Elixir</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.elixir_of_draenic_wisdom" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.flask_of_blinding_light'])"> Elixir of Draenic Wisdom</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.drums"> Drums (WIP)</label>
                    </div>
                </fieldset>
                <fieldset>
                    <legend>Items</legend>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.t5_2set"> T5 2-set bonus</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.t5_4set"> T5 4-set bonus</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.serpent_coil"> Serpent Coil</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.silver_crescent"> Icon of the Silver Crescent</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.spellfire_set"> Spellfire set bonus</label>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="config.chaotic_skyfire"> Chaotic Skyfire meta gem</label>
                    </div>
                </fieldset>
            </div>
            <div class="close" @click="configToggle">X</div>
        </div>
    </div>
</template>

<script>
    import { SimulationWorker, SimulationWorkers } from "./simulation";

    export default {
        mounted() {
            this.loadConfig();
        },

        data() {
            return {
                result: null,
                is_running: false,
                config_open: false,
                log_filter: {
                    mana_ticks: false,
                },
                config: {
                    iterations: 200,
                    race: "RACE_UNDEAD",
                    spec: "SPEC_ARCANE",

                    duration: 200,
                    vampiric_touch_regen: 50,

                    misery: true,
                    curse_of_elements: true,
                    judgement_of_the_crusader: false,
                    judgement_of_wisdom: true,
                    vampiric_touch: true,

                    totem_of_wrath: true,
                    wrath_of_air: true,
                    mana_spring: true,
                    arcane_intellect: true,
                    divine_spirit: true,
                    improved_divine_spirit: false,
                    blessing_of_kings: true,
                    blessing_of_wisdom: true,
                    mark_of_the_wild: true,
                    moonkin_aura: false,
                    mage_armor: true,
                    molten_armor: false,

                    spell_dmg_food: true,
                    spell_crit_food: false,
                    brilliant_wizard_oil: true,
                    flask_of_supreme_power: true,
                    flask_of_blinding_light: false,
                    adepts_elixir: false,
                    elixir_of_draenic_wisdom: false,
                    drums: true,

                    t5_2set: true,
                    t5_4set: true,
                    spellfire_set: false,
                    chaotic_skyfire: true,
                    serpent_coil: true,
                    silver_crescent: true,

                    innervate: 0,
                    mana_tide: true,
                    bloodlust: true,

                    regen_mana_at: 20,
                    regen_rotation: "ROTATION_FB",
                    mana_tide_at: 10,
                    bloodlust_at: 10,
                    cooldowns_at: 10,

                    talents: "2500250300030150330125000000000000000000000000535000310030010000000",

                    stats: {
                        intellect: 0,
                        spirit: 0,
                        crit: 0,
                        hit: 0,
                        haste: 0,
                        spell_power: 0,
                        spell_power_arcane: 0,
                        spell_power_frost: 0,
                        spell_power_fire: 0,
                    }
                },
            };
        },

        computed: {
            talentsLink() {
                if (this.config.talents.match(/^[0-9]+$/))
                    return "https://tbcdb.com/talents/index.html?en&mage&"+this.config.talents;
                if (this.config.talents.match(/tbcdb\.com.*mage\&([0-9]+)/i))
                    return this.config.talents;
                return null;
            }
        },

        methods: {
            runMultiple() {
                var start = new Date;

                var self = this;
                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                    var stop = new Date;
                    console.log("Completed after "+(stop.getTime() - start.getTime())+"ms");
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                this.is_running = true;
                sim.start(this.config);
            },

            runSingle() {
                var self = this;
                var sim = new SimulationWorker((result) => {
                    self.is_running = false;
                    self.result = result;
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                this.is_running = true;
                sim.start(this.config);
            },

            dontStack(input, config) {
                if (!Array.isArray(config))
                    config = [config];

                if (input.target.checked) {
                    for (var i=0; i<config.length; i++) {
                        var key = config[i];
                        if (_.get(this.config, key)) {
                            _.set(this.config, key, false);
                        }
                    }
                }
            },

            showLog(log) {
                if (!log.filter)
                    return true;

                return this.log_filter[log.filter];
            },

            timeFormat(s) {
                var dec = Math.round((s%1) * 100);
                var sec = Math.floor(s);
                if (dec == 100) {
                    dec = 0;
                    sec++;
                }

                return (sec < 10 ? "0"+sec : sec)+"."+(dec < 10 ? "0"+dec : dec);
            },

            round(num) {
                return Math.round(num);
            },

            configToggle() {
                this.config_open = !this.config_open;
                if (!this.config_open)
                    this.saveConfig();
            },

            saveConfig() {
                window.localStorage.setItem("magesim_tbc_config", JSON.stringify(this.config));
            },

            loadConfig() {
                var str = window.localStorage.getItem("magesim_tbc_config");
                if (str) {
                    var config = JSON.parse(str);
                    if (config)
                        _.merge(this.config, config);
                }
            },
        }
    }
</script>