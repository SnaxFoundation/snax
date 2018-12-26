# Run in docker

Simple and fast setup of SNAX on Docker is also available.

## Install Dependencies

- [Docker](https://docs.docker.com) Docker 17.05 or higher is required
- [docker-compose](https://docs.docker.com/compose/) version >= 1.10.0

## Docker Requirement

- At least 7GB RAM (Docker -> Preferences -> Advanced -> Memory -> 7GB or above)
- If the build below fails, make sure you've adjusted Docker Memory settings and try again.

## Build snax image

```bash
git clone https://github.com/SnaxFoundation/snax.git --recursive  --depth 1
cd snax/Docker
docker build . -t snax/snax
```

The above will build off the most recent commit to the master branch by default. If you would like to target a specific branch/tag, you may use a build argument. For example, if you wished to generate a docker image based off of the v1.4.3 tag, you could do the following:

```bash
docker build -t snax/snax:v1.5.2 --build-arg branch=v1.5.2 .
```

By default, the symbol in snax.system is set to SNAX. You can override this using the symbol argument while building the docker image.

```bash
docker build -t snax/snax --build-arg symbol=<symbol> .
```

## Start snaxnode docker container only

```bash
docker run --name snaxnode -p 8888:8888 -p 9876:9876 -t snax/snax snaxnoded.sh -e --http-alias=snaxnode:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888 arg1 arg2
```

By default, all data is persisted in a docker volume. It can be deleted if the data is outdated or corrupted:

```bash
$ docker inspect --format '{{ range .Mounts }}{{ .Name }} {{ end }}' snaxnode
fdc265730a4f697346fa8b078c176e315b959e79365fc9cbd11f090ea0cb5cbc
$ docker volume rm fdc265730a4f697346fa8b078c176e315b959e79365fc9cbd11f090ea0cb5cbc
```

Alternately, you can directly mount host directory into the container

```bash
docker run --name snaxnode -v /path-to-data-dir:/opt/snax/bin/data-dir -p 8888:8888 -p 9876:9876 -t snax/snax snaxnoded.sh -e --http-alias=snaxnode:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888 arg1 arg2
```

## Get chain info

```bash
curl http://127.0.0.1:8888/v1/chain/get_info
```

## Start both snaxnode and kxd containers

```bash
docker volume create --name=snaxnode-data-volume
docker volume create --name=kxd-data-volume
docker-compose up -d
```

After `docker-compose up -d`, two services named `snaxnoded` and `kxd` will be started. snaxnode service would expose ports 8888 and 9876 to the host. kxd service does not expose any port to the host, it is only accessible to clisnax when running clisnax is running inside the kxd container as described in "Execute clisnax commands" section.

### Execute clisnax commands

You can run the `clisnax` commands via a bash alias.

```bash
alias clisnax='docker-compose exec kxd /opt/snax/bin/clisnax -u http://snaxnoded:8888 --wallet-url http://localhost:8900'
clisnax get info
clisnax get account inita
```

Upload sample exchange contract

```bash
clisnax set contract exchange contracts/exchange/
```

If you don't need kxd afterwards, you can stop the kxd service using

```bash
docker-compose stop kxd
```

### Develop/Build custom contracts

Due to the fact that the snax/snax image does not contain the required dependencies for contract development (this is by design, to keep the image size small), you will need to utilize the snax/snax-dev image. This image contains both the required binaries and dependencies to build contracts using snaxcpp.

You can either use the image available on [Docker Hub](https://hub.docker.com/r/snax/snax-dev/) or navigate into the dev folder and build the image manually.

```bash
cd dev
docker build -t snax/snax-dev .
```

### Change default configuration

You can use docker compose override file to change the default configurations. For example, create an alternate config file `config2.ini` and a `docker-compose.override.yml` with the following content.

```yaml
version: "2"

services:
  snaxnode:
    volumes:
      - snaxnode-data-volume:/opt/snax/bin/data-dir
      - ./config2.ini:/opt/snax/bin/data-dir/config.ini
```

Then restart your docker containers as follows:

```bash
docker-compose down
docker-compose up
```

### Clear data-dir

The data volume created by docker-compose can be deleted as follows:

```bash
docker volume rm snaxnode-data-volume
docker volume rm kxd-data-volume
```

### Docker Hub

Docker Hub image available from [docker hub](https://hub.docker.com/r/snax/snax/).
Create a new `docker-compose.yaml` file with the content below

```bash
version: "3"

services:
  snaxnoded:
    image: snax/snax:latest
    command: /opt/snax/bin/snaxnoded.sh --data-dir /opt/snax/bin/data-dir -e --http-alias=snaxnoded:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888
    hostname: snaxnoded
    ports:
      - 8888:8888
      - 9876:9876
    expose:
      - "8888"
    volumes:
      - snaxnode-data-volume:/opt/snax/bin/data-dir

  kxd:
    image: snax/snax:latest
    command: /opt/snax/bin/kxd --wallet-dir /opt/snax/bin/data-dir --http-server-address=127.0.0.1:8900 --http-alias=localhost:8900 --http-alias=kxd:8900
    hostname: kxd
    links:
      - snaxnoded
    volumes:
      - kxd-data-volume:/opt/snax/bin/data-dir

volumes:
  snaxnode-data-volume:
  kxd-data-volume:

```

*NOTE:* the default version is the latest, you can change it to what you want

run `docker pull snax/snax:latest`

run `docker-compose up`

### SNAX Testnet

We can easily set up a SNAX local testnet using docker images. Just run the following commands:

Note: if you want to use the mongo db plugin, you have to enable it in your `data-dir/config.ini` first.

```
# create volume
docker volume create --name=snaxnode-data-volume
docker volume create --name=kxd-data-volume
# pull images and start containers
docker-compose -f docker-compose-snax-latest.yaml up -d
# get chain info
curl http://127.0.0.1:8888/v1/chain/get_info
# get logs
docker-compose logs -f snaxnoded
# stop containers
docker-compose -f docker-compose-snax-latest.yaml down
```

The `blocks` data are stored under `--data-dir` by default, and the wallet files are stored under `--wallet-dir` by default, of course you can change these as you want.

### About MongoDB Plugin

Currently, the mongodb plugin is disabled in `config.ini` by default, you have to change it manually in `config.ini` or you can mount a `config.ini` file to `/opt/snax/bin/data-dir/config.ini` in the docker-compose file.
