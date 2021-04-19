#!/bin/bash

PORT=8080

echo "port ${PORT};" > tmp.config

../build/bin/webserver tmp.config &
sleep 1
echo "Hello, world!" | nc localhost 8080 -w 1 > tmp.out
diff -w regression.out tmp.out
EXIT=$?

rm -f tmp.config tmp.out
kill $! > /dev/null 2>&1
exit $EXIT
