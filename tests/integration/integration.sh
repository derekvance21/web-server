#!/bin/bash
DIR=`dirname $0`
PWD=`pwd`
PORT=8080
REGRESSION_FOLDER="regressions"
CONFIG_FILE="config"
OUTPUT=`mktemp`
PROXY_OUTPUT=`mktemp`

set -e

function cleanup() {
  kill $! > /dev/null 2>&1
}

trap cleanup EXIT

function test() {
  req=$1
  regression=$2
  echo "Testing $req ..."
  curl -sSi -m 2 -o $OUTPUT "http://localhost:${PORT}${req}"
  diff -w $OUTPUT "$REGRESSION_FOLDER/$regression"
}

function test_proxy() {
  echo "Testing proxy $req ..."
  req=$1
  proxy_dest=$2
  curl -sS -m 2 -o $PROXY_OUTPUT $proxy_dest
  curl -sS -m 2 -o $OUTPUT "http://localhost:${PORT}${req}"
  diff -w $OUTPUT $PROXY_OUTPUT
}

function test_bad_request() {
  echo "Testing bad request ..."
  echo "Invalid HTTP Request" | nc -q 1 localhost 8080 > badreq.out
  sleep 1
  diff -w badreq.out "$REGRESSION_FOLDER/regression_400.out"
}

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/integration"
  exit 1
fi

echo "Starting webserver ..."
../../build/bin/webserver $CONFIG_FILE & >/dev/null

sleep 0.5 # wait a bit before starting tests to avoid race conditions

test "/echo" regression_echo.out
test "/bad/location" regression_404.out
test "/static/hello.html" regression_static.out
test "/static1/img.jpg" regression_static1.out
test "/testing/folder/example_config/config_locations" regression_static2.out

test_proxy "/vaxx/text.txt" "http://34.105.8.173/static/text.txt"
test_proxy "/uw" "http://www.washington.edu"

test_bad_request

cleanup
echo "All tests passed!"
exit 0