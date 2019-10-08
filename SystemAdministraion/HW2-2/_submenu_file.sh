cwd="$(pwd -P)"
dlg_ret=0
while [ $dlg_ret -eq 0 ]; do
    dlg_args=(sim_dialog --title "'$SIM_TITLE -- File Browser'" --menu "Current Directory: $cwd" 27 80 20)

    files=($(ls -a $cwd))

    # Add each interface as a menu item
    for i in "${files[@]}"; do
        dlg_args+=("$i" "$(file -b --mime-type "$cwd/$i")")
    done

    dlg_sel="$("${dlg_args[@]}")"
    dlg_ret=$?

    if [ $dlg_ret -eq 0 ]; then
        if [ -d "$cwd/$dlg_sel" ]; then
            cwd="$(readlink -f "$cwd/$dlg_sel")"
        else
            bash "$(script_dir)/_applet_file.sh" "$cwd/$dlg_sel"
        fi
    fi
done

