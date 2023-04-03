#!/bin/bash
if ! [ $SUDO_USER ]; then
    echo >&2 "This script must be run with sudo!"
    exit 1
fi
home=$(eval echo "~$SUDO_USER")

user_ns=$(grep CONFIG_USER_NS= "/boot/config-$(uname -r)" | cut -d= -f2)
if ! [[ "$user_ns" == "y" ]]; then
	echo "Your kernel was not compiled with user namespace support"
	exit 1
fi

if ! [ -d "$home/.config/lxc" ]; then
    echo "Making $home/.config/lxc"
    mkdir -p "$home/.config/lxc"
    chown $SUDO_USER:$SUDO_USER "$home/.config/lxc"
fi

echo "Making $home/.config/lxc/default.conf"
cat >"$home/.config/lxc/default.conf" <<EOF
lxc.net.0.type = empty
lxc.idmap = u 0 100000 65536
lxc.idmap = g 0 100000 65536
EOF
chown $SUDO_USER:$SUDO_USER "$home/.config/lxc/default.conf"

echo "Copying templates/lxc-player to /usr/share/lxc/templates/lxc-player"
cp templates/lxc-player /usr/share/lxc/templates/lxc-player
chown $SUDO_USER:$SUDO_USER "/usr/share/lxc/templates/lxc-player"

if ! grep "$SUDO_USER:100000:65536" /etc/subuid >/dev/null; then
    echo "Allocating uid range"
    echo "$SUDO_USER:100000:65536" >> /etc/subuid
fi

if ! grep "$SUDO_USER:100000:65536" /etc/subgid >/dev/null; then
    echo "Allocating gid range"
    echo "$SUDO_USER:100000:65536" >> /etc/subgid
fi

if [ -f /proc/sys/kernel/unprivileged_userns_clone ]; then
    sysctl_out=$(sysctl kernel.unprivileged_userns_clone)
    if [ "$sysctl_out" != "kernel.unprivileged_userns_clone = 1" ]; then
        echo "Setting kernel.unprivileged_userns_clone = 1"
        sysctl kernel.unprivileged_userns_clone=1
        echo "kernel.unprivileged_userns_clone=1" >> /etc/sysctl.conf
    fi
fi

uidmap_out=$(ls -l /bin/newuidmap)
if ! [[ "$uidmap_out" == "-rwsr-xr-x"* ]]; then
    echo "Setting SUID for newuidmap"
    chmod 4755 /bin/newuidmap
fi

gidmap_out=$(ls -l /bin/newgidmap)
if ! [[ "$gidmap_out" == "-rwsr-xr-x"* ]]; then
    echo "Setting SUID for newgidmap"
    chmod 4755 /bin/newgidmap
fi

echo "Done!"
