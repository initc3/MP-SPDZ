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

WORKDIR /usr/src/MP-SPDZ
COPY . .
RUN make -j 2 tldr
RUN make -j 2 shamir
RUN make -j 2 online offline
RUN mkdir Player-Data
