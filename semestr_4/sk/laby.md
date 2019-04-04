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

## Lab 3+4

- nazwy warstwy fizycznej: ` `
- nazwy warstwy łącza danych: ` `

### Sprawdzanie połączenia

- `ethtool [-p|--identify] DEV`
- `ethtool DEV`: `link detected: yes/no`
- `ip (link|addr)`: `UP,LOWER_UP -> ok, NO-CARRIER,UP -> err`
- `lampka zapalona -> ok, nie świeci -> err`

### Adres MAC

- multicast: pierwszy oktet nieparzysty `00:00:01`
- broadcast: `ff:ff:ff:ff:ff:ff`
- pokaż MAC: `ip link`
- ustaw MAC: `ip link set dev DEV address XX:XX:XX:XX:XX:XX`
- pokaż IP: `ip addr`
- ustaw IP: `ip addr add X.X.X.X/X dev DEV`

### Ethernet II

| nazwa        | oktety  |
|--------------|---------|
| preambuła    | 7       |
| separator    | 1       |
| docelowy MAC | 6       |
| źródłowy MAC | 6       |
| 802.1Q tag   | 4       |
| ethertype    | 2       |
| dane         | 46-1500 |
| CRC          | 4       |
| separator    | 12      |

