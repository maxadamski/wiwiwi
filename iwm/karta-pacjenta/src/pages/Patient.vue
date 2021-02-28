<script>
import {Plotly} from 'vue-plotly'

export default {
    name: 'Patient',
    components: {
        Plotly,
    },
    data: () => ({
        layout: {
            xaxis: {
                type: 'date',
                rangeslider: {
                    autorange: true
                }
            },
        },
        options: {},

        eventTypes: {
            Observation: {name: 'Observation', icon: 'clipboard-outline', show: true},
            MedicationRequest: {name: 'Med. Request', icon: 'pill', show: true},
        },
        showContact: false,
        ready: false,
        events: [],
        observations: {},
        plotVariable: null,
        observationModalActive: false,
        currentObservation: {},
        currentMedication: null,
        commitMessage: null,

        newIntent: null,
        newStatus: null,

        dateRange: [],
        dateUnit: 'date',
        dateUnits: {
            date: {value: null, next: 'month'},
            month: {value: 'month', next: 'date'},
        },
        info: {
            address: null,
            name: null,
            gender: null,
            birthday: null,
            contact: [],
        },
    }),
    methods: {
        showAllTypes() {
            for (const type in this.eventTypes) {
                this.eventTypes[type].show = true
            }
        },
        makeComponent(x, date) {
            const name = x.code.text
            if (x.hasOwnProperty('valueQuantity')) {
                const value = x.valueQuantity.value
                const unit = x.valueQuantity.unit

                if (!this.observations.hasOwnProperty(name))
                    this.observations[name] = {x: [], y: [], points: [], mode: 'lines+markers'}
                this.observations[name].points.push([date, value])

                return {
                    name: name, value: value,
                    unit: unit == '{score}' ? '' : unit,
                }
            } else if (x.hasOwnProperty('valueCodeableConcept')) {
                return {
                    name: name,
                    value: x.valueCodeableConcept.text,
                    unit: '',
                }
            } else {
                console.log('unsupported component')
                console.log(x)
                return {}
            }
        },
        shouldShow(event) {
            if (this.dateRange.length == 2){
                if (event.date < this.dateRange[0] || event.date > this.dateRange[1])
                    return false
            }

            return this.eventTypes[event.type].show 
        },
        inspectMedication(event) {
            this.currentMedication = event
            this.newIntent = event.intent
            this.newStatus = event.status
        },
        commitMedication(event) {
            if (event.intent === this.newIntent && event.status === this.newStatus) {
                this.commitMessage = 'No changes to commit'
                return
            }
            const intents = ['proposal', 'plan', 'order', 'original-order', 'reflex-order', 'filler-order', 'instance-order', 'option']
            const statuses = ['active','on-hold','cancelled','completed','entered-in-error','stopped','draft','unknown']
            if (!intents.includes(this.newIntent)) {
                this.commitMessage = 'Intent must be equal to one of these values: ' + intents.join(',')
                return
            }
            if (!statuses.includes(this.newStatus)) {
                this.commitMessage = 'Status must be equal to one of these values: ' + statuses.join(',')
                return
            }

            let patches = []
            if (event.intent !== this.newIntent) patches.push({op: 'replace', path: '/intent', value: this.newIntent})
            if (event.status !== this.newStatus) patches.push({op: 'replace', path: '/status', value: this.newStatus})
            this.$fhir.patch({type: event.type, id: event.id, data: patches})
                .then((res) => {
                    console.log(res)
                    event.intent = this.newIntent
                    event.status = this.newStatus
                    this.commitMessage = 'Data updated successfully'
                })
                .catch((err) => {
                    console.log(err)
                    this.commitMessage = 'Unexpected error occured'
                })
        },
        makeMedicationRequest(x) {
            let condition = {name: 'n/a', status: 'n/a', verification: ''}
            console.log(x)
            if (x.hasOwnProperty('reasonReference'))
                this.$fhir.search({type: 'Condition', query: {_id: x.reasonReference[0].reference}}).then((res) => {
                    const reason_ = res.data.entry[0].resource
                    console.log(reason_)
                    condition.name = reason_.code.text
                    condition.status = reason_.clinicalStatus.coding[0].code
                    condition.verification = reason_.verificationStatus.coding[0].text
                })
            return {
                id: x.id,
                ver: x.meta.versionId,
                type: 'MedicationRequest',
                date: new Date(x.authoredOn),
                lastUpdate: new Date(x.meta.lastUpdated),
                medication: x.medicationCodeableConcept.text,
                intent: x.intent,
                status: x.status,
                doctor: x.requester.display,
                condition: condition,
            }
        },
        prevVersion(event) {
            console.log('prev')
            console.log(event)
            if (event.hasOwnProperty('history')) {
                if (event.showVer < event.history.length - 1) {
                    event.showVer += 1
                    for (const key in event.history[event.showVer]) {
                        event[key] = event.history[event.showVer][key]
                    }
                }
            }
        },
        nextVersion(event) {
            if (event.hasOwnProperty('history')) {
                if (event.showVer > 0) {
                    event.showVer -= 1
                    for (const key in event.history[event.showVer]) {
                        event[key] = event.history[event.showVer][key]
                    }
                }
            }
        },
        isVersioned(event) {
            return event.hasOwnProperty('history') && event.history.length > 1
        },
    },
    mounted() {
        const id = this.$route.params.id
        this.$fhir.read({type: 'Patient', patient: id}).then((res) => {
            const x = res.data
            const name = x.name[0]
            const addr = x.address[0]
            this.info.name = [...name.given, name.family].join(' ')
            this.info.gender = x.gender
            this.info.birthday = new Date(x.birthDate).toLocaleDateString()
            this.info.address = `${addr.country}, ${addr.state}, ${addr.city}, ${addr.line.join(' ')}`
            for (const u of x.telecom) {
                this.info.contact.push({name: u.system, value: u.value})
            }
            for (const u of x.identifier) {
                if (!u.type) continue
                this.info.contact.push({name: u.type.text, value: u.value})
            }
        })

        this.$fhir.fetchAll({type: 'Observation', query: {subject: {$eq: id}}}).then((res) => {
            var buffer = {}

            for (const x of res) {
                const key = x.effectiveDateTime
                const date = new Date(key)
                if (!buffer.hasOwnProperty(key)) {
                    buffer[key] = {
                        type: 'Observation',
                        date: date,
                        data: [],
                    }
                }

                if (x.hasOwnProperty('component')) {
                    for (const comp of x.component) {
                        buffer[key].data.push(this.makeComponent(comp, date))
                    }
                } else {
                    buffer[key].data.push(this.makeComponent(x, date))
                }
            }

            this.events = [...this.events, ...Object.values(buffer)]
            this.events.sort((x, y) => y.date - x.date)
            for (const key in this.observations) {
                this.observations[key].points.sort((x, y) => x[0] - y[0])
                for (const point of this.observations[key].points) {
                    this.observations[key].x.push(point[0])
                    this.observations[key].y.push(point[1])
                }
            }
            this.ready = true
        })

        this.$fhir.fetchAll({type: 'MedicationRequest', query: {subject: {$eq: id}}}).then((res) => {
            for (const x of res) {
                let event = this.makeMedicationRequest(x)
                this.$fhir.resourceHistory({type: event.type, id: event.id}).then((hist) => {
                    event.history = hist.data.entry.map((u) => this.makeMedicationRequest(u.resource))
                    event.showVer = 0
                })
                this.events.push(event)
            }
            this.events.sort((x, y) => y.date - x.date)
        })

    },
}
</script>

