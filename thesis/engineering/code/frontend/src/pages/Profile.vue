<script>
export default {
    name: 'Profile',
    data: () => ({
        willDeleteAccount: false,
        deleteAccountConfirm: null,
        newGroup: null,
        editGroup: null,
        loginUser: "",
        loginPass: "",
        signupType: null,
        signupUser: null,
        signupPass1: null,
        signupPass2: null,
        willCreateAccount: false,
    }),
    computed: {
        canCreateAccount() {
            return this.signupUser !== null && this.signupPass1 !== null &&
                this.signupType !== null && this.signupPass1 === this.signupPass2
        }
    },
    methods: {
        async addGroup(name) {
            const [groupData, groupStatus] = await this.safeApi('POST', `/groups`, {name: name})
            if (groupStatus === 409) {
                this.$toast('A group with that name already exists')
                return
            }
            await this.fetchGroups()
        },
        async updateGroup(group) {
            for (const i in this.$s.groups) {
                if (this.$s.groups[i].id === group.id) {
                    if (this.$s.groups[i].name !== group.name) {
                        let oldName = this.$s.groups[i].name;
                        this.$s.groups[i].name = group.name
                        const [groupData, groupStatus] = await this.safeApi('PATCH', `/groups/${group.id}`, JSON.stringify({name: group.name}))
                        if (groupStatus === 409) {
                            this.$toast('A group with that name already exists')
                            this.$s.groups[i].name = oldName
                        }
                    }
                    break
                }
            }
            await this.fetchGroups()
        },
        async activateGame(gameId) {
            await this.safeApi('POST', `/games/activate/${gameId}`)
            const [groupsData, groupsStatus] = await this.safeApi('GET', `/groups`)
            this.$s.groups = groupsData
            const [gameData, gameStatus] = await this.safeApi('GET', '/games/active')
            this.$s.game = gameData
            const [gamesData, gamesStatus] = await this.safeApi('GET', `/games`)
            this.$s.games = gamesData
        },
        async changeStudentNickname() {
            const [teamPatch, teamPatchStatus] = await this.safeApi('PATCH', `/students/me`, JSON.stringify({nickname: this.$s.studentNick}))
            if (teamPatchStatus === 422) {
                this.$toast('Nickname cannot be empty or have more than 50 characters')
                const [userData, userStatus] = await this.safeApi('GET', '/students/me')
                this.$s.studentNick = userData.nickname
            }
            if (teamPatchStatus === 409) {
                this.$toast('Nickname already taken')
                const [userData, userStatus] = await this.safeApi('GET', '/students/me')
                this.$s.studentNick = userData.nickname
            }
        },
        async deleteAccount() {
            if (this.deleteAccountConfirm !== 'I want to delete my account') return
            this.willDeleteAccount = false;
            await this.safeApi('DELETE', `/students/me`)
            await this.doLogout()
        },
        async createAccount() {
            const path = this.signupType === 'teacher' ? '/teachers' : '/students'
            let data = {login: this.signupUser, password: this.signupPass1}
            if (this.signupType === 'student') data.nickname = data.login
            const [response, status] = await this.safeApi('POST', path, data)
            this.$toast(status == 200 ? 'Created user' : `Error (status code ${status})`)
            this.endCreateAccount()
        },
        endCreateAccount() {
            this.signupUser = this.signupPass1 = this.signupPass2 = this.signupType = null
            this.willCreateAccount = false
        },
    }

}
</script>

