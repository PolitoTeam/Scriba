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
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make install \
    && rm -rf /home/user/mongo-c-driver-${MONGOC_VERSION}.tar.gz

WORKDIR /home/user
RUN git clone https://github.com/mongodb/mongo-cxx-driver.git \
    --branch releases/stable --depth 1 \
    && cd mongo-cxx-driver/build \
    && cmake ..                         \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX=/usr/local   \
    && make EP_mnmlstc_core \
    && make \
    && make install

# Create build directory
RUN mkdir /home/user/build

COPY docker-entrypoint.sh /
RUN chmod +x /docker-entrypoint.sh
COPY . /home/user/SharedEditor

ENTRYPOINT ["/docker-entrypoint.sh"]