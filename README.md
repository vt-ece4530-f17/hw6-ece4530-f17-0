# Using raspberry pi3 as a router

If you have a raspberry pi3 (RPI) you can use it as a router for the DE1SoC.
1) Connect an Ethernet Cable from RPI to DE1SoC
2) Make sure the RPI has wireless network connectivity (See below - Automatic network connections for raspberry pi3)
3) Enable port forwarding on RPI (see below - Port Forwarding on raspberry pi3)

One you have completed these steps, you can log in to the RPI (as pi) and into the DE1SoC (as root) as follows. rpi_ip_address is the wireless address of the RPI, 50444 is the forwarding port.

```   
     ssh pi@rpi_ip_address
     ssh -P 50444 root@rpi_ip_address
```

## Port Forwarding on raspberry pi3

### 1. Enable router network configuration on RPI

In /etc/network/interfaces, replace the configuration for eth0 with the following. You may need to do this as root.

```
# when used as router port
auto eth0
iface eth0 inet static
   address 192.168.10.1
   netmask 255.255.255.0
   network 192.168.10.0
   broadcast 192.168.10.255
```

This selects a network of the form 192.168.10.x, with 192.168.10.1 reserved for the RPI.
Your DE1SoC should be configured with a similar 192.168.10.x address. For example, in /etc/network/interfaces of DE1SoC you could put

```
# Wired or wireless interfaces
auto eth0
iface eth0 inet static
   address 192.168.10.20
   netmask 255.255.254.0
   gateway 192.168.10.255
   dns-nameservers 198.82.247.98 198.82.247.66
```

Note - when you modify /etc/network/interfaces on the RPI, be careful not to modify configuration of other interfaces. For example, a full /etc/network/interfaces on RPI looks like

```
auto lo
iface lo inet loopback

# when used as router port
auto eth0
iface eth0 inet static
   address 192.168.10.1
   netmask 255.255.255.0
   network 192.168.10.0
   broadcast 192.168.10.255

allow-hotplug wlan0
iface wlan0 inet dhcp
    wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf

allow-hotplug wlan1
iface wlan1 inet manual
    wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
```


### 2. Enable port forwarding on the RPI

As root, run the following set of commands. These are valid for the ip address configuration shown above.

```
iptables --flush
iptables --table nat --flush
iptables --delete-chain
iptables --table nat --delete-chain
iptables --table nat --append POSTROUTING --out-interface wlan0 -j MASQUERADE
iptables --append FORWARD --in-interface wlan0 -j ACCEPT
echo 1 > /proc/sys/net/ipv4/ip_forward

# for DE1SOC (socfpga)
iptables -t nat -A PREROUTING -p tcp -i wlan0 --dport 50444 -j DNAT --to 192.168.10.20:22
iptables -t nat -A POSTROUTING -p tcp --dport 22 -j MASQUERADE
```

Once you run these commands, you can log in to the DE1SoC as shown on top of this page.


## Automatic network connections for raspberry pi3

I am using a pi that boots up, connects to a wireless network, and emails its IP address to me. This is quite convenient if you move a setup frequently between networks, or if you don't want to deal with complex pi-network integration. The steps to follow

### -1. Initialize the raspberry pi. I use Noobs to configure standard Raspbian

    https://www.raspberrypi.org/downloads/noobs/

SSH is turned off by default. Turn it on with 

    sudo raspi-config 

-> select interfacing options
-> enable ssh

### 0. create new gmail account/password. This gmail account is used as the 'From' account that sends out IP addresses.

### 1. Configure mailing capabilities on raspberry:

    sudo apt-get install mailutils ssmtp

Modify /etc/wpa_supplicant/wpa_supplicant.conf

  For a simple WPA network add the following configuration:
```   
     network={
       ssid="networkid"
       psk="networkpasswd"
     }
```
  For VT's wireless network (eduroam) add the following configuration
```
     network={
       ssid="eduroam"
       scan_ssid=1
       key_mgmt=WPA-EAP
       eap=PEAP
       identity="your-email-address@vt.edu"
       password=hash:83020ec25bbb006fda8bbba4d142d333
       phase2="MSCHAPV2"
     }
```
  Where password is VT network password hash create by
```
     echo -n password_here | iconv -t utf16le | openssl md4
```

  This will enable the raspberry to connect to the nearest WiFi

### 2. Configure the mailer (/etc/ssmtp/ssmtp.conf) and add the following lines

```
    mailhub=smtp.gmail.com:587
    hostname=raspberrypi
    AuthUser=gmailaccount-goes-here
    AuthPass=gmailpassword-goes-here
    FromLineOverride=YES
    UseSTARTTLS=YES
```

### 3. Create a script /home/pi/mailip.sh

```
    #!/bin/sh
    /sbin/ifconfig | /usr/bin/mail -s "Raspberry Hello" your_vt_email_goes_here
```

### 4. In crontab add the mail command

```
    @reboot sleep 60 && /home/pi/mailip.sh
```
