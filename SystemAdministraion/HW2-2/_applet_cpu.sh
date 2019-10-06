# Write information texts
 msg="CPU Model:    $(sysctl_value 'hw.model')\n"
msg+="CPU Machine:  $(sysctl_value 'hw.machine')\n"
msg+="CPU Cores:    $(sysctl_value 'hw.ncpu')"

sim_dialog --title "$SIM_TITLE -- CPU Infomation" --msgbox "$msg" 8 60

