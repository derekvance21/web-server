#!/bin/bash
DIR=`dirname $0`
PWD=`pwd`
PORT=8080

function kill_server() {
  kill $! > /dev/null 2>&1
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

curl -sSi -m 2 http://localhost:$PORT/echo -o tmp.out
diff -w regression_echo.out tmp.out
EXIT=$?
if [[ $EXIT -ne 0 ]]; then
  echo "http://localhost:${PORT}/ failed with code: ${EXIT}"
  kill_server
  exit $EXIT
fi
rm -f tmp.out

curl -sSi -m 2 http://localhost:$PORT/bad/location -o tmp.out
diff -w regression_404.out tmp.out
EXIT=$?
if [[ $EXIT -ne 0 ]]; then
  echo "http://localhost:${PORT}/ failed with code: ${EXIT}"
  kill_server
  exit $EXIT
fi

rm -f tmp.config tmp.out
kill_server
exit $EXIT