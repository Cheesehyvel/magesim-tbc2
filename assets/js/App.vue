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
                <div class="final-stats" v-if="final_stats">
                    <table class="simple">
                        <tbody>
                            <tr>
                                <td>Intellect</td>
                                <td>{{ final_stats.intellect }}</td>
                            </tr>
                            <tr>
                                <td>Spirit</td>
                                <td>{{ final_stats.spirit }}</td>
                            </tr>
                            <tr>
                                <td>Mp5</td>
                                <td>{{ final_stats.mp5 }}</td>
                            </tr>
                            <tr>
                                <td>Spell power</td>
                                <td>{{ final_stats.spell_power }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_arcane">
                                <td>SP Arcane</td>
                                <td>+{{ final_stats.spell_power_arcane }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_frost">
                                <td>SP Frost</td>
                                <td>+{{ final_stats.spell_power_frost }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_fire">
                                <td>SP Fire</td>
                                <td>+{{ final_stats.spell_power_fire }}</td>
                            </tr>
                            <tr>
                                <td>Crit</td>
                                <td>{{ $round(final_stats.crit, 2) }}%</td>
                            </tr>
                            <tr>
                                <td>Hit</td>
                                <td>{{ $round(final_stats.hit, 2) }}%</td>
                            </tr>
                        </tbody>
                    </table>
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
                            v-for="slot in slots"
                            @click="setActiveSlot(slot);"
                        >{{ formatKey(slot) }}</div>
                    </div>
                    <div class="items">
                        <div class="items-wrapper">
                            <div class="btn" :class="[!hasComparisons || is_running ? 'disabled' : '']" @click="runComparison">
                                Run item comparison
                            </div>

                            <table class="mt-2">
                                <thead>
                                    <tr>
                                        <th class="min"></th>
                                        <th class="title">Name</th>
                                        <th v-if="hasComparisons">DPS</th>
                                        <th>Sockets</th>
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
                                        @click="equipToggle(active_slot, item)"
                                        :key="item.id"
                                    >
                                        <td class="min">
                                            <span class="compare" :class="[isComparing(item) ? 'active' : '']" @click.stop="compareItem(item)">
                                                <help icon="e915">Add to comparison</help>
                                            </span>
                                        </td>
                                        <td class="title">
                                            <a :href="itemUrl(item)" :class="['quality-'+$get(item, 'q', 'epic')]" target="_blank" @click.stop>
                                                {{ item.title }}
                                            </a>
                                        </td>
                                        <th v-if="hasComparisons">
                                            {{ comparisonDps(item) }}
                                        </th>
                                        <td>
                                            <template v-if="item.sockets">
                                                <div class="socket-color" :class="['color-'+socket]" v-for="socket in item.sockets"></div>
                                            </template>
                                            <span class="ml-n" v-if="item.bonus" :class="[hasSocketBonus(active_slot) ? 'socket-bonus' : '']">
                                                +{{ formatStats(item.bonus) }}
                                            </span>
                                        </td>
                                        <td>{{ formatSP(item) }}</td>
                                        <td>{{ $get(item, "crit", "") }}</td>
                                        <td>{{ $get(item, "hit", "") }}</td>
                                        <td>{{ $get(item, "int", "") }}</td>
                                        <td>{{ $get(item, "spi", "") }}</td>
                                        <td>{{ $get(item, "mp5", "") }}</td>
                                    </tr>
                                    <tr
                                        class="item"
                                        @click="quickset(set)"
                                        v-for="(set, key) in items.quicksets"
                                        v-if="active_slot == 'quicksets'"
                                    >
                                        <td>{{ set.title }}</td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
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
                                        <td>
                                            <a :href="spellUrl(item)" :class="['quality-'+$get(item, 'q', 'uncommon')]" target="_blank" @click.stop>
                                                {{ item.title }}
                                            </a>
                                        </td>
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
                                                :class="[isSocketed(active_slot, gem.id, index) ? 'active' : '']"
                                                v-for="gem in activeGems(index)"
                                                @click="setSocket(active_slot, gem, index)"
                                            >
                                                <td>
                                                    <a :href="itemUrl(gem)" class="gem-color" :class="['color-'+gem.color]" target="_blank" @click.stop>
                                                        {{ gem.title }}
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
                    <div class="close" @click="logToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>

                <div class="config" v-if="config_open">
                    <div class="fieldsets">
                        <fieldset>
                            <legend>General</legend>
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
                                    <option :value="specs.SPEC_FIRE">Fire</option>
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
                            <div class="form-item" v-if="config.spec == specs.SPEC_ARCANE">
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
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.tooltips" @input="refreshTooltips(true)"> <span>Use item tooltips (requires reload)</span></label>
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
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.inspiring_presence"> <span>Inspiring Presence (Draenei hit aura)</span></label>
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Consumes</legend>
                            <div class="form-item" v-if="!config.battle_elixir && !config.guardian_elixir">
                                <label>Flask</label>
                                <select v-model="config.flask">
                                    <option :value="flasks.FLASK_NONE">None</option>
                                    <option :value="flasks.FLASK_SUPREME_POWER">Supreme Power (70 sp)</option>
                                    <option :value="flasks.FLASK_BLINDING_LIGHT">Blinding Light (80 arc)</option>
                                    <option :value="flasks.FLASK_PURE_DEATH">Pure Death (80 fire/frost)</option>
                                    <option :value="flasks.FLASK_DISTILLED_WISDOM">Distilled Wisdom (65 int)</option>
                                </select>
                            </div>
                            <div class="form-item" v-if="!config.flask">
                                <label>Battle Elixir</label>
                                <select v-model="config.battle_elixir">
                                    <option :value="elixirs.ELIXIR_NONE">None</option>
                                    <option :value="elixirs.ELIXIR_ADEPTS">Adept's Elixir (24 sp / 24 crit)</option>
                                    <option :value="elixirs.ELIXIR_GREATER_ARCANE">Greater Arcane (35 sp)</option>
                                </select>
                            </div>
                            <div class="form-item" v-if="!config.flask">
                                <label>Guardian Elixir</label>
                                <select v-model="config.guardian_elixir">
                                    <option :value="elixirs.ELIXIR_NONE">None</option>
                                    <option :value="elixirs.ELIXIR_DRAENIC_WISDOM">Draenic Wisdom (30 int / 30 spi)</option>
                                    <option :value="elixirs.ELIXIR_MAJOR_MAGEBLOOD">Major Mageblood (16 mp5)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Weapon oil</label>
                                <select v-model="config.weapon_oil">
                                    <option :value="weapon_oils.OIL_NONE">None</option>
                                    <option :value="weapon_oils.OIL_BRILLIANT_WIZARD">Brilliant Wizard Oil (36 sp / 14 crit)</option>
                                    <option :value="weapon_oils.OIL_SUPERIOR_WIZARD">Superior Wizard Oil (42 sp)</option>
                                    <option :value="weapon_oils.OIL_SUPERIOR_MANA">Superior Mana Oil (14 mp5)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Food</label>
                                <select v-model="config.food">
                                    <option :value="foods.FOOD_NONE">None</option>
                                    <option :value="foods.FOOD_SPELL_POWER">Blackened Basilisk (23 sp / 20 spi)</option>
                                    <option :value="foods.FOOD_SPELL_CRIT">Skullfish Soup (20 crit / 20 spi)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Drums</label>
                                <select v-model="config.drums">
                                    <option :value="drums.DRUMS_NONE">None</option>
                                    <option :value="drums.DRUMS_OF_BATTLE">Drums of Battle (80 haste)</option>
                                    <option :value="drums.DRUMS_OF_WAR">Drums of War (30 sp)</option>
                                    <option :value="drums.DRUMS_OF_RESTORATION">Drums of Restoration (600 mana)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Potion</label>
                                <select v-model="config.potion">
                                    <option :value="potions.POTION_NONE">None</option>
                                    <option :value="potions.POTION_MANA">Mana potion</option>
                                    <option :value="potions.POTION_DESTRUCTION">Destruction potion</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Conjured</label>
                                <select v-model="config.conjured">
                                    <option :value="conjureds.CONJURED_NONE">None</option>
                                    <option :value="conjureds.CONJURED_MANA_GEM">Mana Emerald</option>
                                    <option :value="conjureds.CONJURED_FLAME_CAP">Flame Cap</option>
                                </select>
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
                            <div class="form-item" v-if="hasTalent('combustion')">
                                <label>Combustion at</label>
                                <input type="text" v-model.number="config.combustion_at">
                            </div>
                            <div class="form-item" v-if="config.potion && config.potion != potions.POTION_MANA">
                                <label>Potion at</label>
                                <input type="text" v-model.number="config.potion_at">
                            </div>
                            <div class="form-item" v-if="config.conjured && config.conjured != conjureds.CONJURED_MANA_GEM">
                                <label>Conjured at</label>
                                <input type="text" v-model.number="config.conjured_at">
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
                            <div class="form-item" v-if="config.drums">
                                <label>First drums at</label>
                                <input type="text" v-model.number="config.drums_at">
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
                                <label>Number of innervates</label>
                                <input type="text" v-model.number="config.innervate">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Profiles</legend>
                            <div class="profiles">
                                <div class="profile" v-for="profile in profiles" :key="profile.id">
                                    <div class="name" @click="loadProfile(profile)">{{ profile.name }}</div>
                                    <div class="actions">
                                        <div class="save" @click="saveProfile(profile)">
                                            <span class="material-icons">
                                                &#xe161;
                                            </span>
                                        </div>
                                        <div class="delete" @click="deleteProfile(profile)">
                                            <span class="material-icons">
                                                &#xe872;
                                            </span>
                                        </div>
                                    </div>
                                </div>
                                <div class="btn mt-1" @click="newProfile()">New profile</div>
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
    import constants from "./constants";

    export default {
        mounted() {
            this.loadConfig();
            this.loadGear();
            this.loadProfiles();
            this.finalStats();
            this.loadTooltips();
        },

        data() {
            var data = {
                ...constants,
                items: items,
                equipped: {},
                enchants: {},
                gems: {},
                item_gems: {},
                item_comparison: [],
                profiles: [],
                active_slot: "weapon",
                final_stats: null,
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
                    "6": true,
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
                    inspiring_presence: false,

                    food: 0,
                    flask: 0,
                    battle_elixir: 0,
                    guardian_elixir: 0,
                    weapon_oil: 0,
                    drums: 0,
                    potion: 22832,
                    conjured: 22044,

                    tirisfal_2set: true,
                    tirisfal_4set: true,
                    tempest_2set: false,
                    tempest_4set: false,
                    spellfire_set: false,
                    spellstrike_set: false,
                    eternal_sage: false,
                    meta_gem: 0,
                    trinket1: 0,
                    trinket2: 0,

                    innervate: 0,
                    mana_tide: true,
                    bloodlust: true,

                    regen_mana_at: 20,
                    regen_rotation: "ROTATION_FB",
                    mana_tide_at: 20,
                    bloodlust_at: 20,
                    icy_veins_at: 1,
                    cold_snap_at: 21,
                    combustion_at: 1,
                    trinket1_at: 21,
                    trinket2_at: 1,
                    berserking_at: 1,
                    arcane_power_at: 1,
                    presence_of_mind_at: 0,
                    drums_at: 1,
                    potion_at: 21,
                    conjured_at: 21,

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
                    },

                    tooltips: false,
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
                data.equipped[slot] = null;
                data.enchants[slot] = null;
                data.gems[slot] = [null, null, null];
            }

            data.slots = [...slots, "quicksets"];

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

            hasComparisons() {
                return this.item_comparison.length > 1;
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

            async runComparisonFor(item_id) {
                var self = this;
                this.equip(this.active_slot, item_id, false);

                return new Promise((resolve, reject) => {
                    var sim = new SimulationWorkers(self.config.iterations, (result) => {
                        self.is_running = false;
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                        reject(error);
                    });

                    self.log_open = false;
                    self.prepare();
                    self.is_running = true;
                    sim.start(self.config);
                });
            },

            async runComparison() {
                if (!this.hasComparisons || this.is_running)
                    return;

                for (var i in this.item_comparison)
                    this.item_comparison[i].dps = null;

                var max_dps = 0;
                var best_item_id = 0;
                var result, cmp;
                for (var i in this.item_comparison) {
                    cmp = this.item_comparison[i];
                    result = await this.runComparisonFor(cmp.id);
                    this.item_comparison[i].dps = result.avg_dps;
                    if (result.avg_dps > max_dps) {
                        max_dps = result.avg_dps;
                        best_item_id = cmp.id;
                    }
                }

                this.equip(this.active_slot, best_item_id);
            },

            prepare() {
                this.saveGear();
                this.saveConfig();
                this.itemStats();
                this.itemConfig();
                this.finalStats();
            },

            setActiveSlot(slot) {
                if (this.is_running)
                    return;

                this.active_slot = slot;
                this.item_comparison = [];

                if (window.$WowheadPower && this.config.tooltips) {
                    this.$nextTick(function() {
                        this.refreshTooltips();
                    });
                }
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

            getItem(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.equip[eslot], {id: id}, null);
            },

            equippedItem(slot) {
                var id = this.equipped[slot];
                if (!id)
                    return null;

                return this.getItem(slot, id);
            },

            activeGems(index) {
                if (this.activeSockets.length < index)
                    return [];
                if (this.activeSockets[index] == "m")
                    return this.items.gems.filter(g => g.color == "m");
                return this.items.gems.filter(g => g.color != "m");
            },

            finalStats() {
                var x;
                this.itemStats();
                this.itemConfig();

                var stats = _.cloneDeep(this.config.stats);

                // Attribute additions
                if (this.config.arcane_intellect)
                    stats.intellect+= 40;
                if (this.config.divine_spirit)
                    stats.spirit+= 40;
                if (this.config.guardian_elixir == this.elixirs.ELIXIR_DRAENIC_WISDOM) {
                    stats.intellect+= 30;
                    stats.spirit+= 30;
                }
                if (this.config.mark_of_the_wild) {
                    stats.intellect+= 18;
                    stats.spirit+= 18;
                }
                if (this.config.flask == this.flasks.FLASK_DISTILLED_WISDOM)
                    stats.intellect+= 65;
                if (this.config.food == this.foods.FOOD_SPELL_POWER || this.config.food == this.foods.FOOD_SPELL_CRIT)
                    stats.spirit+= 20;

                // Attribute multipliers
                if (x = this.hasTalent("arcane_mind"))
                    stats.intellect*= 1.0 + x*0.03;
                if (this.config.race == this.races.RACE_GNOME)
                    stats.intellect*= 1.05;
                if (this.config.race == this.races.RACE_HUMAN)
                    stats.spirit*= 1.1;
                if (this.config.blessing_of_kings) {
                    stats.intellect*= 1.1;
                    stats.spirit*= 1.1;
                }
                if (this.metaGem() && this.metaGem().id == this.items.ids.EMBER_SKYFIRE)
                    stats.intellect*= 1.02;
                stats.intellect = Math.round(stats.intellect);
                stats.spirit = Math.round(stats.spirit);

                // Mp5
                if (this.config.guardian_elixir == this.elixirs.ELIXIR_MAJOR_MAGEBLOOD)
                    stats.mp5+= 16;
                if (this.config.weapon_oil == this.weapon_oils.OIL_SUPERIOR_MANA)
                    stats.mp5+= 14;

                // Spell power
                var int_multi = 0;
                if (x = this.hasTalent("mind_mastery"))
                    int_multi+= x*0.05;
                if (this.config.spellfire_set)
                    int_multi+= 0.07;
                if (int_multi > 0)
                    stats.spell_power+= Math.round(stats.intellect * int_multi);

                if (this.config.improved_divine_spirit)
                    stats.spell_power+= stats.spirit*0.1;
                if (this.config.wrath_of_air)
                    stats.spell_power+= 102;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_WIZARD)
                    stats.spell_power+= 36;
                if (this.config.weapon_oil == this.weapon_oils.OIL_SUPERIOR_WIZARD)
                    stats.spell_power+= 42;
                if (this.config.food == this.foods.FOOD_SPELL_POWER)
                    stats.spell_power+= 23;
                if (this.config.flask == this.flasks.FLASK_SUPREME_POWER)
                    stats.spell_power+= 70;
                if (this.config.flask == this.flasks.FLASK_BLINDING_LIGHT)
                    stats.spell_power_arcane+= 80;
                if (this.config.flask == this.flasks.FLASK_PURE_DEATH) {
                    stats.spell_power_fire+= 80;
                    stats.spell_power_frost+= 80;
                }
                if (this.config.battle_elixir == this.elixirs.ELIXIR_ADEPTS)
                    stats.spell_power+= 24;
                if (this.config.battle_elixir == this.elixirs.ELIXIR_GREATER_ARCANE)
                    stats.spell_power+= 35;

                // Spell crit
                var critrating = 0;
                if (this.config.judgement_of_the_crusader)
                    stats.crit+= 3;
                if (this.config.moonkin_aura)
                    stats.crit+= 5;
                if (this.config.totem_of_wrath)
                    stats.crit+= 3;
                if (this.config.molten_armor)
                    stats.crit+= 3;
                if (this.config.battle_elixir == this.elixirs.ELIXIR_ADEPTS)
                    critrating+= 24;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_WIZARD)
                    critrating+= 14;
                if (this.config.food == this.foods.FOOD_SPELL_CRIT)
                    critrating+= 20;
                if (critrating > 0)
                    stats.crit+= this.critRatingToChance(critrating);
                if (x = this.hasTalent("arcane_instability"))
                    stats.crit+= x;

                // Spell hit
                if (this.config.totem_of_wrath)
                    stats.hit+= 3;
                if (this.config.race == this.races.RACE_DRAENEI || this.config.inspiring_presence)
                    stats.hit+= 1;

                this.final_stats = stats;
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
                    spell_power_fire: 0,
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
                stats.spell_power_fire+= item_stats.sp_fire;
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

                num = this.numEquippedSet(this.items.ids.TEMPEST_SET);
                this.config.tempest_2set = num > 1;
                this.config.tempest_4set = num > 3;

                this.config.spellstrike_set = this.numEquippedSet(this.items.ids.SPELLSTRIKE_SET) > 1;
                this.config.spellfire_set = this.numEquippedSet(this.items.ids.SPELLFIRE_SET) > 2;

                this.config.eternal_sage = this.isEquipped("finger", this.items.ids.ETERNAL_SAGE);

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

            spellUrl(spell) {
                return "https://tbcdb.com/?spell="+spell.id;
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

            equipToggle(slot, item) {
                if (this.equipped[slot] == item.id)
                    this.unequip(slot);
                else
                    this.equip(slot, item);
            },

            unequip(slot, save) {
                this.equipped[slot] = null;
                this.gems[slot] = [null, null, null];

                this.finalStats();
                if (typeof(save) == "undefined" || save)
                    this.saveGear();
            },

            equip(slot, item, save) {
                if (this.equipped[slot] == item.id)
                    return;

                if (!_.isObject(item))
                    item = this.getItem(slot, item);

                if (slot == "weapon") {
                    if (item.twohand)
                        this.equipped.off_hand = null;
                }
                if (slot.indexOf("trinket") === 0) {
                    var other = slot == "trinket1" ? "trinket2" : "trinket1";
                    if (this.isEquipped(other, item.id))
                        return;
                }

                this.equipped[slot] = item.id;

                if (this.item_gems.hasOwnProperty(item.id)) {
                    this.gems[slot] = this.item_gems[item.id];
                }
                else {
                    this.gems[slot] = [null, null, null];
                    if (item.sockets) {
                        this.gems[slot] = this.defaultGems(item);
                        this.item_gems[item.id] = this.gems[slot];
                    }
                }

                this.finalStats();
                if (typeof(save) == "undefined" || save)
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
                this.finalStats();
            },

            isEnchanted(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEnchanted(slot+"1") || this.isEnchanted(slot+"2");

                return _.get(this.enchants, slot) == id;
            },

            setSocket(slot, gem, index) {
                if (this.isSocketed(slot, gem.id, index)) {
                    this.gems[slot].splice(index, 1, null);
                }
                else {
                    if (gem.unique && this.isSocketedAnywhere(gem.id))
                        return;
                    this.gems[slot].splice(index, 1, gem.id);
                }

                var item_id = this.equipped[slot];
                if (item_id) {
                    if (!this.item_gems.hasOwnProperty(item_id))
                        this.item_gems[item_id] = [null, null, null];
                    this.item_gems[item_id].splice(index, 1, gem.id);
                }

                this.saveGear();
                this.finalStats();
            },

            quickset(set) {
                for (var slot in set.equip)
                    this.equipped[slot] = set.equip[slot];
                for (var slot in set.enchants)
                    this.enchants[slot] = set.enchants[slot];
                for (var slot in set.gems) {
                    this.gems[slot] = set.gems[slot];
                    if (this.equipped[slot])
                        this.item_gems[this.equipped[slot]] = this.gems[slot];
                }

                this.saveGear();
                this.finalStats();
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

            defaultGems(item) {
                var gems = [null, null, null];

                if (item.sockets) {
                    for (var i=0; i<item.sockets.length; i++)
                        gems[i] = this.defaultGem(item.sockets[i]);
                }

                return gems;
            },

            defaultGem(color) {
                if (this.config.spec == this.specs.SPEC_ARCANE)
                    return color == "m" ? this.items.ids.INSIGHTFUL_EARTHSTORM : this.items.ids.RUNED_LIVING_RUBY;
                return color == "m" ? this.items.ids.CHAOTIC_SKYFIRE : this.items.ids.RUNED_LIVING_RUBY;
            },

            hasTalent(talent) {
                var indexes = {
                    presence_of_mind: 13,
                    arcane_mind: 14,
                    arcane_instability: 16,
                    arcane_power: 19,
                    mind_mastery: 21,
                    combustion: 41,
                    icy_veins: 53,
                    cold_snap: 59,
                };

                var index = _.isNumber(talent) ? talent : _.get(indexes, talent);
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
                var item = this.equippedItem(slot);

                if (item && item.use)
                    return true;

                return false;
            },

            isComparing(item) {
                return _.findIndex(this.item_comparison, {id: item.id}) != -1;
            },

            compareItem(item) {
                var index = _.findIndex(this.item_comparison, {id: item.id});
                if (index == -1)
                    this.item_comparison.push({id: item.id, dps: null});
                else
                    this.item_comparison.splice(index, 1);
            },

            comparisonDps(item) {
                var cmp = _.find(this.item_comparison, {id: item.id});
                return cmp && cmp.dps ? _.round(cmp.dps) : null;
            },

            formatStats(item) {
                var stats = [];

                if (item.sp)
                    stats.push(item.sp+" sp");
                if (item.hit)
                    stats.push(item.hit+" hit");
                if (item.crit)
                    stats.push(item.crit+" crit");
                if (item.haste)
                    stats.push(item.haste+" haste");
                if (item.int)
                    stats.push(item.int+" int");
                if (item.spi)
                    stats.push(item.spi+" spi");
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

            formatSP(data) {
                var arr = [];

                if (data.sp)
                    arr.push(data.sp);
                if (data.sp_arcane)
                    arr.push(data.sp_arcane+" arc");
                if (data.sp_frost)
                    arr.push(data.sp_frost+" frost");
                if (data.sp_fire)
                    arr.push(data.sp_fire+" fire");

                if (data.spell_power)
                    arr.push(data.spell_power);
                if (data.spell_power_arcane)
                    arr.push(data.spell_power_arcane+" arc");
                if (data.spell_power_frost)
                    arr.push(data.spell_power_frost+" frost");
                if (data.spell_power_fire)
                    arr.push(data.spell_power_fire+" fire");

                return arr.join(" / ");
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

            saveProfile(profile) {
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.gems = _.cloneDeep(this.gems);
                profile.config = _.cloneDeep(this.config);

                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1)
                    this.profiles.splice(index, 1, profile);
                else
                    this.profiles.push(profile);

                this.saveProfiles();
            },

            loadProfile(profile) {
                if (profile.equipped)
                    _.merge(this.equipped, _.pick(profile.equipped, _.keys(this.equipped)));
                if (profile.enchants)
                    _.merge(this.enchants, _.pick(profile.enchants, _.keys(this.enchants)));
                if (profile.gems)
                    _.merge(this.gems, _.pick(profile.gems, _.keys(this.gems)));
                if (profile.config)
                    _.merge(this.config, _.pick(profile.config, _.keys(this.config)));

                this.finalStats();
            },

            deleteProfile(profile) {
                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1) {
                    this.profiles.splice(index, 1);
                    this.saveProfiles();
                }
            },

            newProfile() {
                var profile = {
                    id: this.uuid(),
                    name: prompt("Profile name"),
                    equipped: {},
                    enchants: {},
                    gems: {},
                    config: {},
                };

                this.saveProfile(profile);
            },

            uuid() {
                return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
                    var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
                    return v.toString(16);
                });
            },

            showLog(log) {
                return this.log_filter[log.type];
            },

            configToggle() {
                this.log_open = false;
                this.config_open = !this.config_open;
                if (!this.config_open) {
                    this.saveConfig();
                    this.finalStats();
                }
            },

            logToggle() {
                this.config_open = false;
                this.log_open = !this.log_open;
            },

            loadTooltips() {
                if (!window.aowow_tooltips && this.config.tooltips) {
                    window.aowow_tooltips = { "colorlinks": true, "iconizelinks": true, "renamelinks": true };
                    var script = document.createElement("script");
                    script.id = "wowheadpower";
                    script.type = "text/javascript";
                    script.src = "http://tbcdb.com/tooltips/power.js?vnew";
                    document.body.appendChild(script);
                }
            },

            refreshTooltips(save) {
                if (window.$WowheadPower)
                    window.$WowheadPower.refreshLinks();

                if (save) {
                    var self = this;
                    setTimeout(function() {
                        self.saveConfig();
                    }, 50);
                }
            },

            saveGear() {
                window.localStorage.setItem("magesim_tbc_equipped", JSON.stringify(this.equipped));
                window.localStorage.setItem("magesim_tbc_enchants", JSON.stringify(this.enchants));
                window.localStorage.setItem("magesim_tbc_gems", JSON.stringify(this.gems));
            },

            loadGear() {
                var equipped, enchants, gems;

                var str = window.localStorage.getItem("magesim_tbc_equipped");
                if (str) {
                    equipped = JSON.parse(str);
                    if (equipped)
                        _.merge(this.equipped, _.pick(equipped, _.keys(this.equipped)));
                }

                var str = window.localStorage.getItem("magesim_tbc_enchants");
                if (str) {
                    enchants = JSON.parse(str);
                    if (enchants)
                        _.merge(this.enchants, _.pick(enchants, _.keys(this.enchants)));
                }

                var str = window.localStorage.getItem("magesim_tbc_gems");
                if (str) {
                    gems = JSON.parse(str);
                    if (gems)
                        _.merge(this.gems, _.pick(gems, _.keys(this.gems)));
                }

                if (!equipped)
                    this.quickset(this.items.quicksets.t5bis);
            },

            saveConfig() {
                window.localStorage.setItem("magesim_tbc_config", JSON.stringify(this.config));
            },

            loadConfig() {
                var str = window.localStorage.getItem("magesim_tbc_config");
                if (str) {
                    var config = JSON.parse(str);
                    if (config)
                        _.merge(this.config, _.pick(config, _.keys(this.config)));
                }
            },

            saveProfiles() {
                window.localStorage.setItem("magesim_tbc_profiles", JSON.stringify(this.profiles));
            },

            loadProfiles() {
                var str = window.localStorage.getItem("magesim_tbc_profiles");
                if (str) {
                    var profiles = JSON.parse(str);
                    if (profiles)
                        this.profiles = profiles;
                }
            },
        }
    }
</script>