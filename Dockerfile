FROM python:3.8

RUN apt-get update && apt-get install -y --no-install-recommends \
                automake \
                build-essential \
                git \
                libboost-dev \
                libboost-thread-dev \
                libsodium-dev \
                libssl-dev \
                libtool \
                m4 \
                texinfo \
                yasm \
        && rm -rf /var/lib/apt/lists/*

# ENV NTL_VERSION 10.5.0
#
# RUN set -ex \
#         \
#        && wget -O ntl.tar.gz "https://www.shoup.net/ntl/ntl-$NTL_VERSION.tar.gz" \
#        && mkdir -p /usr/src/ntl \
#        && tar -xzC /usr/src/ntl --strip-components=1 -f ntl.tar.gz \
#        && rm ntl.tar.gz \
#	    \
#	    && cd /usr/src/ntl/src \
#        && ./configure CXXFLAGS="-g -O2 -fPIC -march=native -pthread -std=c++11" \
#        && make \
#        && make install \
#        && rm -rf /usr/src/ntl

WORKDIR /usr/src/MP-SPDZ
COPY . .
RUN make -j 2 tldr
RUN make -j 2 shamir
RUN make -j 2 online offline
# RUN make -j 2 overdrive
RUN mkdir Player-Data
