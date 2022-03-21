# How to connect to eduroam on Linux without a GUI

These instructions are confirmed to work on Void Linux. They may work on other distros, but file locations and service management commands may be different. Also, I don't care for `networkd`, so figure it out yourself if you use it.

Do the following instructions as root (`sudo su`).

## 1. Getting the certificates

Sign in at `elogin.put.poznan.pl`, and click the `Certyfikaty/Certificates` tab.

Click `Wygeneruj certyfikat - klucz CA/Create a certificate with CA key` in the `Certyfikat ogólnego przeznaczenia/General purpose certificate` section. Then click `Pobierz certyfikat z kluczem prywatnym/Download certificate with private key`, enter your current eKonto password in the `Bieżące hasło do eKonta field/Current eKonto password` field, and remember the password in the `Hasło do pliku z certyfikatem/Password for the certificate file` field. Lastly, scroll down and click `Pobierz certyfikat/Download certificate`, which will download a `.p12` file - rename it to `eduroam.p12`. This is the only file we need, as it contains the public, private and CA certificates, which we will extract with `openssl`.

Extract public certificate, private key and CA certificates from the downloaded `eduroam.p12` file. When asked for import password enter the password from the `Hasło do pliku z certyfikatem` filed. In step 2. I'll assume that you moved the resulting files to `/etc/ssl/cert`.

```
# only output CA certificates (-cacerts), don't output private keys (-nokeys)
openssl pkcs12 -in eduroam.p12 -cacerts -nokeys -out eduroam-ca.crt

# only output client certificates (-clcerts), don't output private keys (-nokeys)
openssl pkcs12 -in eduroam.p12 -clcerts -nokeys -out eduroam-public.crt

# only output encrypted private keys (-nocerts), don't encrypt the result (-nodes)
openssl pkcs12 -in eduroam.p12 -nocerts -nodes -out eduroam-private.key
```

If you didn't encrypt `eduroam-private.key`, make sure it is only readable by `root`. You could also omit the `-nodes` flag and provide an encryption password, when asked. You would then need to provide this password in network config files.

## 2.A. Configuring `wpa_supplicant`

Append the following content to `/etc/wpa_supplicant/wpa_supplicant.conf`

```
network={
	ssid="eduroam"
	scan_ssid=1
	key_mgmt=WPA-EAP
	eap=TLS
	identity=jan.kowalski@student.put.poznan.pl" # your student email
	ca_cert="/etc/ssl/cert/adena-ca.crt"
	client_cert="/etc/ssl/cert/eduroam-public.crt"
	private_key="/etc/ssl/cert/eduroam-private.key"
	private_key_passwd="mysecret" # only needed if you encrypted eduroam-private.key
}
```

Ensure that only root can read the network config file.

```
chmod 600 /etc/wpa_supplicant/wpa_supplicant.conf
```

Restart the `wpa_supplicant` service or reboot.

```
# In Void Linux
sv restart wpa_supplicant
```

When in doubt, refer to the `wpa_supplicant` manpage, for example [here](https://linux.die.net/man/5/wpa_supplicant.conf).

## 2.B. Configuring `iwd`

In my experience, `iwd` is better than `wpa_supplicant`. In particular: it establishes connections faster after wakeup from sleep, it's more energy efficient, and it has a more intuitive CLI.

Create file `/var/lib/iwd/eduroam.8021x` with the following content.

```
[Security]
EAP-Method=TLS
EAP-Identity=jan.kowalski@student.put.poznan.pl # your student email
EAP-TLS-CACert=/etc/ssl/cert/eduroam-ca.crt
EAP-TLS-ClientCert=/etc/ssl/cert/eduroam-public.crt
EAP-TLS-ClientKey=/etc/ssl/cert/eduroam-private.key
EAP-TLS-ClientKeyPassphrase=mysecret # only needed if you encrypted eduroam-private.key
```

Ensure that only root can read the network config file.

```
chmod 600 /var/lib/iwd/eduroam.8021x
```

Restart the `iwd` service or reboot.

```
# In Void Linux
sv restart iwd
```

If `iwd` is not connecting to eduroam and reports `error loading client private key`, then you also need to load the `pkcs8_key_parser` kernel module. Execute the following command to automatically load the module on boot.

```
# In Void Linux
echo pkcs8_key_parser > /etc/modules-load.d/pkcs8_key_parser.conf

# In some other distros
echo pkcs8_key_parser >> /etc/modules
```

When in doubt, refer to the `iwd` configuration docs [here](https://iwd.wiki.kernel.org/networkconfigurationsettings).
