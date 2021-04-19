#!/bin/bash

DIR=`dirname $0`
PWD=`pwd`
PORT=8080

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/"
  exit 1
fi

echo "port ${PORT};" > tmp.config

../build/bin/webserver tmp.config &
sleep 0.2
echo "Hello, world!" | nc localhost 8080 -w 1 > tmp.out
diff -Z regression.out tmp.out
EXIT=$?

rm -f tmp.config tmp.out
kill $! > /dev/null 2>&1
exit $EXIT
