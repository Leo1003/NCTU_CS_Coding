cwd="$(pwd)"
dlg_ret=0
while [ $dlg_ret -eq 0 ]; do
    dlg_args=(sim_dialog --title "'$SIM_TITLE -- File Browser'" --menu "Current Directory: $cwd" 27 80 20)

    files=($(ls -a $cwd))

    if [ -z "$files" ]; then
        # Can't get any files
        dlg_args+=('.' 'inode/directory')
        dlg_args+=('..' 'inode/directory')
        dlg_args+=('###' '(Cannot read the directory!)')
    else
        # Add each entry as a menu item
        for i in "${files[@]}"; do
            dlg_args+=("$i" "$(file -b --mime-type "$cwd/$i")")
        done
    fi

    dlg_sel="$("${dlg_args[@]}")"
    dlg_ret=$?

    if [ $dlg_ret -eq 0 ]; then
        selpath="$(rdslash "$cwd/$dlg_sel")"
        if [ -d "$selpath" ]; then
            # Check if we can enter the directory
            ncwd="$(cd "$selpath" && pwd)"
            cwd=${ncwd:-$cwd}
        elif [ -e "$selpath" ]; then
            bash "$(script_dir)/_applet_file.sh" "$selpath"
        fi
    fi
done

