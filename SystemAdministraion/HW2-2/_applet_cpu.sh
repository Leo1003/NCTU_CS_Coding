# Write information texts
 msg="CPU Model:    $(sysval 'hw.model')\n"
msg+="CPU Machine:  $(sysval 'hw.machine')\n"
msg+="CPU Cores:    $(sysval 'hw.ncpu')"

sim_dialog --title "$SIM_TITLE -- CPU Infomation" --msgbox "$msg" 8 60

