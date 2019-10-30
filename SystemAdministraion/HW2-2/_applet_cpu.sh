# Write information texts
 msg="CPU Model:    $(sysval 'hw.model')\n"
msg+="CPU Machine:  $(sysval 'hw.machine')\n"
msg+="CPU Cores:    $(sysval 'hw.ncpu')"

sim_dialog --title "$SIM_TITLE -- CPU Infomation" --msgbox "$msg" 8 60
dlg_ret=$?

# Only if terminate with 'Cancel' button would return 0
if [ $dlg_ret -eq 1 ]; then
    exit 0
else
    exit 1
fi

