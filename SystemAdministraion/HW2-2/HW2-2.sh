#!/usr/bin/env bash
script_dir() {
     SOURCE="${BASH_SOURCE[0]}"
     DIR="$(dirname "$(readlink -f "$SOURCE")")"
     echo "$DIR"
}
export -f script_dir

sim_dialog() {
    dialog --title 'System Information Monitor' --stdout "$@"
    return $?
}
export -f sim_dialog

dlg_ret=0

while [ $dlg_ret -eq 0 ]; do
    dlg_tag="$(sim_dialog --menu 'Main Menu' 0 0 0 \
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

