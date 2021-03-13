<template>
    <div id="app">
        <a class="github" href="https://github.com/Cheesehyvel/magesim-tbc2" target="_blank"></a>

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
                                <tr>
                                    <th>Name</th>
                                    <th>Sockets</th>
                                    <th>Socket bonus</th>
                                    <th>Spell power</th>
                                    <th>Crit rating</th>
                                    <th>Hit rating</th>
                                    <th>Intellect</th>
                                    <th>Spirit</th>
                                    <th>Mp5</th>
                                </tr>
                            </thead>
                            <tbody>
                                <tr
                                    class="item"
                                    :class="[isEquipped(active_slot, item.id) ? 'active' : '']"
                                    v-for="item in activeItems"
                                    @click="equip(active_slot, item)"
                                >
                                    <td>
                                        {{ item.title }}
                                        <a :href="itemUrl(item)" target="_blank" @click.stop>
                                            <span class="material-icons ml-n">&#xe89e;</span>
                                        </a>
                                    </td>
                                    <td>
                                        <template v-if="item.sockets">
                                            <div class="socket-color" :class="['color-'+socket]" v-for="socket in item.sockets"></div>
                                        </template>
                                    </td>
                                    <td>
                                        <span v-if="item.bonus" :class="[hasSocketBonus(active_slot) ? 'socket-bonus' : '']">
                                            {{ formatStats(item.bonus) }}
                                        </span>
                                    </td>
                                    <td>{{ formatSP(item) }}</td>
                                    <td>{{ $get(item, "crit", "") }}</td>
                                    <td>{{ $get(item, "hit", "") }}</td>
                                    <td>{{ $get(item, "int", "") }}</td>
                                    <td>{{ $get(item, "spirit", "") }}</td>
                                    <td>{{ $get(item, "mp5", "") }}</td>
                                </tr>
                            </tbody>
                        </table>

                        <table class="mt-4" v-if="activeEnchants.length">
                            <thead>
                                <tr>
                                    <th>Enchant</th>
                                    <th>Spell power</th>
                                    <th>Crit rating</th>
                                    <th>Hit rating</th>
                                    <th>Intellect</th>
                                    <th>Spirit</th>
                                    <th>Mp5</th>
                                </tr>
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
                                    <td>{{ $get(item, "crit", "") }}</td>
                                    <td>{{ $get(item, "hit", "") }}</td>
                                    <td>{{ $get(item, "int", "") }}</td>
                                    <td>{{ $get(item, "spi", "") }}</td>
                                    <td>{{ $get(item, "mp5", "") }}</td>
                                </tr>
                            </tbody>
                        </table>

                        <div class="sockets mt-4" v-if="activeSockets.length">
                            <div class="socket" v-for="(socket, index) in activeSockets">
                                <div class="title">
                                    <span>Socket {{ (index+1) }}</span>
                                    <span class="socket-color" :class="['color-'+socket]"></span>
                                </div>
                                <table>
                                    <thead>
                                        <tr>
                                            <th>Gem</th>
                                            <th>Stats</th>
                                            <th>Unique</th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        <tr
                                            class="gem-color"
                                            :class="['color-'+gem.color, isSocketed(active_slot, gem.id, index) ? 'active' : '']"
                                            v-for="gem in activeGems(index)"
                                            @click="setSocket(active_slot, gem, index)"
                                        >
                                            <td>
                                                {{ gem.title }}
                                                <a :href="itemUrl(gem)" target="_blank" @click.stop>
                                                    <span class="material-icons ml-n">&#xe89e;</span>
                                                </a>
                                            </td>
                                            <td>{{ formatStats(gem) }}</td>
                                            <td><template v-if="gem.unique">Yes</template></td>
                                        </tr>
                                    </tbody>
                                </table>
                            </div>
                        </div>
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
                                    <td>{{ (log.t ? round(log.dmg/log.t) : "0") }}</td>
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
                                    <option :value="races.RACE_BLOOD_ELF">Blood elf</option>
                                    <option :value="races.RACE_DRAENEI">Draenei</option>
                                    <option :value="races.RACE_GNOME">Gnome</option>
                                    <option :value="races.RACE_HUMAN">Human</option>
                                    <option :value="races.RACE_TROLL">Troll</option>
                                    <option :value="races.RACE_UNDEAD">Undead</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Spec</label>
                                <select v-model="config.spec">
                                    <option :value="specs.SPEC_ARCANE">Arcane</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Talents (<a :href="talentsLink" target="_blank">link</a>)</label>
                                <input type="text" v-model="config.talents">
                            </div>
                            <div class="form-item" v-if="config.spec == specs.SPEC_ARCANE">
                                <label>Regen rotation</label>
                                <select v-model="config.regen_rotation">
                                    <option :value="regen_rotations.ROTATION_FB">3AB, 3FrB</option>
                                    <option :value="regen_rotations.ROTATION_AMFB">3AB, AM, FrB</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Regen rotation at mana %</label>
                                <input type="text" v-model.number="config.regen_mana_at">
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
                                <label><input type="checkbox" v-model="config.mage_armor" @input="dontStack($event, 'molten_armor')"> <span>Mage Armor</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.molten_armor" @input="dontStack($event, 'mage_armor')"> <span>Molten Armor</span></label>
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
                            <div class="form-item" v-if="hasTalent('presence_of_mind')">
                                <label>Presence of Mind at</label>
                                <input type="text" v-model.number="config.presence_of_mind_at">
                            </div>
                            <div class="form-item" v-if="hasTalent('arcane_power')">
                                <label>Arcane Power at</label>
                                <input type="text" v-model.number="config.arcane_power_at">
                            </div>
                            <div class="form-item" v-if="hasTalent('icy_veins')">
                                <label>Icy Veins at</label>
                                <input type="text" v-model.number="config.icy_veins_at">
                            </div>
                            <div class="form-item" v-if="hasTalent('cold_snap')">
                                <label>Cold Snap at</label>
                                <input type="text" v-model.number="config.cold_snap_at">
                            </div>
                            <div class="form-item" v-if="config.race == races.RACE_TROLL">
                                <label>Berserking at</label>
                                <input type="text" v-model.number="config.berserking_at">
                            </div>
                            <div class="form-item" v-if="hasUseTrinket(1)">
                                <label>Trinket #1 at</label>
                                <input type="text" v-model.number="config.trinket1_at">
                            </div>
                            <div class="form-item" v-if="hasUseTrinket(2)">
                                <label>Trinket #2 at</label>
                                <input type="text" v-model.number="config.trinket2_at">
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
                                <label><input type="checkbox" v-model="config.spell_dmg_food" @input="dontStack($event, 'spell_crit_food')"> <span>Spell damage food</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.spell_crit_food" @input="dontStack($event, 'spell_dmg_food')"> <span>Spell crit food</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.brilliant_wizard_oil"> <span>Brilliant Wizard Oil</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.flask_of_supreme_power" @input="dontStack($event, ['flask_of_blinding_light', 'adepts_elixir', 'elixir_of_draenic_wisdom'])"> <span>Flask of Supreme Power</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.flask_of_blinding_light" @input="dontStack($event, ['flask_of_supreme_power', 'adepts_elixir', 'elixir_of_draenic_wisdom'])"> <span>Flask of Blinding Light</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.adepts_elixir" @input="dontStack($event, ['flask_of_supreme_power', 'flask_of_blinding_light'])"> <span>Adept's Elixir</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.elixir_of_draenic_wisdom" @input="dontStack($event, ['flask_of_supreme_power', 'flask_of_blinding_light'])"> <span>Elixir of Draenic Wisdom</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.drums"> <span>Drums (WIP)</span></label>
                            </div>
                        </fieldset>
                    </div>
                    <div class="close" @click="configToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
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
                races: {
                    RACE_BLOOD_ELF: 0,
                    RACE_DRAENEI: 1,
                    RACE_GNOME: 2,
                    RACE_HUMAN: 3,
                    RACE_TROLL: 4,
                    RACE_UNDEAD: 5
                },
                specs: {
                    SPEC_ARCANE: 0,
                },
                regen_rotations: {
                    ROTATION_FB: 0,
                    ROTATION_AMFB: 1,
                },
                items: items,
                equipped: {},
                enchants: {},
                gems: {},
                active_slot: "weapon",
                result: null,
                is_running: false,
                config_open: false,
                log_open: false,
                log_filter: {
                    "0": true,
                    "1": true,
                    "2": false,
                    "3": true,
                    "4": true,
                    "5": true,
                },
                config: {
                    iterations: 10000,
                    race: 5,
                    spec: 0,

                    duration: 180,
                    vampiric_touch_regen: 40,

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

                    tirisfal_2set: true,
                    tirisfal_4set: true,
                    spellfire_set: false,
                    spellstrike_set: false,
                    meta_gem: 0,
                    trinket1: 0,
                    trinket2: 0,

                    innervate: 0,
                    mana_tide: true,
                    bloodlust: true,

                    regen_mana_at: 20,
                    regen_rotation: "ROTATION_FB",
                    mana_tide_at: 10,
                    bloodlust_at: 10,
                    icy_veins_at: 0,
                    cold_snap_at: 20,
                    trinket1_at: 0,
                    trinket2_at: 20,
                    berserking_at: 0,
                    arcane_power_at: 0,
                    presence_of_mind_at: 19,

                    talents: "2500250300030150330125000000000000000000000000535000310030010000000",

                    stats: {
                        intellect: 465,
                        spirit: 285,
                        mp5: 0,
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
                "weapon", "off_hand", "ranged",
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
                data.gems[slot] = [null, null, null];
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
                var slot = this.equipSlotToEnchantSlot(this.active_slot);

                if (!this.items.enchants.hasOwnProperty(slot))
                    return [];

                return this.items.enchants[slot];
            },

            activeSockets() {
                var item = this.equippedItem(this.active_slot);
                return item && item.sockets ? item.sockets : [];
            },
        },

        methods: {
            runMultiple() {
                var self = this;
                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                this.log_open = false;
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
                this.saveGear();
                this.saveConfig();
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

            equipSlotToEnchantSlot(slot) {
                return this.equipSlotToItemSlot(slot);
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

            equippedItem(slot) {
                var id = this.equipped[slot];
                if (!id)
                    return null;

                slot = this.equipSlotToItemSlot(slot);

                return _.find(this.items.equip[slot], {id: this.equipped[slot]}, null)
            },

            activeGems(index) {
                if (this.activeSockets.length < index)
                    return [];
                if (this.activeSockets[index] == "m")
                    return this.items.gems.filter(g => g.color == "m");
                return this.items.gems.filter(g => g.color != "m");
            },

            baseStats() {
                // Undead default
                var stats = {
                    intellect: 149,
                    spirit: 150,
                    mp5: 0,
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
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    haste: 0,
                    sp_arcane: 0,
                    sp_frost: 0,
                    sp_fire: 0,
                };

                var addStats = function(itm) {
                    for (var stat in itm) {
                        if (item_stats.hasOwnProperty(stat))
                            item_stats[stat]+= itm[stat];
                    }
                };

                var slot, item, has_bonus, get_bonus, enchant, gem, gem_id;
                for (var key in this.equipped) {
                    slot = this.equipSlotToItemSlot(key);
                    item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item) {
                        has_bonus = item.hasOwnProperty("bonus");
                        get_bonus = true;

                        addStats(item);

                        if (this.items.enchants[slot] && this.enchants[key]) {
                            enchant = _.find(this.items.enchants[slot], {id: this.enchants[key]});
                            if (enchant)
                                addStats(enchant);
                        }

                        if (item.sockets) {
                            for (var i=0; i<item.sockets.length; i++) {
                                gem_id = this.gems[slot][i];
                                gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                                if (gem)
                                    addStats(gem);
                                if (has_bonus && (!gem || !this.matchSocketColor(item.sockets[i], gem.color)))
                                    get_bonus = false;
                            }

                            if (has_bonus && get_bonus)
                                addStats(item.bonus);
                        }
                    }
                }

                stats.intellect+= item_stats.int;
                stats.spirit+= item_stats.spi;
                stats.mp5+= item_stats.mp5;
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
                this.config.tirisfal_2set = num > 1;
                this.config.tirisfal_4set = num > 3;

                this.config.spellstrike_set = this.numEquippedSet(this.items.ids.SPELLSTRIKE_SET) > 1;
                this.config.spellfire_set = this.numEquippedSet(this.items.ids.SPELLFIRE_SET) > 2;

                this.config.trinket1 = 0;
                this.config.trinket2 = 0;
                this.config.meta_gem = 0;
                if (this.isSpecialItem(this.equipped.trinket1))
                    this.config.trinket1 = this.equipped.trinket1;
                if (this.isSpecialItem(this.equipped.trinket2))
                    this.config.trinket2 = this.equipped.trinket2;
                if (this.metaGem() && this.isSpecialItem(this.metaGem().id))
                    this.config.meta_gem = this.metaGem().id;
            },

            itemUrl(item) {
                return "https://tbcdb.com/?item="+item.id;
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

            isSpecialItem(item_id) {
                for (var key in this.items.ids) {
                    if (this.items.ids[key] == item_id)
                        return true;
                }
                return false;
            },

            equip(slot, item) {
                if (slot == "weapon") {
                    if (item.twohand)
                        this.equipped.off_hand = null;
                }

                if (this.equipped[slot] == item.id)
                    this.equipped[slot] = null;
                else
                    this.equipped[slot] = item.id;

                this.gems[slot] = [null, null, null];

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

            setSocket(slot, item, index) {
                if (this.isSocketed(slot, item.id, index)) {
                    this.gems[slot].splice(index, 1, null);
                }
                else {
                    if (item.unique && this.isSocketedAnywhere(item.id))
                        return;
                    this.gems[slot].splice(index, 1, item.id);
                }

                this.saveGear();
            },

            matchSocketColor(sock, gem) {
                if (sock == gem)
                    return true;
                if (sock == "r" && ["o", "p"].indexOf(gem) != -1)
                    return true;
                if (sock == "y" && ["o", "g"].indexOf(gem) != -1)
                    return true;
                if (sock == "b" && ["g", "p"].indexOf(gem) != -1)
                    return true;
                return false;
            },

            hasSocketBonus(slot) {
                var item = this.equippedItem(slot);

                if (item && item.sockets && item.bonus) {
                    for (var i=0; i<item.sockets.length; i++) {
                        var gem_id = this.gems[slot][i];
                        var gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                        if (!gem || !this.matchSocketColor(item.sockets[i], gem.color))
                            return false;
                    }

                    return true;
                }

                return false;
            },

            isSocketed(slot, id, index) {
                return _.get(this.gems[slot], index) == id;
            },

            isSocketedAnywhere(id) {
                for (var slot in this.gems) {
                    for (var i=0; i<this.gems[slot].length; i++) {
                        if (id == this.gems[slot][i])
                            return true;
                    }
                }
                return false;
            },

            metaGem() {
                for (var key in this.gems.head) {
                    if (this.gems.head[key]) {
                        var gem = _.find(this.items.gems, {id: this.gems.head[key]});
                        if (gem && gem.color == "m")
                            return gem;
                    }
                }

                return null;
            },

            hasTalent(talent) {
                var indexes = {
                    presence_of_mind: 13,
                    arcane_power: 19,
                    icy_veins: 53,
                    cold_snap: 59,
                };

                var index = _.get(indexes, talent);
                if (!index)
                    return;

                var numbers = null;
                if (this.config.talents.match(/^[0-9]+$/)) {
                    numbers = this.config.talents;
                }
                else {
                    var m = this.config.talents.match(/tbcdb\.com.*mage\&([0-9]+)/i);
                    numbers = m[1];
                }

                if (numbers && numbers[index])
                    return parseInt(numbers[index]);

                return false;
            },

            hasUseTrinket(nr) {
                var slot = "trinket"+nr;
                var item = this.equippedItem(nr);

                if (item && item.use)
                    return true;

                return false;
            },

            formatStats(item) {
                var stats = [];

                if (item.int)
                    stats.push(item.int+" int");
                if (item.spi)
                    stats.push(item.spi+" spi");
                if (item.sp)
                    stats.push(item.sp+" sp");
                if (item.hit)
                    stats.push(item.hit+" hit");
                if (item.crit)
                    stats.push(item.crit+" crit");
                if (item.desc)
                    stats.push(item.desc);

                return stats.join(" / ");
            },

            formatKey(str) {
                return _.startCase(str);
            },

            formatSockets(item) {
                if (!item.sockets)
                    return null;

                return item.sockets.join(" / ");
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
                window.localStorage.setItem("magesim_tbc_gems", JSON.stringify(this.gems));
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

                var str = window.localStorage.getItem("magesim_tbc_gems");
                if (str) {
                    var gems = JSON.parse(str);
                    if (gems)
                        _.merge(this.gems, gems);
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