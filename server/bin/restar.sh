#!/bin/bash

ps -ef|grep "hellgate_server ../conf/hellgate_server.conf"|grep -v grep|awk '{print $2}'|xargs kill 
      
#server done
cd /usr/local/hellgate/bin
./start.sh
