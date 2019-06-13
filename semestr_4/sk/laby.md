# Sieci Komputerowe

## Konfiguracja Linux

### Interfejsy

- `ip (link|addr) [show [up]]`
- `ip link set dev <dev> (up|down)`
- `ip link set dev <dev> address XX:XX:XX:XX:XX:XX` ustaw MAC
- `ip addr (add|del) X.X.X.X/X dev DEV` ustaw IP
- `ip addr flush dev <dev>`

### Router

- `sysctl net.ipv4.conf.all.forwarding=1`
- `ip route add (<dst>/<mask> | default) via <next> [dev <dev>]`
- `ip route (list|flush|del) ...`

### Switch

- `ip l add link <dev> <new_dev> type vlan id <id>
- `ip l del <dev>`

### Firewall

- Tabele: filter, nat, mangle, raw, security
- Łańcuchy: INPUT, OUTPUT, FORWARD, PREROUTING, POSTROUTING
- Akcje: ACCEPT, DROP, REJECT, LOG, MARK, SET

- `iptables -t <table> -L [-n] [--line-numbers] [-v] [-x] [-Z]`
- `iptables -t <table> -P <chain> <action>
- `iptables (-A|-I <n>) <chain> -s <src> -d <dst> -j <action>
- `iptables (-F|-D <n>) <chain>
- `iptables-save/restore`

- `-p udp/tcp --sport/dport <port>`
- `-m conntrack --ctstate INVALID,NEW,ESTABLISHED,RELATED`
- `-m connlimit --connlimit-above/below <n>`
- `-m comment <text>`


## Konfiguracja Cisco

- `picocom [-b9600] /dev/ttyS0` łączenie
- `ctrl+a ctrl+q` wychodzenie

- `ping <dst>`
- `traceroute <dst> [numeric]`
- `enable` - tryb uprzywilejowany
- `configure terminal` - tryb konfiguracji

### Router

- `show interfaces brief`
- `show ip route`
- `show ip ospf (interface|neighbor|database)`

- `hostname <name>`
- `interface <name> X/X` konfiguracja interfejsu
- `ip route <addr> <mask> <next>` trasowanie
- `router ospf <id=1>`

### Switch

- `show vlan brief`
- `show interfaces trunk`

- `vlan <id>`
- `name <name>` nazwa VLAN
- `interface <name> X/X`
- `interface range <name> X/X-Y, ...`

### Interfejsy

- `ip address <addr> <mask>`
- `clock rate <speed>`
- `no shutdown`

- `switchport access vlan <id>`
- `switchport mode (access|trunk)` nie trunk / trunk
- `switchport trunk encapsulation dot1q` używaj 802.1Q
- `no shutdown`

### OSPF

- `network <addr> <odwrócona maska> area <area=0>`
- `auto-cost reference-bandwidth 1000` maksymalna prędkość do kosztów 
- `no passive-interface Gi 0/0` włącz OSPF na interfejsie


## Debugging

- `ping -s <rozmiar> <dst>`
- `traceroute -T <dst>`
- `tracepath -T <dst>`
- `arping -I <dev> <dst>`
- `netstat -g` - grupy multicastowe
- `netserver` - serwer testu szybkości
- `netperf -H <dst>` - klient testu szybkości
- `nc -l <port>` - serwer no porcie
- `nc <dst> <port>` - klient do portu
- `ethtool [-p|--identify] DEV`
- `ethtool DEV`: `link detected: yes/no`
- `tcpdump`
- `ip (link|addr)`: `UP,LOWER_UP -> ok, NO-CARRIER,UP -> err`
- `lampka zapalona -> ok, nie świeci -> err`
- `wireshark xD`
