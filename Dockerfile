FROM python:3.10.3-bullseye as base

RUN apt-get update && apt-get install -y --no-install-recommends \
                automake \
                build-essential \
                clang-11 \
                git \
                libboost-dev \
                libboost-thread-dev \
                libclang-dev \
                libntl-dev \
                libsodium-dev \
                libssl-dev \
                libtool \
                m4 \
                texinfo \
                yasm \
                vim \
                gdb \
                valgrind \
        && rm -rf /var/lib/apt/lists/*

ENV MP_SPDZ_HOME /usr/src/MP-SPDZ
WORKDIR $MP_SPDZ_HOME

RUN pip install --upgrade pip ipython

COPY . .

# mpir
COPY --from=initc3/mpir:55fe6a9 /usr/local/mpir/include/* /usr/local/include/
COPY --from=initc3/mpir:55fe6a9 /usr/local/mpir/lib/* /usr/local/lib/
COPY --from=initc3/mpir:55fe6a9 /usr/local/mpir/share/info/* /usr/local/share/info/

RUN echo "CXX = clang++-11" >> CONFIG.mine \
        && echo "USE_NTL = 1" >> CONFIG.mine \
        && echo "MY_CFLAGS += -I/usr/local/include" >> CONFIG.mine \
        && echo "MY_LDLIBS += -Wl,-rpath -Wl,/usr/local/lib -L/usr/local/lib" >> CONFIG.mine

# ssl keys
ARG n=4
RUN ./Scripts/setup-ssl.sh $n

# DEBUG and configuration flags
RUN echo "MY_CFLAGS += -DDEBUG_NETWORKING" >> CONFIG.mine \
        && echo "MY_CFLAGS += -DVERBOSE" >> CONFIG.mine \
        && echo "MY_CFLAGS += -DDEBUG_MAC" >> CONFIG.mine \
        && echo "MY_CFLAGS += -DDEBUG_FILE" >> CONFIG.mine


FROM base

ARG program="malicious-shamir-party.x"

ARG mod="-DGFP_MOD_SZ=4"
ARG prep_dir="/opt/preprocessing-data"

RUN mkdir -p $prep_dir \
        && echo "PREP_DIR = '-DPREP_DIR=\"${prep_dir}\"'" >> CONFIG.mine \
        && echo "MOD = ${mod}" >> CONFIG.mine

RUN make clean && make ${program} && cp ${program} /usr/local/bin/

RUN ./compile.py tutorial
RUN echo 1 2 3 4 > Player-Data/Input-P0-0 && echo 1 2 3 4 > Player-Data/Input-P1-0

# test with:
# malicious-shamir-party.x -N 4 -T 1 0 tutorial & malicious-shamir-party.x -N 4 -T 1 1 tutorial & malicious-shamir-party.x -N 4 -T 1 2 tutorial & malicious-shamir-party.x -N 4 -T 1 3 tutorial
