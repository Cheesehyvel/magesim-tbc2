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
                    datasets: [{
                        data: [],
                        borderColor: "#08f",
                        pointRadius: 0,
                        label: "Mana",
                    }],
                };

                var last = _.last(this.log);

                // Mana
                for (var i=0; i<this.log.length; i++)
                    data.datasets[0].data.push({x: this.log[i].t, y: this.log[i].mana_percent});

                // CDs
                var cds = [
                    { title: "Arcane Power", color: "#d3f" },
                    { title: "Icy Veins", color: "#6df" },
                    { title: "Bloodlust", color: "#d55" },
                    { title: "Drums of Battle", color: "#993" },
                    { title: "Drums of War", color: "#993" },
                    { title: "Drums of Restoration", color: "#993" },
                    { title: "Evocation", color: "#6d6" },
                ];
                var delta = 0;
                for (var i=0; i<cds.length; i++) {
                    var start = _.find(this.log, {text: "Gained "+cds[i].title});
                    if (start) {
                        var end = _.find(this.log, {text: "Lost "+cds[i].title});
                        data.datasets.push({
                            data: [{x: start.t, y: delta}, {x: end ? end.t : last.t, y: delta++}],
                            borderColor: cds[i].color,
                            pointRadius: 0,
                            label: cds[i].title,
                            fill: false,
                        });
                    }
                }

                var options = {
                    legend: {
                        display: true,
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

                this.renderChart(data, options);
            }
        }
    }
</script>