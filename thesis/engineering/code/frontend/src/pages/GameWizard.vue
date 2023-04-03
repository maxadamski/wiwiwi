<script>
const dateRegex = /^([1-9][0-9]{3,})-(0?[1-9]|1[012])-(0?[1-9]|[12][0-9]|3[01])$/

export default {
    name: 'GameWizard',
    data: () => ({
        editGame: null,
        name: null,
        subtitle: null,
        deadline: null,
        resources: [
            {code: 0, name: 'Overview', format: 'Markdown', file: null},
            {code: 1, name: 'Rules', format: 'Markdown', file: null},
            {code: 2, name: 'Game Widget', format: 'HTML', file: null},
        ],
        players: [
            {name: 'Judge', env: null, file: null, id: -1},
        ],
        newPlayer: null,
        editPlayer: null,
    }),
    computed: {
        deadlineValid() {
            return String(this.deadline).match(dateRegex)
        },
    },
    methods: {
        addPlayer(player) {
            this.players.push(player)
        },
        deletePlayer(player) {
            this.players = this.players.filter(x => x.id != player.id)
        },
        patchPlayer(player) {
            for (const i in this.players) {
                if (this.players[i].id === player.id) {
                    this.players[i] = player
                    break
                }
            }
        },
        async postGame() {
            const gameForm = new FormData()
            gameForm.append('name', this.name)
            gameForm.append('description', this.subtitle)
            gameForm.append('environment_id', this.$s.envs.find(env => env.name === this.players[0].env).id)
            gameForm.append('deadline', this.deadline)

            gameForm.append('overview', this.resources[0].file)
            gameForm.append('rules', this.resources[1].file)
            gameForm.append('widget', this.resources[2].file)
            gameForm.append('executables', this.players[0].file)

            const [gamePost, gamePostStatus] = await this.safeApi('POST', `/games`, gameForm)
            if (gamePostStatus === 415) {
                console.log(`Bad extension! (status conde ${gamePostStatus})`)
            } else if (gamePostStatus === 413) {
                console.log(`Bad size! (status conde ${gamePostStatus})`)
            } else if (gamePostStatus === 409) {
                console.log(`Game with that name already exists (status code ${gamePostStatus})`)
            } else {
                for (const i in this.players) {
                    if (this.players[i].id !== -1) {
                        const refPlayerForm = new FormData()
                        const name = this.players[i].name
                        refPlayerForm.append('name', name)
                        refPlayerForm.append('executables', this.players[i].file)
                        refPlayerForm.append('environment_id', this.$s.envs.find(env => env.name === this.players[i].env).id)
                        const [refPlayer, refPlayerStatus] = await this.safeApi('POST', `/games/${gamePost}/ref_submissions`, refPlayerForm)
                        if (refPlayerStatus === 415) this.$toast(`Bad file extension of player ${name}`)
                        if (refPlayerStatus === 413) this.$toast(`Code archive for player ${name} is too big`)
                    }
                }
                this.$s.games.push({name: this.name, active: false, id: gamePost})
                await this.$router.push({path: '/profile'})
            }
        },
        async patchGame() {
            const gameForm = new FormData()
            gameForm.append('name', this.name)
            gameForm.append('description', this.subtitle)
            gameForm.append('deadline', this.deadline)

            if (this.resources[0].file) gameForm.append('overview', this.resources[0].file)
            if (this.resources[1].file) gameForm.append('rules', this.resources[1].file)
            if (this.resources[2].file) gameForm.append('widget', this.resources[2].file)

            const [gamePatch, gamePatchStatus] = await this.safeApi('PATCH', `/games/${this.editGame}`, gameForm)

            if (gamePatchStatus === 415) this.$toast('Bad file extension')
            if (gamePatchStatus === 413) this.$toast('Bad file size')
            if (gamePatchStatus === 409) this.$toast('Game with that name already exists')
            if (gamePatchStatus === 422) this.$toast('Bad game name or subtitle length')

            if (gamePatchStatus === 200) {
                for (const i in this.$s.games) {
                    if (this.$s.games[i].id.toString() === this.editGame) {
                        this.$s.games[i].name = this.name
                        break
                    }
                }

                if (this.editGame === this.$s.game.id.toString()) {
                    const [gameData, gameStatus] = await this.safeApi('GET', '/games/active')
                    this.$s.game = gameData
                }
                await this.$router.push({path: '/profile'})
            }
        }
    },
    async mounted() {
        this.editGame = this.$route.query.edit
        if (this.editGame !== undefined) {
            const [gameData, gameStatus] = await this.safeApi('GET', `/games/${this.editGame}`)
            this.name = gameData.name
            this.subtitle = gameData.description
            this.deadline = gameData.deadline.split('T')[0]
            this.players[0].env = this.$s.envs.find(env => env.id === gameData.environment_id).name
            const [gameSubmissions, gameSubmissionsStatus] = await this.safeApi('GET', `/games/${this.editGame}/ref_submissions`)
            for (const i in gameSubmissions) {
                this.players.push({
                    name: gameSubmissions[i].name,
                    env: this.$s.envs.find(env => env.id === gameSubmissions[i].environment_id).name
                })
            }
        }
    },
}

