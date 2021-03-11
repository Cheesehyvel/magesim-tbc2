<template>
    <div id="app">
        <div class="wrapper">
            <div class="sidebar">
                <div class="actions">
                    <div class="btn block" @click="configToggle" :class="[is_running ? 'disabled' : '']">Config</div>
                    <div class="btn block mt-n" @click="runSingle" :class="[is_running ? 'disabled' : '']">Run</div>
                    <div class="btn block mt-n" @click="runMultiple" :class="[is_running ? 'disabled' : '']">Run {{ config.iterations }} times</div>
                </div>
                <div class="result" v-if="result">
                    <template v-if="result.iterations">
                        <div>DPS</div>
                        <div>{{ $round(result.avg_dps, 2) }}</div>
                        <div>{{ $round(result.min_dps, 2) }} - {{ $round(result.max_dps, 2) }}</div>
                    </template>
                    <template v-else>
                        <div>DPS</div>
                        <div>{{ $round(result.dps, 2) }}</div>
                        <div>Damage: {{ result.dmg }}</div>
                        <div class="btn mt-1" v-if="result.log" @click="logToggle">Combat log</div>
                    </template>
                </div>
            </div>
            <div class="main">
                <div class="gear">
                    <div class="slots">
                        <div
                            class="slot"
                            :class="[active_slot == slot ? 'active' : '']"
                            v-for="(i, slot) in equipped"
                            @click="active_slot = slot"
                        >{{ formatKey(slot) }}</div>
                    </div>
                    <div class="items">
                        <table>
                            <thead>
                                <th>Name</th>
                                <th>Spell power</th>
                                <th>Crit rating</th>
                                <th>Hit rating</th>
                                <th>Intellect</th>
                                <th>Spirit</th>
                            </thead>
                            <tbody>
                                <tr
                                    class="item"
                                    :class="[isEquipped(active_slot, item.id) ? 'active' : '']"
                                    v-for="item in activeItems"
                                    @click="equip(active_slot, item)"
                                >
                                    <td>{{ item.title }}</td>
                                    <td>{{ formatSP(item) }}</td>
                                    <td>{{ $get(item, "crit", 0) }}</td>
                                    <td>{{ $get(item, "hit", 0) }}</td>
                                    <td>{{ $get(item, "int", 0) }}</td>
                                    <td>{{ $get(item, "spirit", 0) }}</td>
                                </tr>
                            </tbody>
                        </table>

                        <table class="mt-4" v-if="activeEnchants.length">
                            <thead>
                                <th>Enchant</th>
                                <th>Spell power</th>
                                <th>Crit rating</th>
                                <th>Hit rating</th>
                                <th>Intellect</th>
                                <th>Spirit</th>
                            </thead>
                            <tbody>
                                <tr
                                    class="item"
                                    :class="[isEnchanted(active_slot, item.id) ? 'active' : '']"
                                    v-for="item in activeEnchants"
                                    @click="enchant(active_slot, item)"
                                >
                                    <td>{{ item.title }}</td>
                                    <td>{{ formatSP(item) }}</td>
                                    <td>{{ $get(item, "crit", 0) }}</td>
                                    <td>{{ $get(item, "hit", 0) }}</td>
                                    <td>{{ $get(item, "int", 0) }}</td>
                                    <td>{{ $get(item, "spirit", 0) }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>

                <div class="log" v-if="log_open">
                    <div class="log-wrapper">
                        <div class="filter">
                            <div class="form-item">
                                <label><input type="checkbox" v-model="log_filter[2]"> <span>Show mana gain</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="log_filter[3]"> <span>Show buffs</span></label>
                            </div>
                        </div>
                        <table>
                            <thead>
                                <th>Time</th>
                                <th>Mana</th>
                                <th>DPS</th>
                                <th>Event</th>
                            </thead>
                            <tbody>
                                <tr
                                    v-for="log in result.log"
                                    v-if="showLog(log)"
                                    :class="['type-'+log.type]"
                                >
                                    <td>{{ formatTime(log.t) }}</td>
                                    <td>{{ round(log.mana) }} ({{ round(log.mana_percent) }}%)</td>
                                    <td>{{ round(log.dmg/log.t) }}</td>
                                    <td>{{ log.text }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
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
                                <input type="text" v-model.number="config.duration">
                            </div>
                            <div class="form-item">
                                <label>Number of sims</label>
                                <input type="text" v-model.number="config.iterations">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Debuffs</legend>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.misery"> <span>Misery</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.curse_of_elements"> <span>Curse of Elements</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.judgement_of_the_crusader"> <span>Imp. Judgement of the Crusader</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.judgement_of_wisdom"> <span>Judgement of Wisdom</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.vampiric_touch"> <span>Vampiric Touch</span></label>
                            </div>
                            <div class="form-item" v-if="config.vampiric_touch">
                                <label>Vampiric Touch mana/sec</label>
                                <input type="text" v-model.number="config.vampiric_touch_regen">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Buffs</legend>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.arcane_intellect"> <span>Arcane Intellect</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mage_armor" @input="dontStack($event, 'buffs.molten_armor')"> <span>Mage Armor</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.molten_armor" @input="dontStack($event, 'buffs.mage_armor')"> <span>Molten Armor</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.divine_spirit"> <span>Divine Spirit</span></label>
                            </div>
                            <div class="form-item" v-if="config.divine_spirit">
                                <label><input type="checkbox" v-model="config.improved_divine_spirit"> Imp. Divine Spirit</label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mark_of_the_wild"> <span>Mark of the Wild</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.totem_of_wrath"> <span>Totem of Wrath</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.wrath_of_air"> <span>Wrath of Air Totem</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mana_spring"> <span>Mana Spring Totem</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.blessing_of_kings"> <span>Blessing of Kings</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.blessing_of_wisdom"> <span>Blessing of Wisdom</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.moonkin_aura"> <span>Moonkin Aura</span></label>
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Cooldowns</legend>
                            <div class="form-item">
                                <label>Personal CDs at</label>
                                <input type="text" v-model.number="config.cooldowns_at">
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.bloodlust"> <span>Bloodlust <template v-if="config.bloodlust">at</template></span></label>
                                <input type="text" v-model.number="config.bloodlust_at" v-if="config.bloodlust">
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mana_tide"> <span>Mana Tide <template v-if="config.mana_tide">at</template></span></label>
                                <input type="text" v-model.number="config.mana_tide_at" v-if="config.mana_tide">
                            </div>
                            <div class="form-item">
                                <label>Innervates</label>
                                <input type="text" v-model.number="config.innervate">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Consumes</legend>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.spell_dmg_food" @input="dontStack($event, 'consumes.spell_crit_food')"> <span>Spell damage food</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.spell_crit_food" @input="dontStack($event, 'consumes.spell_dmg_food')"> <span>Spell crit food</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.brilliant_wizard_oil"> <span>Brilliant Wizard Oil</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.flask_of_supreme_power" @input="dontStack($event, ['consumes.flask_of_blinding_light', 'consumes.adepts_elixir', 'consumes.elixir_of_draenic_wisdom'])"> <span>Flask of Supreme Power</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.flask_of_blinding_light" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.adepts_elixir', 'consumes.elixir_of_draenic_wisdom'])"> <span>Flask of Blinding Light</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.adepts_elixir" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.flask_of_blinding_light'])"> <span>Adept's Elixir</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.elixir_of_draenic_wisdom" @input="dontStack($event, ['consumes.flask_of_supreme_power', 'consumes.flask_of_blinding_light'])"> <span>Elixir of Draenic Wisdom</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.drums"> <span>Drums (WIP)</span></label>
                            </div>
                        </fieldset>
                    </div>
                    <div class="close" @click="configToggle">X</div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
    import { SimulationWorker, SimulationWorkers } from "./simulation";
    import items from "./items";

    export default {
        mounted() {
            this.loadConfig();
            this.loadGear();
        },

        data() {
            var data = {
                items: items,
                equipped: {},
                enchants: {},
                gems: {},
                active_slot: "main_hand",
                result: null,
                is_running: false,
                config_open: false,
                log_open: false,
                log_filter: {
                    "0": true,
                    "1": true,
                    "2": true,
                    "3": true,
                    "4": true,
                    "5": true,
                },
                config: {
                    iterations: 10000,
                    race: "RACE_UNDEAD",
                    spec: "SPEC_ARCANE",

                    duration: 180,
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
                        intellect: 465,
                        spirit: 285,
                        crit: 20,
                        hit: 0,
                        haste: 0,
                        spell_power: 1000,
                        spell_power_arcane: 50,
                        spell_power_frost: 0,
                        spell_power_fire: 0,
                    }
                },
            };

            var slots = [
                "main_hand", "off_hand", "ranged",
                "head", "neck", "shoulder", "back", "chest", "wrist",
                "hands", "waist", "legs", "feet",
                "finger1", "finger2", "trinket1", "trinket2",
            ];

            for (var slot of slots) {
                var islot = slot;
                var i = 0;
                if (slot.indexOf("finger") === 0 || slot.indexOf("trinket") === 0) {
                    islot = slot.substr(0, slot.length-1);
                    i = parseInt(slot.substr(slot.length-1))-1;
                }
                data.equipped[slot] = _.get(data.items.equip, islot+"."+i+".id", null);
                data.enchants[slot] = _.get(data.items.enchants, islot+"."+i+".id", null);
                data.gems[slot] = null;
            }

            return data;
        },

        computed: {
            talentsLink() {
                if (this.config.talents.match(/^[0-9]+$/))
                    return "https://tbcdb.com/talents/index.html?en&mage&"+this.config.talents;
                if (this.config.talents.match(/tbcdb\.com.*mage\&([0-9]+)/i))
                    return this.config.talents;
                return null;
            },

            activeItems() {
                var slot = this.equipSlotToItemSlot(this.active_slot);

                return this.items.equip[slot];
            },

            activeEnchants() {
                var slot = this.equipSlotToItemSlot(this.active_slot);

                if (!this.items.enchants.hasOwnProperty(slot))
                    return [];

                return this.items.enchants[slot];
            },
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

                this.prepare();
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

                this.prepare();
                this.is_running = true;
                sim.start(this.config);
            },

            prepare() {
                this.itemStats();
                this.itemConfig();
            },

            equipSlotToItemSlot(slot) {
                if (slot.indexOf("finger") === 0)
                    slot = "finger";
                if (slot.indexOf("trinket") === 0)
                    slot = "trinket";

                return slot;
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

            baseStats() {
                // Undead default
                var stats = {
                    intellect: 149,
                    spirit: 150,
                    crit: 0.91,
                    hit: 0,
                    haste: 0,
                    spell_power: 0,
                    spell_power_arcane: 0,
                    spell_power_frost: 0,
                };

                if (this.config.race == "RACE_TROLL") {
                    stats.intellect = 147;
                    stats.spirit = 146;
                }
                if (this.config.race == "RACE_BLOOD_ELF") {
                    stats.intellect = 155;
                    stats.spirit = 144;
                }
                if (this.config.race == "RACE_DRAENEI") {
                    stats.intellect = 152;
                    stats.spirit = 147;
                }
                if (this.config.race == "RACE_GNOME") {
                    stats.intellect = 155;
                    stats.spirit = 145;
                }
                if (this.config.race == "RACE_HUMAN") {
                    stats.intellect = 151;
                    stats.spirit = 145;
                }

                return stats;
            },

            itemStats() {
                var stats = this.baseStats();

                var item_stats = {
                    int: 0,
                    spi: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    haste: 0,
                    sp_arcane: 0,
                    sp_frost: 0,
                    sp_fire: 0,
                };

                for (var key in this.equipped) {
                    var slot = this.equipSlotToItemSlot(key);
                    var item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item) {
                        for (var stat in item) {
                            if (item_stats.hasOwnProperty(stat))
                                item_stats[stat]+= item[stat];
                        }
                    }
                }

                for (var key in this.enchants) {
                    var slot = this.equipSlotToItemSlot(key);
                    var item = _.find(this.items.enchants[slot], {id: this.enchants[key]});
                    if (item) {
                        for (var stat in item) {
                            if (item_stats.hasOwnProperty(stat))
                                item_stats[stat]+= item[stat];
                        }
                    }
                }

                stats.intellect+= item_stats.int;
                stats.spirit+= item_stats.spi;
                stats.spell_power+= item_stats.sp;
                stats.spell_power_arcane+= item_stats.sp_arcane;
                stats.spell_power_frost+= item_stats.sp_frost;
                stats.crit+= this.critRatingToChance(item_stats.crit);
                stats.hit+= this.hitRatingToChance(item_stats.hit);
                stats.haste+= this.hasteRatingToHaste(item_stats.haste);
                stats.crit+= stats.intellect/80;

                stats.crit = _.round(stats.crit, 2);
                stats.hit = _.round(stats.hit, 2);
                stats.haste = _.round(stats.haste, 2);

                this.config.stats = stats;
            },

            itemConfig() {
                var num = this.numEquippedSet(this.items.ids.TIRISFAL_SET);
                this.config.t5_2set = num > 1;
                this.config.t5_4set = num > 3;

                this.config.spellfire_set = this.numEquippedSet(this.items.ids.SPELLFIRE_SET) > 2;
                this.config.serpent_coil = this.isEquipped("trinket", this.items.ids.SERPENT_COIL);
                this.config.silver_crescent = this.isEquipped("trinket", this.items.ids.SILVER_CRESCENT);
            },

            critRatingToChance(rating) {
                return rating / 22.08;
            },

            hitRatingToChance(rating) {
                return rating / 12.62;
            },

            hasteRatingToHaste(rating) {
                return rating / 15.75;
            },

            equip(slot, item) {
                if (slot == "two_hand") {
                    this.equipped.off_hand = null;
                    slot = "main_hand";
                }

                if (this.equipped[slot] == item.id)
                    this.equipped[slot] = null;
                else
                    this.equipped[slot] = item.id;

                this.saveGear();
            },

            isEquipped(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEquipped(slot+"1", id) || this.isEquipped(slot+"2", id);

                return _.get(this.equipped, slot) == id;
            },

            numEquippedSet(id) {
                var num = 0;

                for (var key in this.equipped) {
                    var slot = this.equipSlotToItemSlot(key);
                    var item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item && _.get(item, "itemset") == id)
                        num++;
                }

                return num;
            },

            enchant(slot, item) {
                if (slot == "two_hand") {
                    this.enchants.off_hand = null;
                    slot = "main_hand";
                }

                if (this.enchants[slot] == item.id)
                    this.enchants[slot] = null;
                else
                    this.enchants[slot] = item.id;

                this.saveGear();
            },

            isEnchanted(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEnchanted(slot+"1") || this.isEnchanted(slot+"2");

                return _.get(this.enchants, slot) == id;
            },

            formatKey(str) {
                return _.startCase(str);
            },

            formatSP(item) {
                var str = item.sp ? item.sp : 0;
                var extra = [];

                if (item.sp_arcane)
                    extra.push("+"+item.sp_arcane+" arc");
                if (item.sp_frost)
                    extra.push("+"+item.sp_frost+" frost");
                if (item.sp_fire)
                    extra.push("+"+item.sp_fire+" fire");

                if (extra.length)
                    str+= " / "+extra.join(" / ");

                return str;
            },

            formatTime(s) {
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

            showLog(log) {
                return this.log_filter[log.type];
            },

            configToggle() {
                this.log_open = false;
                this.config_open = !this.config_open;
                if (!this.config_open)
                    this.saveConfig();
            },

            logToggle() {
                this.config_open = false;
                this.log_open = !this.log_open;
            },

            saveGear() {
                window.localStorage.setItem("magesim_tbc_equipped", JSON.stringify(this.equipped));
                window.localStorage.setItem("magesim_tbc_enchants", JSON.stringify(this.enchants));
            },

            loadGear() {
                var str = window.localStorage.getItem("magesim_tbc_equipped");
                if (str) {
                    var equipped = JSON.parse(str);
                    if (equipped)
                        _.merge(this.equipped, equipped);
                }

                var str = window.localStorage.getItem("magesim_tbc_enchants");
                if (str) {
                    var enchants = JSON.parse(str);
                    if (enchants)
                        _.merge(this.enchants, enchants);
                }
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