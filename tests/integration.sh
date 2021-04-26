#!/bin/bash
DIR=`dirname $0`
PWD=`pwd`
PORT=8080

function cleanup() {
  rm -f tmp.out
  kill $! > /dev/null 2>&1
}

function test() {
  req=$1
  regression=$2
  rm -f tmp.out
  curl -sSi -m 2 -o tmp.out "http://localhost:${PORT}${req}"
  diff -w tmp.out $regression
  if [[ $? -ne 0 ]]; then
    cleanup
    exit 1
  fi
}

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/"
  exit 1
fi

../build/bin/webserver example_config/config_locations &
EXIT=$?
if [[ $EXIT -ne 0 ]]; then
  echo "Failed to start webserver; exit code: ${$?}"
  exit $EXIT
fi

test "/echo" regression_echo.out
test "/bad/location" regression_404.out
test "/static/hello.html" regression_static.out
test "/static1/img.jpg" regression_static1.out
test "/testing/folder/example_config/config_locations" regression_static2.out

cleanup
echo "All tests passed!"
exit 0