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
Snax is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the Snax software depends in part on how it is used, configured, and deployed. Snax is built upon many third-party libraries such as Binaryen (Apache License) and WAVM  (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Block.one makes no representation or guarantee that Snax or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement Snax, you do so at your own risk. In no event will Block.one be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.  

## Installation

**If you have previously installed Snax, please run the `snax_uninstall` script (it is in the directory where you cloned Snax) before downloading and using the binary releases.**

### Mac OS X Brew Install
```sh
$ brew tap snax/snax
$ brew install snax
```
### Mac OS X Brew Uninstall
```sh
$ brew remove snax
```
### Ubuntu 18.04 Debian Package Install
```sh
$ wget https://github.com/SnaxFoundation/snax/releases/download/v0.1.0/snax-0.1.0.ubuntu-18.04-x86_64.deb
$ sudo apt install ./snax-1.4.3.ubuntu-18.04-x86_64.deb
```
### Ubuntu 16.04 Debian Package Install
```sh
$ wget https://github.com/SnaxFoundation/snax/releases/download/v1.4.3/snax-1.4.3.ubuntu-16.04-x86_64.deb
$ sudo apt install ./snax-1.4.3.ubuntu-16.04-x86_64.deb
```
### Debian Package Uninstall
```sh
$ sudo apt remove snax
```
### RPM Package Install
```sh
$ wget https://github.com/SnaxFoundation/snax/releases/download/v1.4.3/snax-1.4.3.x86_64-0.x86_64.rpm
$ sudo yum install ./snax-1.4.3.x86_64-0.x86_64.rpm
```
### RPM Package Uninstall
```sh
$ sudo yum remove snax.cdt
```

## Supported OS

Snax currently supports the following operating systems:  
1. Amazon 2017.09 and higher
2. Centos 7
3. Fedora 25 and higher (Fedora 27 recommended)
4. Mint 18
5. Ubuntu 16.04 (Ubuntu 16.10 recommended)
6. Ubuntu 18.04
7. MacOS Darwin 10.12 and higher (MacOS 10.13.x recommended)

## Resources
1. [Website](https://snax.one)
2. [Blog](https://medium.com/@snax)
3. [Discord](https://discord.gg/qygxJAZ)
4. [White Paper](https://snax.one/whitepaper.pdf)
5. [Roadmap](https://snax.one/roadmap)
