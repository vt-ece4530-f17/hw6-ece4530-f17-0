# Using raspberry pi3 as a router

## Port Forwarding on raspberry pi3

## Automatic network connections for raspberry pi3

I am using a pi that boots up, connects to a wireless network, and emails its IP address to me. This is quite convenient if you move a setup frequently between networks, or if you don't want to deal with complex pi-network integration. The steps to follow

-1. Initialize the raspberry pi. I use Noobs to configure standard Raspbian

    https://www.raspberrypi.org/downloads/noobs/

SSH is turned off by default. Turn it on with 

    sudo raspi-config 

-> select interfacing options
-> enable ssh

0. create new gmail account/password. This gmail account is used as the 'From' account that sends out IP addresses.

1. Configure mailing capabilities on raspberry:

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
       password=hash:83020ec25531006fda8473a4d142da59
       phase2="MSCHAPV2"
     }
```
  Where password is VT network password hash create by
```
     echo -n password_here | iconv -t utf16le | openssl md4
```

  This will enable the raspberry to connect to the nearest WiFi

2. Configure the mailer (/etc/ssmtp/ssmtp.conf) and add the following lines
```
    mailhub=smtp.gmail.com:587
    hostname=raspberrypi
    AuthUser=gmailaccount-goes-here
    AuthPass=gmailpassword-goes-here
    FromLineOverride=YES
    UseSTARTTLS=YES
```
3. Create a script /home/pi/mailip.sh
```
    #!/bin/sh
    /sbin/ifconfig | /usr/bin/mail -s "Raspberry Hello" your_vt_email_goes_here
```
4. In crontab add the mail command
```
    @reboot sleep 60 && /home/pi/mailip.sh
```