</script>

<template lang="pug">
    div
        h2 {{editGame ? 'Edit' : 'Create'}} Game

        h4 Basic Information
        .hflex.flex-wrap.hlist-1
            div
                h5 Name
                input(type='text' v-model='name' placeholder='Game name')
            div
                h5 Subtitle
                input(type='text' v-model='subtitle' placeholder='Game subtitle')
            div
                h5 Deadline
                input(type='text' v-model='deadline' placeholder='2021-01-28' :class='{invalid: deadline && !deadlineValid}')

        h4 Game Resources

        table.resource-table
            tr
                th Name
                th Format
                th Actions

            tr(v-for='res in resources')
                td {{res.name}}
                td {{res.format}}
                td
                    label.input-file
                        input(type='file' :accept='res.ext' @change='res.file = $event.target.files[0]')
                        span &#8613; Upload


        h4 Game Code

        table.player-table
            tr
                th Name
                th Env
                th(v-if="!editGame") Code
                th(v-if="!editGame")

            tr(v-for="player in players" :key="player.id")
                template(v-if='editPlayer !== null && editPlayer.id === player.id')
                    td
                        input(type='text' v-model='editPlayer.name' :disabled='editPlayer.id === -1')

                    td
                        select(v-model='editPlayer.env')
                            option(:value='null' disabled) Click to select
                            option(v-for="env in $s.envs" :value='env.name') {{ env.name }}

                    td
                        label.input-file
                            input(type='file' @change='editPlayer.file = $event.target.files[0]')
                            span Upload

                    td.hcombo
                        button(@click='patchPlayer(editPlayer); editPlayer = null') Save
                        button(@click='editPlayer = null') Cancel

                template(v-else)
                    td {{ player.name }}
                    td {{ player.env ? player.env : 'None' }}
                    td(v-if="!editGame") {{ player.file !== null ? 'Ready' : (player.file === true ? 'Uploaded' : 'None') }}
                    td.hcombo(v-if="!editGame")
                        button(@click='editPlayer = {...player}' :disabled='editPlayer || newPlayer') &#10002; Edit
                        button(v-if='player.id !== -1' @click='deletePlayer(player)' :disabled='editPlayer || newPlayer') Delete

            tr(v-if='newPlayer !== null')
                td
                    input(type='text' v-model='newPlayer.name' placeholder='New Player')

                td
                    select(v-model='newPlayer.env')
                        option(:value='undefined' disabled) Click to select
                        option(v-for="env in $s.envs" :value='env.name') {{ env.name }}

                td
                    label.input-file
                        input(type='file' @change='newPlayer.file = $event.target.files[0]')
                        span &#8613; Upload

                td.hcombo
                    button(@click='addPlayer(newPlayer); newPlayer = null' :disabled='!newPlayer.name || newPlayer.env === null || newPlayer.file === null') Submit
                    button(@click='newPlayer = false; newPlayer = null') &#10539; Cancel

            tr(v-else-if="!editGame")
                td New Player
                td
                td
                td
                    button(@click='newPlayer = {name: "New Player", file: null}' :disabled='editPlayer !== null || newPlayer !== null') + Create

        h4 Save
        button.w-100.mb-2(@click="editGame ? patchGame() : postGame()" :disabled="!editGame && (!name || !subtitle || !deadline || !resources[0].file || !resources[1].file || !resources[2].file || !players[0].file)") Save
        button.w-100(@click='$router.go(-1)') &#10005; Cancel

</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.resource-table > tr
    > :nth-child(1)
        width 25ch

    > :nth-child(2)
        width 20ch


.player-table > tr
    > :nth-child(1)
        width 25ch

    > :nth-child(2)
        width 20ch

    > :nth-child(3)
        width 15ch

</style>

