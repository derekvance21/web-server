#!/bin/bash
DIR=`dirname $0`
PWD=`pwd`
PORT=8080

function cleanup() {
  rm -f tmp.out tmp.config
  kill $! > /dev/null 2>&1
}

function test() {
  rm -f tmp.out
  curl -sSi -m 2 -o tmp.out $1
  diff -w tmp.out $2
  if [[ $? -ne 0 ]]; then
    cleanup
    exit 1
  fi
}

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/"
  exit 1
fi

echo "port ${PORT}; location /echo { echo; }" > tmp.config
../build/bin/webserver tmp.config &
EXIT=$?
if [[ $EXIT -ne 0 ]]; then
  echo "Failed to start webserver; exit code: ${$?}"
  exit $EXIT
fi

# testing echo
test "http://localhost:$PORT/echo" regression_echo.out
test "http://localhost:$PORT/bad/location" regression_404.out
cleanup
exit 0