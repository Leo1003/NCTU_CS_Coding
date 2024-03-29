# Check argument count
if [ $# -lt 1 ]; then
    exit 1
fi

while true; do
    if [ ! -e "$1" ]; then
        exit 1
    fi

    # Get some Iinformation about the file
    filename="$(basename -- "$1")"
    mimetype="$(file -b --mime-type "$1")"
    eval "$(stat -s "$1")"

     msg="File Name: "$filename"\n"
    msg+="File Info: "$(file -b "$1")"\n"
    msg+="File Size: "$(bytes_fmt "$st_size")""

    if [[ "$mimetype" == text/* ]]; then
        editbtn=('--extra-button' '--extra-label' 'Edit')
    else
        editbtn=()
    fi

    sim_dialog --title "$SIM_TITLE -- File Infomation" ${editbtn[@]} --msgbox "$msg" 24 80
    dlg_ret=$?

    if [ $dlg_ret -eq 3 ]; then
        "${EDITOR:-ee}" "$1"
    else
        break
    fi
done
