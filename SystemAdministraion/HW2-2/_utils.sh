sim_dialog() {
    dialog --backtitle "$SIM_TITLE (By Leo Chen)" --stdout "$@"
    return $?
}
export -f sim_dialog

sysval() {
    if [ $# -ge 1 ]; then
        echo "$(sysctl -n "$1")"
    fi
}
export -f sysval

bytes_fmt() {
    echo "$(echo "$1" | awk '
        BEGIN {
            unit_str = " B|KB|MB|GB|TB|PB|EB|ZB|YB"
            split(unit_str, unit, "|")
        }
        function bytes_fmt(x) {
            lv = 1;
            while (x >= 1024 && lv <= 9) {
                x /= 1024;
                lv += 1;
            }
            return sprintf("%.02f %s", x, unit[lv]);
        }
        /^[[:digit:]]+$/ {print bytes_fmt($0)}
    ')"
}
export -f bytes_fmt

# Remove duplicated slash
rdslash() {
    echo "$(echo "$1" | tr -s '/')"
}
export -f rdslash

# Percent division
perdiv() {
    printf '% 2.02f' $(bc <<< "scale=2; $1 * 100 / $2")
}
export -f perdiv

