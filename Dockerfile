FROM ubuntu:18.04

ENV SNAX_ROOT=/opt/snax
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/snax/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

COPY lib/ /usr/local/lib/
COPY bin/ /opt/snax/bin/
COPY build/contracts /contracts
COPY Docker/config.ini /
COPY genesis.json /
COPY snax-version /etc
COPY Docker/*.sh /opt/snax/bin/

RUN apt-get update \
      && DEBIAN_FRONTEND=noninteractive \
      apt-get -y install \
        openssl \
        libssl1.0.0 \
        libssl-dev \
        ca-certificates \
      && rm -rf /var/lib/apt/lists/* \
      && chmod +x /opt/snax/bin/*.sh

EXPOSE 8888 9876 8900
