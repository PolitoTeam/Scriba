#!/bin/bash
echo -n "Building... "
cd build
(
    echo "DEFINES += DOCKER" >> /home/user/SharedEditor/Server/Server.pro \
    && /opt/Qt/5.12.0/gcc_64/bin/qmake /home/user/SharedEditor/Server/Server.pro -spec linux-g++  CONFIG+=debug CONFIG+=qml_debug \
    && /usr/bin/make clean -j8 \
    && /usr/bin/make -f Makefile qmake_all \
    && /usr/bin/make -j8
) >/dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "Completed"
else
    echo "Failed"
    exit 1
fi

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
cd /opt/Qt/5.12.0/gcc_64/lib/
/home/user/build/Server

exec "$@"