<template lang="pug">
div
    template(v-if='!isAuthorized')

        .login-form
            h2 Sign in
            input(type='text' v-model='loginUser' placeholder='Username')
            input(type='password' v-model='loginPass' placeholder='Password')
            label.input-checkbox
                input(type='checkbox')
                | Remember me
            button(@click='doLogin(loginUser, loginPass); loginUser = loginPass = null') Sign in

    template(v-else)


        h2 My Profile

        template(v-if='$local.userType == "student"')
            h3 Basic Information

            h4 Nickname
            .hcombo
                input(type='text' v-model='$s.studentNick')
                button(@click="changeStudentNickname") Save

            h4 Group
            .hcombo
                select(v-model='$s.studentGroup')
                    option(disabled :value='null') Click to select
                    option(v-for='group in $s.groups' :value='group.id') {{ group.name }}
                button(@click="safeApi('PATCH', `/students/me`, {group_id: $s.studentGroup})") Save

        template(v-if='$local.userType == "teacher"')
            h3 My Games
            table
                tr
                    th Game
                    th Actions

                tr(v-for='(game,index) in $s.games' :key='game.id')
                    td {{ game.name }}
                    td
                        .hcombo
                            button(@click="activateGame(game.id)") {{ game.id === $s.game.id ? 'Reset' : 'Activate' }}
                            router-link(tag='button' :to="`/game-wizard?edit=${game.id}`") Edit
                            button(v-if='game.id !== $s.game.id' @click="safeApi('DELETE', `/games/${game.id}`); $delete($s.games, index)") Delete
                tr
                    td New game
                    td.hcombo
                        router-link(tag='button' to='/game-wizard') + Create

            h3 My Groups
            table
                tr
                    th Name
                    th Actions
                tr(v-for='(group, index) in $s.groups')
                    template(v-if='editGroup !== null && editGroup.id === group.id')
                        td
                            input(type='text' v-model='editGroup.name')
                        td.hcombo
                            button(@click='updateGroup(editGroup); editGroup = null') Save
                            button(@click='editGroup = null') Cancel

                    template(v-else)
                        td {{ group.name }}
                        td.hcombo
                            button Export
                            button(@click="editGroup = {...group}") Edit
                            button(@click="safeApi('DELETE', `/groups/${group.id}`); $delete($s.groups, index)") Delete

                tr(v-if='newGroup !== null')
                    td
                        input(type='text' v-model='newGroup')
                    td.hcombo
                        button(@click='addGroup(newGroup); newGroup = null') Save
                        button(@click='newGroup = null') Cancel

                tr(v-else)
                    td New group
                    td
                        button(@click='newGroup = "New group"') + Create

        h3 Account Management
        template(v-if='$local.userType == "teacher"')
            h4 Create Account
            .register-form(v-if='willCreateAccount')
                input(type='text' v-model='signupUser' placeholder='Username')
                input(type='password' v-model='signupPass1' placeholder='Password')
                input(type='password' v-model='signupPass2' placeholder='Repeat Password')
                .hlist-3
                    label.input-checkbox
                        input(type='radio' value='student' v-model='signupType')
                        | Student
                    label.input-checkbox
                        input(type='radio' value='teacher' v-model='signupType')
                        | Teacher

                button(:disabled='!canCreateAccount' @click='createAccount') Create
                button(@click='endCreateAccount') Cancel

            div(v-else)
                button(@click='willCreateAccount = true') Create Account


        h4 Delete Account

        div(v-if='willDeleteAccount')
            p To delete your account, type "I want to delete my account" below and click "Confirm".

            p <u>This operation will delete all your data and is irreversible!</u>

            .hcombo
                input(type='text' placeholder='Type here...' v-model='deleteAccountConfirm')
                button(@click='deleteAccount' :disabled='deleteAccountConfirm !== "I want to delete my account"') Confirm

            p If you don&#39;t want to delete your account, click "Cancel".

            button(@click='deleteAccountConfirm = null; willDeleteAccount = false') Cancel

        div(v-else)
            button(@click='willDeleteAccount = true') Delete Account


        h4 Session Management
        button(@click='doLogout') Sign out

</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

table > tr > :nth-child(1)
    width 20ch

.login-form
    vflex center stretch
    vgap u2
    max-width 500px
    margin 0 auto
    margin-top u4

.register-form
    vflex center stretch
    vgap u2
    max-width 400px

</style>

