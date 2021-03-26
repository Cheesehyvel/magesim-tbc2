<template>
    <div class="histogram">
        <canvas ref="canvas"></canvas>
    </div>
</template>

<script>
    import { Bar } from 'vue-chartjs';

    export default {
        mixins: [Bar],

        mounted() {
            this.draw();
        },

        watch: {
            data() {
                this.draw();
            }
        },

        props: ['data'],

        methods: {
            draw() {
                var bin_size = 25;

                var data = {
                    labels: [],
                    datasets: [{
                        data: [],
                        backgroundColor: "#08f",
                        borderColor: "#05c",
                        borderWidth: 1,
                        barPercentage: 1.25,
                    }],
                };

                var keys = _.keys(this.data);
                var first = parseInt(keys[0]);
                var last = parseInt(_.last(keys));
                for (var i=first; i<=last; i+= bin_size) {
                    data.labels.push(i);
                    data.datasets[0].data.push(_.get(this.data, i.toString(), "0"));
                }
                data.labels.push(i);

                var options = {
                    legend: {
                        display: false,
                    },
                    tooltips: {
                        enabled: false,
                    },
                    scales: {
                        xAxes: [
                            {
                                display: false,
                                ticks: {
                                    max: data.labels[data.labels.length-2],
                                }
                            },
                            {
                                display: true,
                                scaleLabel: {
                                    display: true,
                                    labelString: "DPS",
                                },
                                ticks: {
                                    autoSkip: false,
                                    max: data.labels[data.labels.length-1],
                                }
                            }
                        ],
                        yAxes: [{
                            scaleLabel: {
                                display: true,
                                labelString: "Iterations",
                            },
                            ticks: {
                                beginAtZero: true,
                            }
                        }]
                    }
                };

                this.renderChart(data, options);
            }
        }
    }
</script>