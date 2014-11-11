#!/bin/sh

echo "\nTests 5 000 000 bytes\n"
echo "Run time main"
./main

echo "Run time developer"
cd salsa_sources/
./main

exit 0
