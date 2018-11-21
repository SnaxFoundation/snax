FROM snax/builder
ARG branch=master
ARG symbol=SNAX

RUN git clone -b $branch https://github.com/SnaxFoundation/snax.git --recursive \
    && cd snax && echo "$branch:$(git rev-parse HEAD)" > /etc/snax-version \
    && cmake -H. -B"/opt/snax" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/opt/snax -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /opt/snax --target install \
    && cp /snax/Docker/config.ini / && ln -s /opt/snax/contracts /contracts && cp /snax/Docker/snaxnoded.sh /opt/snax/bin/snaxnoded.sh && ln -s /snax/tutorials /tutorials

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates vim psmisc python3-pip && rm -rf /var/lib/apt/lists/*
RUN pip3 install numpy
ENV SNAX_ROOT=/opt/snax
RUN chmod +x /opt/snax/bin/snaxnoded.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/snax/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
