if [ $(sysval 'kern.smp.active') -eq 1 ]; then
    cpu_cnt=$(sysval 'kern.smp.cpus')
    cpu_cnt=${cpu_cnt:-1}
else
    cpu_cnt=1
fi

cpu_time=($(sysval 'kern.cp_time'))
cpu_time_total=$((${cpu_time[0]} + ${cpu_time[1]} + ${cpu_time[2]} + ${cpu_time[3]} + ${cpu_time[4]}))
declare -i cpu_timen_total

if [ $cpu_cnt -gt 1 ]; then
    cpu_times=($(sysval 'kern.cp_times'))
    declare -a cpu_times_total
    declare -a cpu_timesn_total
    for c in $(seq 0 $(($cpu_cnt - 1))); do
        c_u=${cpu_times[$((c * 5 + 0))]}
        c_n=${cpu_times[$((c * 5 + 1))]}
        c_s=${cpu_times[$((c * 5 + 2))]}
        c_i=${cpu_times[$((c * 5 + 3))]}
        c_d=${cpu_times[$((c * 5 + 4))]}
        cpu_times_total[$c]=$(($c_u + $c_n + $c_s + $c_i + $c_d))
    done
fi

print_cpu_usage() {
    if [ $cpu_cnt -gt 1 ]; then
        local o_u=${cpu_times[$(($1 * 5 + 0))]}
        local o_n=${cpu_times[$(($1 * 5 + 1))]}
        local o_s=${cpu_times[$(($1 * 5 + 2))]}
        local o_i=${cpu_times[$(($1 * 5 + 3))]}
        local o_d=${cpu_times[$(($1 * 5 + 4))]}
        local c_u=${cpu_timesn[$(($1 * 5 + 0))]}
        local c_n=${cpu_timesn[$(($1 * 5 + 1))]}
        local c_s=${cpu_timesn[$(($1 * 5 + 2))]}
        local c_i=${cpu_timesn[$(($1 * 5 + 3))]}
        local c_d=${cpu_timesn[$(($1 * 5 + 4))]}

        cpu_timesn_total[$c]=$(($c_u + $c_n + $c_s + $c_i + $c_d))
        local c_diff=$((${cpu_timesn_total[$c]} - ${cpu_times_total[$1]}))
        if [ $c_diff -lt 1 ]; then
            c_diff=1
        fi

        local c_usr=$((($c_u - $o_u) + ($c_n - $o_n)))
        local c_sys=$((($c_s - $o_s) + ($c_i - $o_i)))
        local c_idle=$(($c_d - $o_d))

        echo "CPU${1} [ USER:$(perdiv $c_usr $c_diff)% / SYS:$(perdiv $c_sys $c_diff)% / IDLE:$(perdiv $c_idle $c_diff)% ]"
    else
        local o_u=${cpu_time[0]}
        local o_n=${cpu_time[1]}
        local o_s=${cpu_time[2]}
        local o_i=${cpu_time[3]}
        local o_d=${cpu_time[4]}
        local c_u=${cpu_timen[0]}
        local c_n=${cpu_timen[1]}
        local c_s=${cpu_timen[2]}
        local c_i=${cpu_timen[3]}
        local c_d=${cpu_timen[4]}

        cpu_timen_total=$(($c_u + $c_n + $c_s + $c_i + $c_d))
        local c_diff=$((${cpu_timen_total} - ${cpu_time_total}))
        if [ $c_diff -lt 1 ]; then
            c_diff=1
        fi

        local c_usr=$((($c_u - $o_u) + ($c_n - $o_n)))
        local c_sys=$((($c_s - $o_s) + ($c_i - $o_i)))
        local c_idle=$(($c_d - $o_d))

        echo "CPU0 [ USER:$(perdiv $c_usr $c_diff)% / SYS:$(perdiv $c_sys $c_diff)% / IDLE:$(perdiv $c_idle $c_diff)% ]"
    fi
}

print_all_usage() {
    local o_u=${cpu_time[0]}
    local o_n=${cpu_time[1]}
    local o_s=${cpu_time[2]}
    local o_i=${cpu_time[3]}
    local o_d=${cpu_time[4]}
    local c_u=${cpu_timen[0]}
    local c_n=${cpu_timen[1]}
    local c_s=${cpu_timen[2]}
    local c_i=${cpu_timen[3]}
    local c_d=${cpu_timen[4]}

    cpu_timen_total=$(($c_u + $c_n + $c_s + $c_i + $c_d))
    local c_diff=$((${cpu_timen_total} - ${cpu_time_total}))
    local c_used=$((($c_u - $o_u) + ($c_n - $o_n) + ($c_s - $o_s) + ($c_i - $o_i)))
    if [ $c_diff -lt 1 ]; then
        c_diff=1
    fi

    gaugediv $c_used $c_diff
}

(
    while true; do
        # Acquire the new statistics
        cpu_timen=($(sysval 'kern.cp_time'))
        if [ $cpu_cnt -gt 1 ]; then
            cpu_timesn=($(sysval 'kern.cp_times'))
        fi

        # Update the gauge
        echo 'XXX'
        print_all_usage
        for c in $(seq 0 $(($cpu_cnt - 1))); do
            print_cpu_usage $c
        done
        echo 'XXX'

        # Save the statistics
        cpu_time=(${cpu_timen[@]})
        cpu_time_total=$cpu_timen_total
        if [ $cpu_cnt -gt 1 ]; then
            cpu_times=(${cpu_timesn[@]})
            cpu_times_total=(${cpu_timesn_total[@]})
        fi

        read -e -t 1 -s
        if [ $? -eq 0 ]; then
            break
        fi
    done
) | sim_dialog --title "$SIM_TITLE -- CPU Usage" --gauge "Collecting CPU usage..." 24 60 0

