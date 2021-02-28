import Vue from 'vue'
import VueRouter from 'vue-router'
import Buefy from 'buefy'

import '@mdi/font/css/materialdesignicons.css'
import 'bulma-extensions/bulma-timeline/dist/css/bulma-timeline.min.css'

import App from './pages/App.vue'
import Index from './pages/Index.vue'
import Patient from './pages/Patient.vue'

const fhir = require('fhir.js')

const store = Vue.observable({

})

const routes = new VueRouter({
    mode: 'history',
    base: '/',
    routes: [
        { path: '/', component: Index },
        { path: '/patient/:id', component: Patient },
    ],
    scrollBehavior(to, from, savedPosition) {
        return {x: 0, y: 0}
    },
})

Vue.use(VueRouter)
Vue.use(Buefy)

Vue.prototype.$log = console.log
Vue.prototype.$s = store
Vue.prototype.$fhir = fhir({baseUrl: 'http://localhost:8080/baseR4'})

new Vue({
    el: '#app',
    router: routes,
    store: store,
    render: f => f(App),
    created() {
    },
})
