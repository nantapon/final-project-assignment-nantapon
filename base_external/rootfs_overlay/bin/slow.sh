#!/bin/sh
tc qdisc add dev eth0 root netem latency 100ms
