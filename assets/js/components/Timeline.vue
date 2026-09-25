<template>
    <div class="graph values">
        <Line
            ref="chart"
            :data="chartData"
            :options="chartOptions"
            :plugins="plugins"
        />
    </div>
</template>

<script>
    import { Line } from 'vue-chartjs';
    import 'chart.js/auto';

    export default {
        components: { Line },

        created() {
            this.plugins = [{
                id: "timeline-images",
                afterRender: (chart) => this.drawImages(chart),
            }];
        },

        mounted() {
            this.draw();
        },

        watch: {
            result() {
                this.draw();
            }
        },

        props: ['result'],

        data() {
            return {
                chartData: { datasets: [] },
                chartOptions: {},
                plugins: [],
                buff_padding: 1.5,
                buff_start_pos: 2,
                cds: [
                    { title: "Bloodlust", color: "rgba(255,70,70,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_bloodlust.jpg" },
                    { title: "Berserking", color: "rgba(190,170,170,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/racial_troll_berserk.jpg" },
                    { title: "Icy Veins", color: "rgba(85,170,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_frost_coldhearted.jpg" },
                    { title: "Arcane Power", color: "rgba(221,51,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_lightning.jpg" },
                    { title: "Band of Eternal Sage", color: "rgba(255, 128, 0, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_ring_55.jpg" },
                    { title: "Light's Wrath", color: "rgba(190, 70, 10, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/classic_spell_holy_blessingofprotection.jpg" },
                    { title: "Destruction", color: "rgba(153, 51, 221, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_potion_107.jpg" },
                    { title: "Mana Tide", color: "rgba(200, 200, 200, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_frost_summonwaterelemental.jpg" },
                    { title: "Power Infusion", color: "rgba(255, 255, 0, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_powerinfusion.jpg" },
                    { title: "Drums of Battle", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_02.jpg" },
                    { title: "Drums of War", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_03.jpg" },
                    { title: "Drums of Restoration", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_07.jpg" },
                    { title: "Innervate", color: "rgba(0,0,0,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_lightning.jpg" },
                    { title: "Evocation", color: "rgba(105,221,105,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_purge.jpg" },
                ],
                trinkets: [
                    { title: "Fel Infusion", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_bone_elfskull_01.jpg" },
                    { title: "Burst of Knowledge", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_amulet_07.jpg" },
                    { title: "Silver Crescent", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas06.jpg" },
                    { title: "Essence of the Martyr", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas01.jpg" },
                    { title: "Serpent Coil", img: "https://wow.zamimg.com/images/wow/icons/large/spell_arcane_arcanetorrent.jpg" },
                    { title: "Dark Iron Pipeweed", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_pipe_01.jpg" },
                    { title: "Recurring Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Essence of Sapphiron", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas06.jpg" },
                    { title: "Spell Haste", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_searinglight.jpg" },
                    { title: "Unstable Currents", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Spell Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Focused Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Enlightenment", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_gem_pearl_04.jpg" },
                    { title: "Arcane Energy", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Crimson Serpent", img: "https://wow.zamimg.com/images/wow/icons/large/ability_hunter_pet_windserpent.jpg" },
                    { title: "Mojo Madness", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_head_troll_01.jpg" },
                    { title: "Aura of the Crusade", img: "https://wow.zamimg.com/images/wow/icons/large/spell_shadow_siphonmana.jpg" },
                    { title: "Insight of the Ashtongue", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_elvencoins.jpg" },
                    { title: "Call of the Nexus", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Power Circle", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_talisman_15.jpg" },
                ],
                mana_gains: [
                    { title: "Mana Gem", color: "rgba(85,255,85,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_gem_stone_01.jpg" },
                    { title: "Mana Potion", color: "rgba(255,255,0,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_potion_137.jpg" },
                ]
            }
        },

        methods: {
            draw() {
                var self = this;

                var data = {
                    datasets: [],
                };

                var options = {
                    plugins: {
                        legend: {
                            display: true,
                            labels: {
                                filter: function(item) {
                                    return item.text != "";
                                }
                            }
                        },
                        tooltip: {
                            enabled: false,
                        }
                    },
                    elements: {
                        line: {
                            tension: 0
                        }
                    },
                    scales: {
                        x: {
                            type: "linear",
                            max: this.result.t,
                            title: {
                                display: true,
                                text: "Time (s)",
                            }
                        },
                        y: {
                            type: "linear",
                            title: {
                                display: true,
                                text: "Mana (%)",
                            }
                        },
                        dps: {
                            id: "dps",
                            type: "linear",
                            position: "right",
                            ticks: {
                                beginAtZero: true,
                            },
                            title: {
                                display: true,
                                text: "DPS",
                            }
                        }
                    }
                };

                // Mana
                var d = [];
                var mana_smooth = true;
                if (mana_smooth) {
                    for (var i=0; i<this.result.log.length; i++) {
                        if (this.result.log[i].text.indexOf("Vampiric Touch") != -1)
                            d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                    }
                    if (!d.length) {
                        for (var i=0; i<this.result.log.length; i++) {
                            if (this.result.log[i].text.indexOf("Mana Regen") != -1)
                                d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                        }
                    }
                    d.push({x: this.result.t, y: _.last(this.result.log).mana_percent});
                }
                else {
                    for (var i=0; i<this.result.log.length; i++)
                        d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                }
                data.datasets.push({
                    data: d,
                    borderColor: "#08f",
                    borderWidth: 0.5,
                    pointRadius: 0,
                    hitRadius: 0,
                    label: "Mana",
                });


                // DPS
                d = [];
                d.push({x: 0, y: 0});
                for (var i=0; i<this.result.log.length; i++) {
                    if (this.result.log[i].type == 1 && this.result.log[i].t)
                        d.push({x: this.result.log[i].t, y: this.result.log[i].dmg / this.result.log[i].t});
                }
                d.push({x: this.result.t, y: this.result.dps});
                data.datasets.push({
                    data: d,
                    borderColor: "#f00",
                    borderWidth: 0.5,
                    pointRadius: 0,
                    label: "DPS",
                    fill: false,
                    yAxisID: "dps",
                });

                var delta = 0;
                var start, end;
                var buff_width = 5;

                // CDs
                for (var i=0; i<this.cds.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+this.cds[i].title});
                    end = _.filter(this.result.log, {text: "Lost "+this.cds[i].title});
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: end.length > j ? end[j].t : this.result.t, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: this.cds[i].color,
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            // label: j == 0 ? this.cds[i].title : '',
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta++;
                }

                // Trinkets
                var trinket_colors = ["rgba(255,255,255,0.6)", "rgba(160,60,160,0.6)"];

                var t = 0;
                for (var i=0; i<this.trinkets.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+this.trinkets[i].title});
                    end = _.filter(this.result.log, {text: "Lost "+this.trinkets[i].title});
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: end.length > j ? end[j].t : this.result.t, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: trinket_colors[t],
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            // label: j == 0 ? this.trinkets[i].title : '',
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length) {
                        delta++;
                        t++;
                        if (t > 1)
                            break;
                    }
                }

                // Mana gained
                /*
                for (var i=0; i<this.mana_gains.length; i++) {
                    start = _.filter(this.result.log, function(a) { return a.text.indexOf(" mana from "+self.mana_gains[i].title) > 0; });
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: start[j].t+1, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: this.mana_gains[i].color,
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta++;
                }
                */

                this.chartData = data;
                this.chartOptions = options;
            },

            drawImages(chart) {
                // vue-chartjs renders once with the initial empty options before
                // mounted() supplies the timeline configuration.
                if (!chart.scales.x || !chart.scales.y)
                    return;

                var self = this;

                var x, y, start;
                var delta = 0;
                var px = 12;

                var buffs = _.concat(this.cds, this.trinkets);
                for (var i=0; i<buffs.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+buffs[i].title});
                    if (buffs[i].img) {
                        for (var j=0; j<start.length; j++) {
                            x = chart.scales.x.getPixelForValue(start[j].t) - px;
                            y = chart.scales.y.getPixelForValue(delta*this.buff_padding + this.buff_start_pos) - px/2;
                            var im = new Image;
                            im.onload = (function(xx, yy, img) {
                                setTimeout(function() { chart.ctx.drawImage(img, xx, yy, 12, 12) }, 100);
                            }(x, y, im));
                            im.src = buffs[i].img;
                        }
                    }
                    if (start.length)
                        delta++;
                }

                for (var i=0; i<this.mana_gains.length; i++) {
                    start = _.filter(this.result.log, function(a) { return a.text.indexOf(" mana from "+self.mana_gains[i].title) > 0; });
                    if (this.mana_gains[i].img) {
                        for (var j=0; j<start.length; j++) {
                            x = chart.scales.x.getPixelForValue(start[j].t) - px;
                            y = chart.scales.y.getPixelForValue(delta*this.buff_padding + this.buff_start_pos) - px/2;
                            var im = new Image;
                            im.onload = (function(xx, yy, img) {
                                setTimeout(function() { chart.ctx.drawImage(img, xx, yy, 12, 12) }, 100);
                            }(x, y, im));
                            im.src = this.mana_gains[i].img;
                        }
                    }
                    if (start.length)
                        delta++;
                }
            },
        }
    }
</script>
