import { unwrap } from "./common"

export const SimpleState = {
    install(Vue, state) {
        Vue.util.defineReactive(Vue.prototype, '$s', state)
    }
}

export const ReactiveStorage = {
    install(Vue, options) {
        const data = {}
        const watch = {}

        for (const key of options.watch) {
            data[key] = options.storage.getItem(key)
            watch[key] = (x) => x !== null ? options.storage.setItem(key, x) : options.storage.removeItem(key)
        }

        Vue.prototype.$local = new Vue({data: data, watch: watch})
    },
}

export const SimpleApi = {
    install(Vue, options) {
        Vue.prototype.$api = (method = 'GET', path = '', data = null) => {
            const url = options.base + path
            const login = options.getLogin()
            const token = options.getToken()

            let args = {
                method: method,
                mode: 'cors',
                credentials: 'same-origin',
                redirect: 'follow',
                referrerPolicy: 'no-referrer',
                headers: {
                    'X-Session-Login': login,
                    'X-Session-Token': token,
                },
            }

            if (method !== 'GET' && data !== null) {
                if (typeof data === 'object' && !(data instanceof FormData)) data = JSON.stringify(data)
                args['body'] = data
            }

            return unwrap(fetch(url, args))
        }
    }
}

