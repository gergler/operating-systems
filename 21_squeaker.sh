#!/bin/bash

count=0

trap "((++count)); echo -e '\a'" SIGINT
trap 'echo $count; exit' SIGQUIT

while true;
do
 sleep 100
done
