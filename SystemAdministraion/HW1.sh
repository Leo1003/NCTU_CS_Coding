#!/usr/bin/env bash
sahwtmp=/tmp/sahw1.txt
rm -f $sahwtmp
uname -a >> $sahwtmp
date -Iseconds >> $sahwtmp
id >> $sahwtmp
service sshd status >> $sahwtmp
zfs list >> $sahwtmp
sudo wg >> $sahwtmp

cat $sahwtmp
read -p "Send mail? " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    mail -v -s '[SAHW1] 0716073' sahw1@nasa.cs.nctu.edu.tw < $sahwtmp
fi
rm $sahwtmp

