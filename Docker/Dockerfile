FROM snax/builder as builder
ARG branch=master
ARG symbol=SNAX

RUN git clone -b $branch https://github.com/SnaxFoundation/snax.git --recursive \
    && cd snax && echo "$branch:$(git rev-parse HEAD)" > /etc/snax-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /tmp/build --target install && rm /tmp/build/bin/snaxcpp


FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /tmp/build/bin /opt/snax/bin
COPY --from=builder /tmp/build/contracts /contracts
COPY --from=builder /snax/Docker/config.ini /
COPY --from=builder /etc/snax-version /etc
COPY --from=builder /snax/Docker/snaxnoded.sh /opt/snax/bin/snaxnoded.sh
ENV SNAX_ROOT=/opt/snax
RUN chmod +x /opt/snax/bin/snaxnoded.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/snax/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
