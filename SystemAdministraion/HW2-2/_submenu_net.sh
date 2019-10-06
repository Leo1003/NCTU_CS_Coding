dlg_ret=0
while [ $dlg_ret -eq 0 ]; do
    interfaces=($(ifconfig -l))

    dlg_args=(sim_dialog --title "'$SIM_TITLE -- Network Menu'" --menu 'Select a network interface:' 24 60 17)
    # Add each interface as a menu item
    for i in "${interfaces[@]}"; do
        dlg_args+=("$i" '*')
    done

    dlg_sel="$("${dlg_args[@]}")"
    dlg_ret=$?

    if [ $dlg_ret -eq 0 ]; then
        bash "$(script_dir)/_applet_net.sh" "$dlg_sel"
    fi
done

