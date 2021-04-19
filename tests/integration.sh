#!/bin/bash

PORT=8080

echo "port ${PORT};" > tmp.config

../build/bin/webserver tmp.config &
curl -sSi http://localhost:$PORT/
EXIT=$?

rm -f tmp.config
kill $! > /dev/null 2>&1
exit $EXIT
