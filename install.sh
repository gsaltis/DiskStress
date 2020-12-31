#!/bin/sh
sudo cp diskstress.rc /etc/init.d/diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc1.d/S99diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc2.d/S99diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc3.d/S99diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc4.d/S99diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc5.d/K99diskstress
sudo ln -s /etc/init.d/diskstress /etc/rc6.d/K99diskstress
sudo timedatectl set-ntp false
