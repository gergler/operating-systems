#!/bin/bash

if [ -z "$*" ];  then
	echo "No options found!"
	exit 1
fi

print_id() {
	echo -n "uid="; id -ru
        echo -n "euid="; id -u
        echo -n "gid="; id -G
        echo -n "geuid="; id -g
}

print_pid() {
	echo "PID="$$
	echo "PPID="$PPID
	echo -n "PGID=";ps -o '%r' -h $$
}

change_ulimit() {
	echo -n "Ulimit:         ";ulimit -Hu
        ulimit -u $OPTARG
        echo -n "Changed ulimit: ";ulimit -Hu
}

change_core() {
        echo -n "Core:         ";ulimit -ar | grep core
        ulimit -c $OPTARG
        echo -n "Changed core: ";ulimit -ar | grep core
}

change_env() {
}

while getopts ":ispuU:cC:dvV:" opt
do
	case $opt in
		i) print_id ;;
		s) group_owner;;
		p) print_pid;;
		u) ulimit -a;;
		U) change_ulimit;;
		c) ulimit -ar | grep core;;
		C) change_core;;
		d) echo -n "Current DIR: ";pwd;;
		v) printenv;;
		V) change_env;;
		*) echo "No reasonable options found!";;
	esac
done
