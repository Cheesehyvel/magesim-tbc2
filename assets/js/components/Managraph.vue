<template>
    <div class="managraph">
        <canvas ref="canvas"></canvas>
    </div>
</template>

<script>
    import { Line } from 'vue-chartjs';

    export default {
        mixins: [Line],

        mounted() {
            this.draw();
        },

        watch: {
            log() {
                this.draw();
            }
        },

        props: ['log'],

        methods: {
            draw() {
                var data = {
                    datasets: [],
                };

                var last = _.last(this.log);

                // CDs
                var cds = [
                    { title: "Arcane Power", color: "#d3f" },
                    { title: "Icy Veins", color: "#5af" },
                    { title: "Bloodlust", color: "#d55" },
                    { title: "Innervate", color: "#00f" },
                    { title: "Mana Tide", color: "#aef" },
                    { title: "Destruction", color: "#93d" },
                    { title: "Burst of Knowledge", color: "#939" },
                    { title: "Power Infusion", color: "#dd0" },
                    { title: "Drums of Battle", color: "#993" },
                    { title: "Drums of War", color: "#993" },
                    { title: "Drums of Restoration", color: "#993" },
                    { title: "Evocation", color: "#6d6" },
                ];
                var delta = 0;
                var start, end;
                for (var i=0; i<cds.length; i++) {
                    start = _.filter(this.log, {text: "Gained "+cds[i].title});
                    end = _.filter(this.log, {text: "Lost "+cds[i].title});
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [{x: start[j].t, y: delta}, {x: end.length > j ? end[j].t : last.t, y: delta}],
                            borderColor: cds[i].color,
                            pointRadius: 0,
                            borderWidth: 5,
                            label: j == 0 ? cds[i].title : '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta+= 1;
                }

                // Mana gained
                var gains = [
                    { title: "Mana Potion", color: "#ff0" },
                    { title: "Mana Gem", color: "#5fa" },
                ];
                for (var i=0; i<gains.length; i++) {
                    start = _.filter(this.log, function(a) { return a.text.indexOf(" mana from "+gains[i].title) > 0; });
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [{x: start[j].t, y: delta}, {x: start[j].t+1, y: delta}],
                            borderColor: gains[i].color,
                            pointRadius: 0,
                            label: j == 0 ? gains[i].title : '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta+= 1;
                }

                var options = {
                    legend: {
                        display: true,
                        labels: {
                            filter: function(item, chart) {
                                return item.text != "";
                            }
                        }
                    },
                    tooltips: {
                        enabled: false,
                    },
                    scales: {
                        xAxes: [{
                            type: "linear",
                            scaleLabel: {
                                enabled: true,
                                labelString: "Time (s)",
                            }
                        }],
                        yAxes: [{
                            type: "linear",
                            scaleLabel: {
                                enabled: true,
                                labelString: "Mana (%)",
                            }
                        }]
                    }
                };

                // Mana
                var d = [];
                for (var i=0; i<this.log.length; i++)
                    d.push({x: this.log[i].t, y: this.log[i].mana_percent});
                data.datasets.push({
                    data: d,
                    borderColor: "#08f",
                    pointRadius: 1,
                    label: "Mana",
                });

                this.renderChart(data, options);
            }
        }
    }
</script>