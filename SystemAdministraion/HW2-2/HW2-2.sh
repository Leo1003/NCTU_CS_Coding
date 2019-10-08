#!/usr/bin/env bash
export SIM_TITLE='System Information Monitor'
script_dir() {
     SOURCE="${BASH_SOURCE[0]}"
     DIR="$(dirname "$(readlink -f "$SOURCE")")"
     echo "$DIR"
}
export -f script_dir

# Load helper functions
source "$(script_dir)/_utils.sh"

dlg_ret=0
while [ $dlg_ret -eq 0 ]; do
    dlg_tag="$(sim_dialog --title "$SIM_TITLE -- Main Menu" --menu 'Select an applet:' 24 60 17 \
        'CPU' 'CPU Info' \
        'MEM' 'Memory Info' \
        'NET' 'Network Info' \
        'FILE' 'File Browser')"
    dlg_ret=$?
    
    if [ $dlg_ret -eq 0 ]; then
        case "$dlg_tag" in
            'CPU')
                bash "$(script_dir)/_applet_cpu.sh"
                ;;
            'MEM')
                bash "$(script_dir)/_applet_mem.sh"
                ;;
            'NET')
                bash "$(script_dir)/_submenu_net.sh"
                ;;
            'FILE')
                bash "$(script_dir)/_submenu_file.sh"
                ;;
        esac
    fi
done

# Reset terminal & Cleanup screen
reset -e '^?'
clear

# Only if terminate with 'Cancel' button would return 0
if [ $dlg_ret -eq 1 ]; then 
    exit 0
else
    exit 1
fi

