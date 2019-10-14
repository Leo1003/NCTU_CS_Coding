(
    pagesz=$(sysval 'hw.pagesize')
    while true; do
        mem_total="$(sysval 'hw.physmem')"
        mem_free="$(($(sysval 'vm.stats.vm.v_free_count') * $pagesz))"
        mem_used="$(($mem_total - $mem_free))"

        echo 'XXX'
        echo "$mem_used $mem_total" | awk '{print int($1 / $2 * 100 + 0.5)}'
        echo "Total: $(bytes_fmt "$mem_total")"
        echo "Used:  $(bytes_fmt "$mem_used")"
        echo "Free:  $(bytes_fmt "$mem_free")"

        read -e -t 1 -s
        if [ $? -eq 0 ]; then
            break
        fi
    done
) | sim_dialog --title "$SIM_TITLE -- Memory Usage" --gauge "Collecting memory usage..." 12 60 0