<template lang="pug">
main.mb-5
    section
        h3 Patient Information

        .info.flex.justify-content-stretch.align-content-center
            h1 {{info.name}}
            .flex
                .ml-5
                    h4 Birth Date
                    h3 {{info.birthday}}
                .ml-5
                    h4 Gender
                    h3.capitalized {{info.gender}}

        .flex.justify-content-center
            b-button.mt-1(@click='showContact = !showContact' rounded :icon-right="showContact ? 'chevron-up' : 'chevron-down'")
                | {{showContact ? 'Show Less' : 'Show More'}}
        table.contact(v-if='showContact')
            tr
                td Address
                td {{info.address}}
            tr(v-for='x in info.contact')
                td.capitalized {{x.name}}
                td {{x.value}}

    hr.my-3


    h3 Filter Events
    section.filters.flex
        .field
            label By date
            b-field
                b-datepicker(v-model='dateRange' placeholder='Click to select' icon='calendar' range trap-focus)
                .control(v-if='dateRange.length > 0')
                    b-button(@click='dateRange = []' icon-left='close').is-danger Clear

        .field
            label By type
            b-field
                b-dropdown(aria-role='list')
                    button.button(slot='trigger' slot-scope='{active}')
                        b-icon(icon='clipboard-outline')
                        span Resource types
                        b-icon(:icon="active ? 'menu-up' : 'menu-down'")
                    b-dropdown-item(custom)
                        b-button.is-primary(@click='showAllTypes' expanded) Show All
                    b-dropdown-item(v-for='(type, key) in eventTypes' :key='key' custom)
                        b-checkbox.my-checkbox(v-model='type.show')
                            .flex.align-items-center
                                b-icon(:icon='type.icon')
                                span {{type.name}}

    hr

    h3 Treatment Timeline
    section.pt-3
        b-progress(v-if='!ready')
        .timeline.is-centered(v-if='ready')
            .timeline-header
                .tag.is-medium.is-primary {{new Date().toLocaleDateString()}}

            .timeline-item(v-for='event in events' v-if='shouldShow(event)')
                .timeline-marker.is-icon(:class="{'is-warning': isVersioned(event)}")
                    b-icon(:icon='eventTypes[event.type].icon')
                .timeline-content
                    p.heading.capitalized [{{event.date.toLocaleDateString()}}] {{event.type}}

                    a(v-if="event.type == 'Observation'" @click='observationModalActive = true; currentObservation = event')
                        p(v-for='entry in event.data.slice(0, 1)')
                            | {{entry.name}}: {{entry.value.toLocaleString(undefined, {minimumFractionDigits: 2})}} {{entry.unit}}
                        p(v-if='event.data.length > 3') And more...

                    a(v-if="event.type == 'MedicationRequest'" @click='inspectMedication(event)')
                        p {{event.medication}}

            .timeline-item
                .timeline-marker
                .timeline-content
            .timeline-header
                .tag.is-medium.is-primary Start
    
    b-modal(:active.sync='observationModalActive' has-modal-card trap-focus scroll='keep')
        .box.card
            div.mb-5
                h2 Observation: Full Report
                span(v-if='currentObservation.date')
                    | Date: {{currentObservation.date.toLocaleDateString()}}

            plotly(:data='[observations[plotVariable]]' :layout='layout' :options='options' v-if='plotVariable != null')
            div.scrollable
                table
                    tr(v-for='entry in currentObservation.data')
                        td
                            b-radio(v-model='plotVariable' :native-value='entry.name') {{entry.name}}
                        td {{entry.value.toLocaleString(undefined, {minimumFractionDigits: 2})}} {{entry.unit}}

            b-button.mt-5(@click='observationModalActive = false; plotVariable = null' rounded expanded) Close

    b-modal(:active='currentMedication != null' has-modal-card trap-focus scroll='keep')
        .box.card(v-if='currentMedication')
            div.mb-5
                h2 Medication Request: Full Report
                div Date: {{currentMedication.date.toLocaleDateString()}}
                div Version timestamp: {{currentMedication.lastUpdate.toLocaleString()}}
                div
                    span.mr-2 Version: {{currentMedication.ver}}
                    b-button(@click='prevVersion(currentMedication)' rounded size='is-small') Previous
                    b-button(@click='nextVersion(currentMedication)' rounded size='is-small') Next

            div.scrollable
                table
                    tr
                        td Medication
                        td {{currentMedication.medication}}
                    tr
                        td Issued by
                        td {{currentMedication.doctor}}
                    tr
                        td Condition
                        td {{currentMedication.condition.name}}
                    tr
                        td Condition Status
                        td {{currentMedication.condition.status}} {{currentMedication.condition.verification}}
                    tr
                        td Intent
                        td(v-if='currentMedication.showVer == 0')
                            b-input(v-model='newIntent')
                        td(v-else)
                            | {{currentMedication.intent}}
                    tr
                        td Status
                        td(v-if='currentMedication.showVer == 0')
                            b-input(v-model='newStatus')
                        td(v-else)
                            | {{currentMedication.status}}

            b-button.is-primary.mt-5(@click='commitMedication(currentMedication)' rounded expanded) Commit Changes
            p.has-text-centered {{commitMessage}}
            b-button.mt-2(@click='currentMedication = null; commitMessage = null' rounded expanded) Close
</template>

<style lang="stylus" scoped>
.timeline-container
    max-width 800px
    margin 0 auto

.my-checkbox
    display flex
    align-items center

.filters
    flex-wrap wrap
    > *
        padding-right var(--u3)

.lh-1
    line-height 1

.box
    min-width 800px

.scrollable
    max-height 200px
    overflow-y scroll
</style>
