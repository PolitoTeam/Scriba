#!/bin/bash
set -e

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
cd /opt/Qt/5.12.0/gcc_64/lib/
~/Server

exec "$@"