<template>
    <span class="sort-link" :class="[active ? 'active' : '']" @click="onClick">
        <span><slot></slot></span>
        <span class="material-icons" :class="[desc ? 'flip-v' : '']">&#xe5c5;</span>
    </span>
</template>

<script>
    export default {
        props: {
            value: {
                default: null,
            },
            name: {
                type: String,
            },
            order: {
                type: String,
                default: "asc",
            }
        },

        computed: {
            active() {
                return this.name == _.get(this.value, "name");
            },

            desc() {
                return this.active && _.get(this.value, "order", this.order) == "desc";
            }
        },

        methods: {
            onClick() {
                var value = {
                    name: this.name,
                    order: this.order
                };

                if (_.get(this.value, "name") == this.name)
                    value.order = this.flipOrder(_.get(this.value, "order", this.order));

                this.$emit("input", value);
            },

            flipOrder(order) {
                return order == "asc" ? "desc" : "asc";
            }
        }
    }
</script>