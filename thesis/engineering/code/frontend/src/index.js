import 'regenerator-runtime/runtime'
import Vue from 'vue'
import VueRouter from 'vue-router'
import App from './pages/App.vue'
import {SimpleState, ReactiveStorage, SimpleApi} from './plugins.js'
import {now, unwrap} from './common.js'
import Toast from 'vue-easy-toast'

const develApiUrl = 'http://localhost:8000'
const finalApiUrl = develApiUrl
const apiUrl = process.env.NODE_ENV === 'development' ? develApiUrl : finalApiUrl

Vue.prototype.$log = console.log

Vue.use(VueRouter)

Vue.use(Toast)

Vue.use(SimpleState, {
    //Public data:
    groups: [],
    envs: [],
    game: {
        id: 1,
        name: 'Placeholder Name',
        description: 'Placeholder Description',
        deadline: now(),
        overview: 'Loading...',
        rules: 'Loading...',
    },
    studentGroup: 1,
    studentId: 1,
    studentNick: 'Nickname',
    studentInvitations: [],
    teamId: 1,
    teamName: 'Team name',
    teamLeaderId: 1,
    teamMembers: [],
    teamInvitations: [],
    teamSubmissions: [],
    games: [],
})

Vue.use(ReactiveStorage, {
    storage: window.localStorage,
    watch: ['sessionLogin', 'sessionKey', 'sessionExp', 'userType'],
})

Vue.use(SimpleApi, {
    base: apiUrl,
    getLogin: () => Vue.prototype.$local.sessionLogin,
    getToken: () => Vue.prototype.$local.sessionKey,
})

Vue.mixin({
    data: () => ({
        apiUrl: apiUrl,
    }),
    computed: {
        isAuthorized() {
            const key = this.$local.sessionKey
            const exp = this.$local.sessionExp
            return key && exp && Number(exp) < Date.now()
        },
    },
    methods: {
        async safeApi(method, path, data) {
            const [resp, err1] = await this.$api(method, path, data)
            if (err1) return [null, -1] // network error (bad url, server down etc.)
            const [json, err2] = await unwrap(resp.json())
            if (err2) return [null, resp.status] // response body is not JSON (plain text, HTML etc.)
            return [json, resp.status]
        },
        async doLogin(user, pass) {
            const [data, status] = await this.safeApi('POST', '/login', {login: user, password: pass})
            if (status == 403 || status == 422) {
                this.$toast('Bad username or password!')
                return
            }
            if (status != 200) {
                this.$toast('Could not sign in. Try again later.')
                return
            }
            this.$local.sessionLogin = user
            this.$local.sessionKey = data.key
            this.$local.sessionExp = data.exp
            this.$local.userType = data.is_teacher ? 'teacher' : 'student'
            await this.fetchProfile()
        },
        async doLogout() {
            this.$local.sessionLogin = null
            this.$local.sessionKey = null
            this.$local.sessionExp = null
            this.$local.userType = null
            this.$toast('Signed out')
        },
        async fetchPublic() {
            await this.fetchGroups()

            const [envs, envsStatus] = await this.safeApi('GET', `/environments`)
            this.$s.envs = envs
            console.log(envs, envsStatus)

            const [gameData, gameStatus] = await this.safeApi('GET', '/games/active')
            this.$s.game = gameData
            this.$s.game.deadline = new Date(gameData.deadline)
        },
        async fetchGroups() {
            const [groups, groupsStatus] = await this.safeApi('GET', `/groups`)
            this.$s.groups = groups
        },
        async fetchStudent() {
            const [userData, userStatus] = await this.safeApi('GET', '/students/me')
            if (userStatus === 403 || userStatus === 500) {
                await this.doLogout()
                return
            }
            this.$s.studentId = userData.id
            this.$s.studentNick = userData.nickname
            this.$s.studentGroup = userData.group_id

            const [studentInvitations, studentInvitationsStatus] = await this.safeApi('GET', '/students/me/invitations')
            this.$s.studentInvitations = studentInvitations

            const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
            this.$s.teamId = studentTeam.id
            this.$s.teamName = studentTeam.name
            this.$s.teamLeaderId = studentTeam.leader_id

            const [studentTeamMembers, teamMembersStatus] = await this.safeApi('GET', `/team/${studentTeam.id}/members`)
            this.$s.teamMembers = studentTeamMembers

            const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${studentTeam.id}/invitations`)
            this.$s.teamInvitations = studentTeamInvitations

            const [teamSubmissions, teamSubmissionsStatus] = await this.safeApi('GET', `/teams/me/submissions`)
            this.$s.teamSubmissions = teamSubmissions
        },
        async fetchTeacher() {
            const [gamesData, gamesStatus] = await this.safeApi('GET', `/games`)
            if (gamesStatus === 403 || gamesStatus === 500) {
                await this.doLogout()
                return
            }
            this.$s.games = gamesData
        },
        async fetchProfile() {
            if (this.$local.userType === 'student') await this.fetchStudent()
            if (this.$local.userType === 'teacher') await this.fetchTeacher()
        }
    },
})

const router = new VueRouter({
    mode: 'history',
    base: '/',
    routes: [
        {path: '/', component: () => import('./pages/Index.vue')},
        {path: '/profile', component: () => import('./pages/Profile.vue')},
        {path: '/game-wizard', component: () => import('./pages/GameWizard.vue'), props: true},
        {path: '/help', component: () => import('./pages/Help.vue')},
        {path: '/404', component: () => import('./pages/NotFound.vue')},
        {path: '*', redirect: '/404'},
    ],
    scrollBehavior(to, from, saved) {
        return {x: 0, y: 0}
    },
})

new Vue({
    el: '#app',
    router: router,
    render: f => f(App),
    async created() {
        await this.fetchPublic()
        if (this.isAuthorized) await this.fetchProfile()
    }
})


