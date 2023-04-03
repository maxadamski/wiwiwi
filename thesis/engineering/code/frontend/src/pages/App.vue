<script>
export default {
    name: 'App',
    data: () => ({
        tabs: [
            {name: 'Home', url: '/'},
            {name: 'Help', url: '/help'},
            {name: 'Profile', url: '/profile'},
        ],
    }),
}
</script>

<template lang="pug">
    #inner-app
        header
            router-link.header-left(to='/')
                img(src='../static/logo.png')
                span.min-tablet AI Colosseum
            nav
                router-link(v-for='tab in tabs' :to='tab.url') {{tab.name}}
        main
            router-view
        footer
            img(src='../static/logo.png')
            small &copy; Copyright 2020
            img(src='../static/logo-put.png')
</template>

<style lang="stylus">
@import "../styles/basic.styl"
@import "../styles/shared.styl"

.et-wrapper
    left 50% !important
    transform translateX(-50%) !important
    top u3 !important
    background #555 !important
    opacity 0.9
</style>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

#inner-app
    display grid
    grid-template auto 1fr auto / 1fr
    height 100vh
    max-width 1024px
    margin 0 auto
    padding 32px 10px

.header-left
    hflex center center
    font-size 1.5em
    font-weight bold
    color text-color

nav
    hgap u5

    a
        color text-color

        &:hover
            text-decoration underline

header
    hflex space-between center

footer
    hflex space-between center
    height 60px
    margin-top 20px
    padding-top 20px
    border-top solid grey
    border-width thin

header img
    height 64px

footer img
    height 64px
</style>
