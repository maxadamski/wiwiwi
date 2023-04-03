# AI Colosseum Containers setup

## Install

### Install the lxc package
``` bash
sudo apt-get install lxc
```

### Automatic install
This script should configure everything that's required for lxc-player to work.
``` bash
sudo ./install.sh
```

### Alternatively: manual install
If for some reason `install.sh` fails to configure the environment
properly, here are the necessary steps.

Check if the kernel has support for user namespaces with the following command:
```bash
grep CONFIG_USER_NS= "/boot/config-$(uname -r)"
```
The output should be:
```
CONFIG_USER_NS=y
```

Create `~/.config/lxc/default.conf` with the following contents:
```
lxc.net.0.type = empty
lxc.idmap = u 0 100000 65536
lxc.idmap = g 0 100000 65536
```

Copy the template file `templates/lxc-player` to `/usr/share/lxc/templates`:
```
cp template/lxc-player /usr/share/lxc/templates/lxc-player
```

Allocate a uid and gid range to the user running lxc:
``` bash
sudo echo "your_user_name:100000:65536" >> /etc/subuid
sudo echo "your_user_name:100000:65536" >> /etc/subgid
```

Next make sure both `newuidmap` and `newgidmap` have SUID set
``` bash
ls -l /bin/newuidmap /bin/newgidmap
```

If they don't, set it with:
``` bash
sudo chmod 4755 /bin/newuidmap /bin/newgidmap
```

**Note:** The remaining steps are only needed for some distributions (Debian, Arch Linux). 

Check if user namespaces are enabled for unprivileged users.
``` bash
sudo sysctl kernel.unprivileged_userns_clone
```
The output should be
```
kernel.unprivileged_userns_clone = 1
```

If it is not, set it with:
``` bash
sudo sysctl kernel.unprivileged_userns_clone=1
```
And make the change permanent with:
``` bash
sudo echo "kernel.unprivileged_userns_clone=1" >> /etc/sysctl.conf
```

## Test
After installation, to test if lxc is configured correctly, try the following:
``` bash
lxc-create -n test -t player
lxc-execute -n test -- bash
```
This should run bash in the `test` container, if all went correctly `/bin` should be
mounted read-only and `/fifo_in` and `/fifo_out` should exist.

## Troubleshooting

### `Could not access /home/user` when running `lxc-execute`

This solution requires the `acl-progs` package.

Solution: allow directory traversal for the inaccessible paths.
```
setfacl -m u:100000:x /home/user
setfacl -m u:100000:x /home/user/.local
setfacl -m u:100000:x /home/user/.local/share
```

