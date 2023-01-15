#!/bin/bash
#
#  latency-control.sh
#  ---------
#  A utility script for traffic shaping using tc
#
#  Usage
#  -----
#  latency-control.sh start - starts the shaper
#  latency-control.sh stop - stops the shaper
#  latency-control.sh restart - restarts the shaper
#  latency-control.sh show - shows the rules currently being shaped
#
#  tc uses the following units when passed as a parameter.
#    kbps: Kilobytes per second
#    mbps: Megabytes per second
#    kbit: Kilobits per second
#    mbit: Megabits per second
#    bps: Bytes per second
#  Amounts of data can be specified in:
#    kb or k: Kilobytes
#    mb or m: Megabytes
#    mbit: Megabits
#    kbit: Kilobits

set -e
set -x

# Interface to shape
IF=lo
# Average to delay packets by
LATENCY=$2

players=$3
concurrency=$4

limit_port() {
  echo 'limit_port' $1
  # say traffic to $PORT is band 3

  tc filter add dev $IF parent 1:0 \
      protocol ip \
      u32 match ip dport $1 0xffff \
      flowid 1:3

  tc filter add dev $IF parent 1:0 \
      protocol ip \
      u32 match ip sport $1 0xffff \
      flowid 1:3
}

start() {
  # Create a priority-based queue.
  tc qdisc add dev $IF root handle 1: prio
  # Delay everything in band 3
  tc qdisc add dev $IF parent 1:3 handle 30: netem delay $LATENCY

  # http server ports
  for (( port = 4000; port < $(($players + 4000)); port++ )) do
    limit_port $port
  done

  # mpc server ports
  for (( i = -1; i < $concurrency; i++ )) do
    port_base=$((5000 + $(($i * 100))))
      for ((port = $port_base; port < $(($players + $port_base)); port++ )) do
        limit_port $port
      done
  done
}

stop() {
    tc qdisc del dev $IF root
}

restart() {
    stop
    sleep 1
    start
}

show() {
    tc -s qdisc ls dev $IF
    tc -p filter show dev lo
}

case "$1" in

start)

echo -n "Starting bandwidth shaping: "
start
echo "done"
;;

stop)

echo -n "Stopping bandwidth shaping: "
stop
echo "done"
;;

restart)

echo -n "Restarting bandwidth shaping: "
restart
echo "done"
;;

show)

echo "Bandwidth shaping status for $IF:"
show
echo ""
;;

*)

pwd=$(pwd)
echo "Usage: latency-control.sh {start|stop|restart|show}"
;;

esac
exit 0