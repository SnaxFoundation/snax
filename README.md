# Snax - Decentralized Social Media Overlay

## Some of the key features of Snax

1. Free rate limited transactions
2. Free account registration
3. Social Transactions over the most popular social platforms (e.g. Twitter, Reddit)
4. Token supply based on Regular Publisher Rewards system for content creators on public social media
5. Low latency block confirmation (0.5 seconds)
6. Low-overhead Byzantine Fault Tolerant Finality
7. Smart contract platform powered by Web Assembly
8. Designed for sparse header light client validation
9. Hierarchical role based permissions
10. Support for biometric hardware secured keys
11. Designed for parallel execution of context free validation logic

## Disclaimer

Snax is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the Snax software depends in part on how it is used, configured, and deployed. Snax is built upon many third-party libraries such as Binaryen (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Snax Foundation makes no representation or guarantee that Snax or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement Snax, you do so at your own risk. In no event will Snax Foundation be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.

## Requirements

```sh
libssl >= 1.0.0
GLIBCXX >= 3.4.20
```

## Supported OS

Snax currently supports only Linux x64 and macOS.

## Run as Block Producer

### 1. Minimal hardware configuration

- 4 cores
- 64 GB RAM
- 512 GB SSD
- 100 mbit\s Internet speed

### 2. Create wallet and account in Snax

1. Download [Snax extension](https://chrome.google.com/webstore/detail/snax/dolcmddbbplempeembpecnpllnbgjlal) for Chrome browser (or use cli if you want)
2. Create Snax wallet with it
3. Create Snax account (or ask Snax team for testnet account in [Discord](https://discord.gg/qygxJAZ))

### 3. Run Snax node with Docker

1. Install [Docker](https://docs.docker.com/install)
2. Create directory for node data

```sh
mkdir $HOME/producer
```

3. Open p2p port on your firewall

```sh
iptables -I INPUT -p tcp --dport 9876 -j ACCEPT
```

4. Run your node with Docker

```sh
docker run -d --restart=always --network=host --name producer \
           -v $HOME/producer:/opt/snax/data snaxfoundation/snax:1.0.1 \
           snaxnoded.sh --signature-provider put_your_public_key_here=KEY:put_your_private_key_here \
           --plugin=snax::producer_plugin \
           --producer-name=put_your_snax_account_name_here \
           --max-clients=20 \
           --http-validate-host=false \
           --p2p-server-address=YOU.PUBLIC.IP:9876 \
           --agent-name="YOU_PRODUCER_NAME" \
           --network-version-match=1 \
           --p2p-peer-address=92.53.79.186:9876 \
           --p2p-peer-address=54.71.79.75:9876 \
           --p2p-peer-address=80.66.90.29:9876 \
           --p2p-peer-address=92.53.79.190:9876 \
           --p2p-peer-address=snax.eossweden.eu:9022 \
           --p2p-peer-address=snax-mainnet-p2p.eosarabia.one:7883 \
           --p2p-peer-address=snax-peer1.eoskh.com:8977 \
           --p2p-peer-address=snax-peer3.eoskh.com:8847 \
           --p2p-peer-address=213.133.102.220:9876 \
           --p2p-peer-address=89.163.242.27:9876 \
           --p2p-peer-address=18.217.111.82:9876 \
           --p2p-peer-address=snaxp2p.eclipse24.io:9877 \
           --p2p-peer-address=snax.eosnairobi.io:7276 \
           --p2p-peer-address=95.216.245.221:5032 \
           --p2p-peer-address=sxn1.eosviet.io:8848 \
           --p2p-peer-address=sxn3.eosviet.io:8978 \
           --p2p-peer-address=p1-snax.eosph.io:8849 \
           --p2p-peer-address=p3-snax.eosph.io:8979 \
           --p2p-peer-address=speer1.nodeone.io:8976 \
           --p2p-peer-address=speer3.nodeone.io:8846 \
           --p2p-peer-address=p2p.snax.telosunlimited.io:9876 \
           --p2p-peer-address=sslapi.teloscentral.com:9993 \
           --p2p-peer-address=node.snax.world:8821 \
           --p2p-peer-address=157.230.219.218:8888 \
           --p2p-peer-address=snaxbp.germaneos.de:7777 \
           --p2p-peer-address=snax.onedac.io:7976 \
           --p2p-peer-address=peer.snax-mainnet.eosblocksmith.io:9876 \
           --p2p-peer-address=213.239.213.227:9876 \
           --p2p-peer-address=157.230.135.197:9876 \
           --p2p-peer-address=p2p.snax.kennybll.com:9876 \
           --p2p-peer-address=snax.crypto4shopper.com:9876
```

### 4. Run kxd keystore with Docker

1. Create directory for keystore

```sh
mkdir $HOME/snax-wallet
```

2. Run kxd keystore with Docker

```sh
docker run  --name=snax-wallet -d -v $HOME/snax-wallet:/root/snax-wallet \
            --restart=always -p 127.0.0.1:8900:8900 snaxfoundation/snax:1.0.1 kxd.sh \
            --http-server-address=0.0.0.0:8900 \
            --access-control-allow-origin=* \
            --access-control-allow-headers=* \
            --http-validate-host=0 \
            --http-alias=snax_kxd:8900
```

### 5. Register yourself as a BP candidate

1. Download the latest release from [https://github.com/SnaxFoundation/snax/releases](https://github.com/SnaxFoundation/snax/releases)
2. Unpack binaries

```sh
tar xvf bin.tar.gz
```

3. Create your wallet

```sh
clisnax --wallet-url http://127.0.0.1:8900 wallet create --to-console
```

4. Import your private key

```sh
clisnax --wallet-url http://127.0.0.1:8900 wallet import --private-key put_your_private_key_here
```

5. Unlock your wallet

```sh
clisnax --wallet-url http://127.0.0.1:8900 wallet unlock
```

6. Register yourself as a BP candidate

```sh
clisnax --url https://cdn.snax.one \
        --wallet-url http://127.0.0.1:8900 system regproducer put_your_snax_account_name_here put_your_public_key_here
```

**7. LOCK YOUR WALLET!**

```sh
clisnax --wallet-url http://127.0.0.1:8900 wallet lock
```

## If you want to build manually

```sh
git clone https://github.com/SnaxFoundation/snax.git
cd snax
bash ./snax_build.sh
sudo bash ./snax_install.sh
```

## Resources

1. [Website](https://snax.one)
2. [Blog](https://medium.com/@snax)
3. [Discord](https://discord.gg/qygxJAZ)
4. [Twitter](https://twitter.com/SnaxTeam)
5. [White Paper](https://snax.one/whitepaper.pdf)
6. [Roadmap](https://snax.one/roadmap)
