<script>
export default {
    name: 'App',
    methods: {
    },
    data: () => ({
    }),
}
</script>

<template lang="pug">
#outer-app
    #inner-app
        header.has-text-centered
            router-link(to='/')
                h1.logo.bold MEDHOSPITALEX
            hr
        router-view
        footer.pb-4.has-text-centered
            hr
            small Copyright &copy; Dariusz Max Adamski
</template>

<style lang="sass">
$phi: 1.618
$family-sans-serif: "Roboto", BlinkMacSystemFont, -apple-system, "Segoe UI", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue", "Helvetica", "Arial", sans-serif
$family-monospace: monospace
$body-line-height: $phi
@import "~bulma"
@import "~buefy/src/scss/buefy"
</style>

<style lang="stylus">
@import "../styles/golden.styl"

#inner-app
    display grid
    grid-template auto 1fr auto / 100%
    height 100vh
    max-width 1800px
    margin 0 auto
    padding 0 10px

hr
    margin var(--u2) 0

main
    margin 0 var(--u3)

td
    padding 0 var(--u2)

.logo
    color salmon
    transition 1s ease all
    &:hover
        transform rotateX(360deg)
</style>
