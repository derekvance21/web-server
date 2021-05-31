#!/bin/bash
DIR=`dirname $0`
PWD=`pwd`
PORT=8080
REGRESSION_FOLDER="regressions"
CONFIG_FILE="config"
OUTPUT=`mktemp`
PROXY_OUTPUT=`mktemp`
COOKIE=`mktemp`

set -e

function cleanup() {
  kill $server > /dev/null 2>&1
  rm -f $OUTPUT $PROXY_OUTPUT $COOKIE
}

trap cleanup EXIT

function test() {
  req=$1
  regression=$2
  curl_args=$3
  echo "Testing $req ..."
  curl -sS -m 2 -b $COOKIE -o $OUTPUT "http://localhost:${PORT}${req}" $curl_args
  diff -w $OUTPUT "$REGRESSION_FOLDER/$regression"
}

function test_proxy() {
  req=$1
  proxy_dest=$2
  echo "Testing proxy $req ..."
  curl -sS -m 2 -b $COOKIE -o $PROXY_OUTPUT $proxy_dest
  curl -sS -m 2 -b $COOKIE -o $OUTPUT "http://localhost:${PORT}${req}"
  diff -w $OUTPUT $PROXY_OUTPUT
}

function test_bad_request() {
  echo "Testing bad request ..."
  echo "Invalid HTTP Request" | nc -q 1 localhost 8080 > $OUTPUT
  sleep 1
  diff -w $OUTPUT "$REGRESSION_FOLDER/regression_400.out"
}

if [[ ! $PWD =~ $DIR$ ]]; then
  echo "wrong directory: must run integration.sh from tests/integration"
  exit 1
fi

echo "Starting webserver ..."
../../build/bin/webserver $CONFIG_FILE > /dev/null 2>&1 &
server=$!

sleep 0.5 # wait a bit before starting tests to avoid race conditions

# Authenticate
curl -sS -m 2 -o $OUTPUT http://localhost:$PORT/login -d password=incorrect-password
diff -w $OUTPUT "$REGRESSION_FOLDER/regression_login_denied.out"

curl -sS -c $COOKIE -m 2 -o $OUTPUT http://localhost:$PORT/login -d password=juzang-password
diff -w $OUTPUT "$REGRESSION_FOLDER/regression_login.out"

test "/bad/location" "regression_404.out" "-i"
test "/static/hello.html" "regression_static.out" "-i"
test "/static1/img.jpg" "regression_static1.out" "-i"
test "/static/sub_folder/dog.txt" "regression_static2.out" "-i"
test_proxy "/vaxx/text.txt" "http://34.105.8.173/static/text.txt"
test_proxy "/uw" "http://www.washington.edu"

test_bad_request

# -- Testing Multithreading
echo "Testing multithreading"
# By calling /sleep handler in the background which sleeps for 15sec
# and calling /echo at the same time and time it
curl -sSi -m 16 -b $COOKIE "http://localhost:${PORT}/sleep" > /dev/null 2>&1 & 
starttime=`date +%s`

test "/static/hello.html" regression_static.out "-i"
endtime=`date +%s`

# bash arithmetic has to be integers - it won't handle decimal places
runtime=$(( endtime - starttime ))

# since conditional is arithmetic, inside double parentheses
if (( runtime >= 15 )); then
    echo "Failed multithreading; time exceeded"
    exit 1
fi

echo "All tests passed!"
exit 0
