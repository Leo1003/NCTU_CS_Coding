# Check argument count
if [ $# -lt 1 ]; then
    exit 1
fi

# Match network information
ifrawstr="$(ifconfig "$1")"
ipaddr="$(echo "$ifrawstr" | sed -E -n 's/.+inet (([[:digit:]]{1,3}\.){3}[[:digit:]]{1,3}).*/\1/p')"
netmask="$(echo "$ifrawstr" | sed -E -n 's/.+netmask (0x[0-9a-f]{8}).*/\1/p')"
macaddr="$(echo "$ifrawstr" | sed -E -n 's/.+ether (([0-9a-f]{2}\:){5}[0-9a-f]{2}).*/\1/p')"

 msg="Interface Name: "$1"\n"
msg+="IPv4 Address:   "$ipaddr"\n"
msg+="Netmask:        "$netmask"\n"
msg+="MAC Address:    "$macaddr""

sim_dialog --title "$SIM_TITLE -- Network Infomation" --msgbox "$msg" 8 60

