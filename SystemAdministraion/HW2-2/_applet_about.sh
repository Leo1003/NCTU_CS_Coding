# Write information texts
 msg="Author:       Leo Chen (Shao-Fu Chen)\n"
msg+="              NCTU CS Student: 0716073"

sim_dialog --title "$SIM_TITLE -- About" --yesno "$msg" 8 80
dlg_ret=$?

exit $dlg_ret

