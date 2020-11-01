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
                vim \
                lsof \
                libmpfr-dev \
                libmpc-dev \
                golang \
        && rm -rf /var/lib/apt/lists/*

#RUN wget https://dl.google.com/go/go1.13.12.linux-amd64.tar.gz
#RUN tar xf go1.13.12.linux-amd64.tar.gz
#RUN mv go /usr/local/go
#RUN echo 'export GOROOT=/usr/local/go' >> ~/.profile
#RUN echo 'export GOPATH=$HOME/gopath' >> ~/.profile
#RUN echo 'export GOBIN=$GOPATH/bin' >> ~/.profile
#RUN echo 'export PATH=$GOPATH:$GOBIN:$GOROOT/bin:$PATH' >> ~/.profile
#RUN rm /bin/sh && ln -s /bin/bash /bin/sh
#RUN /bin/bash -c "source ~/.profile"
#RUN /bin/bash -c "source /usr/local/bin/virtualenvwrapper.sh"

#RUN go get github.com/ethereum/go-ethereum
#RUN cd $GOPATH/src/github.com/ethereum/go-ethereum
ENV GOPATH $HOME/gopath
RUN mkdir -p $GOPATH/src/github.com/ethereum
WORKDIR $GOPATH/src/github.com/ethereum
RUN git clone https://github.com/ethereum/go-ethereum.git
WORKDIR $GOPATH/src/github.com/ethereum/go-ethereum
RUN git checkout cfbb969da
RUN make geth

RUN go get github.com/syndtr/goleveldb/leveldb

ENV MP_SPDZ_HOME /usr/src/MP-SPDZ
WORKDIR $MP_SPDZ_HOME

# mpir
COPY --from=initc3/mpir:55fe6a9 /usr/local/mpir ./local
RUN echo MY_CFLAGS += -I./local/include >> CONFIG.mine
RUN echo MY_LDLIBS += -Wl,-rpath -Wl,./local/lib -L./local/lib >> CONFIG.mine

# pip, ipython
RUN pip install --upgrade pip ipython

# install compiler (console script mpspdz-compile)
COPY Compiler Compiler
RUN pip install --editable Compiler/

COPY . .

RUN make clean
# honest majority, malicious shamir
#RUN make -j 8 malicious-shamir-party.x
#RUN Scripts/setup-ssl.sh 3
#RUN mkdir -p Player-Data

# tldr
RUN make -j 2 tldr
#RUN echo ARCH = -march=native >> CONFIG.mine
#RUN make mascot-party.x
#RUN mkdir -p Player-Data \
#        && echo 1 2 3 4 > Player-Data/Input-P0-0 \
#        && echo 1 2 3 4 > Player-Data/Input-P1-0
#
#CMD Scripts/mascot.sh tutorial

# online & offline
#RUN echo "MY_CFLAGS += -DINSECURE" >> CONFIG.mine
#RUN make -j 8 online
RUN make online
RUN make offline
#RUN ./Scripts/setup-online.sh


# shamir
RUN make malicious-shamir-party.x

## ring
#RUN Scripts/setup-ssl.sh 3
#RUN make -j 8 replicated-ring-party.x
#RUN mkdir -p Player-Data \
#        && echo 3 > Player-Data/Input-P0-0 \
#        && echo 4 > Player-Data/Input-P1-0 \
#        && echo 5 > Player-Data/Input-P2-0

RUN pip install -e Compiler/
RUN pip3 install gmpy2
RUN pip3 install gmpy
RUN pip3 install toml
RUN pip3 install leveldb
RUN pip3 install aiohttp

#CMD /bin/sh -c Scripts/ring.sh mult3
