#!/usr/bin/env bash
export SIM_TITLE='System Information Monitor'
script_dir() {
     SOURCE="${BASH_SOURCE[0]}"
     DIR="$(dirname "$(readlink -f "$SOURCE")")"
     echo "$DIR"
}
export -f script_dir

sim_dialog() {
    dialog --backtitle "$SIM_TITLE (By Leo Chen)" --stdout "$@"
    return $?
}
export -f sim_dialog

sysctl_value() {
    if [ $# -ge 1 ]; then
        echo "$(sysctl "$1" | sed 's/[^[:space:]]*: //')"
    fi
}
export -f sysctl_value

dlg_ret=0

while [ $dlg_ret -eq 0 ]; do
    dlg_tag="$(sim_dialog --title "$SIM_TITLE -- Main Menu" --menu 'Select an applet:' 24 60 17 \
        'CPU' 'CPU Info' \
        'MEM' 'Memory Info' \
        'NET' 'Network Info' \
        'FILE' 'File Browser')"
    dlg_ret=$?
    
    reset -e '^?'
    clear

    if [ $dlg_ret -eq 0 ]; then
        case "$dlg_tag" in
            'CPU')
                bash "$(script_dir)/_applet_cpu.sh"
                ;;
            'MEM')
                ;;
            'NET')
                ;;
            'FILE')
                ;;
        esac
    fi
done

if [ $dlg_ret -eq 1 ]; then 
    exit 0
else
    exit 1
fi

