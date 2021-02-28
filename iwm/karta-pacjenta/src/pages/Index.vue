<script>

export default {
    name: 'Index',
    components: {
    },
    data: () => ({
        patients: [ ],
        search: false,
    }),
    methods: {
    },
    computed: {
    },
    mounted() {
        this.$fhir.fetchAll({type: 'Patient'}).then((res) => {
            for (const x of res) {
                this.patients.push({
                    id: x.id,
                    firstName: x.name[0].given[0],
                    lastName: x.name[0].family,
                    date: x.meta.lastUpdated,
                    gender: x.gender,
                })
            }
        })

    },
}
</script>

<template lang="pug">
main
    .flex.justify-content-stretch
        h3 Patient List
        b-button(@click='search = !search' icon-left='filter') Toggle filters
    b-table(
        :data='patients'
        paginated
        pagination-simple
        per-page='10'
        sort-icon='arrow-up'
        sort-icon-size='is-small'
    )
        template(slot-scope='props')
            b-table-column(field='firstName' label='First Name' sortable :searchable='search')
                | {{props.row.firstName}}
            b-table-column(field='lastName' label='Last Name' sortable :searchable='search')
                | {{props.row.lastName}}
            b-table-column(label='Gender')
                b-icon.mr-2(:icon='props.row.gender == "male" ? "gender-male" : "gender-female"')
                span.capitalized {{props.row.gender}}
            b-table-column(label='Action')
                router-link(:to="`/patient/${props.row.id}`") More
</template>
