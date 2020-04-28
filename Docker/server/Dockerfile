FROM rabits/qt:5.12-desktop

LABEL maintainer="enrico.loparco@studenti.polito.it"

USER root

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    clang \
    libmongoc-1.0-0 \
    libbson-1.0 \
    libssl-dev \
    libsasl2-dev \
    wget

ENV MONGOC_VERSION=1.16.2

# Install mongoc driver
WORKDIR /home/user
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.16.2/mongo-c-driver-${MONGOC_VERSION}.tar.gz \
    && tar xzf mongo-c-driver-${MONGOC_VERSION}.tar.gz \
    && cd mongo-c-driver-${MONGOC_VERSION} \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -j8 -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make install

# Install mongocxx driver (most recent stable release)
WORKDIR /home/user
RUN git clone https://github.com/mongodb/mongo-cxx-driver.git \
    --branch releases/stable --depth 1 \
    && cd mongo-cxx-driver/build \
    && cmake ..                         \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX=/usr/local   \
    && make EP_mnmlstc_core \
    && make -j8 \
    && make install

# RUN apt-get install -y gnupg \
#     && wget -qO - https://www.mongodb.org/static/pgp/server-4.2.asc | apt-key add - \
#     && echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/4.2 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-4.2.list \
#     && apt-get update \
#     && apt-get install -y mongodb-org \
#     && mkdir -p /data/db

COPY ./Server /home/user/Server

# EXPOSE 1500
# EXPOSE 27017

# CMD ["/bin/bash"]
# CMD ["sleep", "infinity"]
COPY docker-entrypoint.sh /
RUN chmod +x /docker-entrypoint.sh
ENTRYPOINT ["/docker-entrypoint.sh"]