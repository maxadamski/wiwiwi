## Lab 2

### Klasyczne nazwy interfejsów

- loopback: `lo`
- ethernet: `eth`
- wireless: `wlan, ath, wifi, radio`
- specjalne: `firewire, ib`
- wirtualne: `dummy, br, tun, ppp, vpn`

### Polecenie `ip`

- `ip link [show [up]]`
- `ip link set dev eth0 (up|down)`
- `ip addr [show [up]]`
- `ip addr (add|del) X.X.X.X/X dev _`
- `ip addr flush dev _`

### Testowanie połączenia

- `ping _`
- `traceroute -T _`
