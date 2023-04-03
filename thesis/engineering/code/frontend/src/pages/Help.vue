<script>
export default {
    name: 'Help',
    data: () => ({
        fizzbuzz: undefined,
        name: undefined,
        checked: [],
        saveCount: 0,
    }),
}
</script>

<template lang="pug">
    div
        h1 Stylesheet Test
        h3 Typography
        div.wrap
            h1 Heading 1
            h2 Heading 2
            h3 Heading 3
            h4 Heading 4
            h5 Heading 5
            p Paragraph

        h3 Article
        div.wrap
            h1 Hello, world

            p Dolor sapiente excepturi voluptatem facilis earum fugiat. A aspernatur fugit molestiae eum quos. Dolor non possimus dicta hic. Numquam vero qui ducimus labore ut.

            p Rerum vitae velit et fugit maxime. Id ab ut similique. Quasi nostrum unde et. Aut sed omnis voluptate laboriosam ut. Distinctio sed officiis quidem vero quia a. Odit sunt vitae molestias id sunt temporibus quis est.

            h2 Lorem Ipsum

            p Dolor sapiente excepturi voluptatem facilis earum fugiat. A aspernatur fugit molestiae eum quos. Dolor non possimus dicta hic. Numquam vero qui ducimus labore ut.

            p Rerum vitae velit et fugit maxime. Id ab ut similique. Quasi nostrum unde et. Aut sed omnis voluptate laboriosam ut. Distinctio sed officiis quidem vero quia a. Odit sunt vitae molestias id sunt temporibus quis est.

            h2 Lorem Ipsum

            p Dolor sapiente excepturi voluptatem facilis earum fugiat. A aspernatur fugit molestiae eum quos. Dolor non possimus dicta hic. Numquam vero qui ducimus labore ut.

            p Rerum vitae velit et fugit maxime. Id ab ut similique. Quasi nostrum unde et. Aut sed omnis voluptate laboriosam ut. Distinctio sed officiis quidem vero quia a. Odit sunt vitae molestias id sunt temporibus quis est.

            pre
                |int main(int argc, char** argv) { hanusth aositdaoih aistois aoisnthois 
                |   printf("hello, world!\n");
                |   return 0;
                |}


        h3 Controls
        div.wrap
            .hlist
                button Accept
                button Cancel

            .hlist
                input(type='text' placeholder='edit me' v-model='name')
                button(@click='saveCount += 1') Save

            label.input-file
                input(type='file')
                span Upload

            .hlist-2
                label.input-radio
                    input(type='radio' value='fizz' v-model='fizzbuzz')
                    | Fizz

                label.input-radio
                    input(type='radio' value='buzz' v-model='fizzbuzz')
                    | Buzz

            .vflex
                label.input-checkbox
                    input(type='checkbox' value='apples' v-model='checked')
                    | Apples
                label.input-checkbox
                    input(type='checkbox' value='oranges' v-model='checked')
                    | Oranges


        h3 Reactive Data
        div.wrap
            div Save Conut: {{saveCount}}
            div Selected: {{fizzbuzz}}
            div Name: {{name}}
            div Checked: {{checked}}

</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.wrap
    border 1px dashed black
</style>

