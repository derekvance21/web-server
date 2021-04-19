#!/bin/bash

DIR=`dirname $0`
PWD=`pwd`
PORT=80

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/"
  exit 1
fi

echo "port ${PORT};" > tmp.config

../build/bin/webserver tmp.config &
if [[ $? -eq 0 ]]; then
  curl -sSi http://localhost:$PORT/ > tmp.out
  diff regression.out tmp.out
  EXIT=$?
else
  echo "Failed to start webserver; exit code: ${$?}"
  EXIT=1
fi

rm -f tmp.config tmp.out
kill $! > /dev/null 2>&1
exit $EXIT
