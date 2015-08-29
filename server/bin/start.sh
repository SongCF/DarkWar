#!/bin/bash

ulimit -c 102400

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/mysql/lib

./hellgate_server ../conf/hellgate_server.conf &

