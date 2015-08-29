#!/bin/bash

ps -ef|grep "hellgate_server ../conf/hellgate_server.conf"|grep -v grep
if [ $? == 1 ]; then
      #server done
      cd /usr/local/hellgate/bin
      ./start.sh
fi