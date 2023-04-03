export const today = () => {
    const today = new Date()
    today.setHours(0, 0, 0, 0)
    return today
}

export const now = () => {
    return new Date()
}

export const datediff = (t0, t1) => {
    const dt = Date.parse(t1) - Date.parse(t0)
    const sec = Math.floor((dt / 1000) % 60)
    const min = Math.floor((dt / 1000 / 60) % 60)
    const hh = Math.floor((dt / (1000 * 60 * 60)) % 24)
    const dd = Math.floor(dt / (1000 * 60 * 60 * 24))
    const mm = (t1.getFullYear() - t0.getFullYear()) * 12 - t0.getMonth() + t1.getMonth() - 1
    return {months: mm, days: dd, hours: hh, minutes: min, seconds: sec}
}

export const unwrap = (promise) => {
    return promise.then(result => [result, null]).catch(error => [null, error])
}
