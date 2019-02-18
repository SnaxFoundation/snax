# Snax - Blockchain-Based Social Overlay

## Some of the key features of Snax

1. Free rate limited transactions
2. Free account registration
3. Social transactions over the most popular social platforms (e.g. Twitter, Reddit)
4. Low latency block confirmation (0.5 seconds)
5. Low-overhead Byzantine Fault Tolerant Finality
6. Smart contract platform powered by Web Assembly
7. Designed for sparse header light client validation
8. Hierarchical role based permissions
9. Support for biometric hardware secured keys
10. Designed for parallel execution of context free validation logic

## Disclaimer
Snax is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the Snax software depends in part on how it is used, configured, and deployed. Snax is built upon many third-party libraries such as Binaryen (Apache License) and WAVM  (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Snax Foundation makes no representation or guarantee that Snax or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement Snax, you do so at your own risk. In no event will Snax Foundation be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.  

## Requirements

```sh
libssl >= 1.0.0
GLIBCXX >= 3.4.20
```

## Installation

### Minimal hardware configuration

*  4 cores
*  32 GB RAM for testnet and 64 GB for mainnet. We're trying to reduce mainnet requirements to 32 GB.
*  128 GB SSD for testnet, 512 GB SSD for mainnet
*  100 mbit\s Internet speed

### With binaries

Download the latest release from [https://github.com/SnaxFoundation/snax/releases](https://github.com/SnaxFoundation/snax/releases)

```sh
tar xvf bin.tar.gz
```

### With Docker

1. Install [Docker](https://docs.docker.com/install)
2. Create Snax account (or ask Snax team for testnet account in [Discord](https://discord.gg/qygxJAZ))
3. Create directory for node data 
```sh
mkdir $HOME/producer
```
4. Open p2p port on your firewall
```sh
iptables -I INPUT -p tcp --dport 9876 -j ACCEPT
```
5. To run Snax node as a block-producer
```sh
docker run -d --restart=always --network=host --name producer \
           -v $HOME/producer:/opt/snax/data snaxfoundation/snax:0.2.0-beta-4 \
           snaxnoded.sh --signature-provider your_public_key=KEY:your_private_key \
           --enable-stale-production \
           --plugin=snax::producer_plugin \
           --producer-name=SNAX_ACCOUNT_NAME \
           --max-clients=0 \
           --http-validate-host=false \
           --plugin=snax::history_api_plugin \
           --p2p-peer-address=18.136.191.21:9876 \
           --p2p-peer-address=54.71.79.75:9876 \
           --p2p-peer-address=80.66.90.29:9876 \
           --p2p-peer-address=92.53.79.186:9876 \
           --p2p-peer-address=92.53.79.190:9876
```

## Supported OS

Snax currently supports only Linux x64

## Resources
1. [Website](https://snax.one)
2. [Blog](https://medium.com/@snax)
3. [Discord](https://discord.gg/qygxJAZ)
4. [White Paper](https://snax.one/whitepaper.pdf)
5. [Roadmap](https://snax.one/roadmap)
6. [Telegram](https://t.me/snaxone)
