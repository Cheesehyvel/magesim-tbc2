<template>
    <div class="help">
        <div class="material-icons" v-html="iconCode"></div>
        <tooltip :position="position">
            <slot></slot>
        </tooltip>
    </div>
</template>

<script>
    export default {
        props: {
            position: {
                type: String,
                default: 'r'
            },

            icon: {
                type: String,
                default() {
                    return "e88e";
                }
            }
        },

        computed: {
            iconCode() {
                return "&#x"+this.icon+";";
            }
        }
    }
</script>