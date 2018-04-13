# dns-fuzz-server

## Overview

`dns-fuzz-server` is the fuzzing tool for DNS full-resolvers(BIND, Unbound, PowerDNS Recursor, knot-resolver, etc.). It is used for finding the bugs or vulnerabilities by replying crafted DNS messages to DNS full-resolvers.

## Quick Start

Install CentOS 7.4.

Install packages for compiling dns-fuzz-server.

```
# yum install epel-release
# yum install gcc-c++ boost-devel gtest-devel wget perl yaml-cpp-devel bind-utils bind

# wget https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.sh
# sh cmake-3.10.0-Linux-x86_64.sh --skip-license --prefix=/usr/local

# wget https://www.openssl.org/source/openssl-1.1.0g.tar.gz
# tar xzf openssl-1.1.0g.tar.gz
# cd openssl-1.1.0g
# ./config
# make
# make install
# echo /usr/local/lib64 > /etc/ld.so.conf.d/local.conf
# ldconfig
```

Compile dns-fuzz-server.

```
$ tar xzf /path/to/dns-fuzz-server-x.x.x.tar.gz
$ cd dns-fuzz-server
$ cmake .
$ make
```

Create zonefile, KSK, ZSK for the domain `example.com`.

```
$ named-checkzone -s full -o data/example.com.zone.full example.com data/example.com.zone
$ (cd data && ./keygen.sh example.com RSASHA256 )
```

Start `fuzz_server`, then `fuzz_server` outputs DS records to stdout. 
If the full-resolver is enabled DNSSEC-validation, append DS records to the parents zone.
```
# ./bin/fuzz_server -z example.com -f data/example.com.zone.full -K data/example.com.ksk.yaml -Z data/example.com.zsk.yaml
DS records
example.com.   IN DS 4657 5 1 8761401F319E1BF7B4B40709EBE035B6F3C12BC5
example.com.   IN DS 4657 5 2 DE3C43E82DCAAA1BC03A936FE3C25E16DFA9C3490F7B52270975B89E79DCB178
example.com.   IN DS 4657 5 3 09F0EFC1E7C2E9B3C2FB20448BC103FFF611718973C402022BFB77D2E5C7FCD7A74AAAD6250C26ACFDF6EB5C06D0FDAE
```

Send queries to full-resolver server.

```
$ ./bin/fuzz_client -s <full_resolver_ip_address> -b example.com
```

## Requirement softwares

* gcc-c++ ( support c++11 )
* boost
* openssl 1.1.0
* yaml-cpp 6 (https://github.com/jbeder/yaml-cpp )
* cmake >= 3.8


## Available command

* ./bin/fuzz_server: DNS authoritative server for fuzzing.
* ./bin/fuzz_client: DNS client for fuzzing.
* ./data/keygen.sh: script to generate KSK, ZSK.

## fuzz_server

`fuzz_server` is DNS authoritative server for fuzzing.

### Command argument

```
fuzz server [ -b <address> ] [ -p <port> ] -z <zone> -f <zone.file> -K <ksk.yaml> -Z <zsk.yaml> [ -d ]
```

* `-h [ --help ]`: help mesage
* `-b / --bind <address>`: Bind IP address
* `-p / --port <port>`: Bind UDP/TCP port
* `-z / --zone <zone>`: zone name
* `-f / --file <zone.file>`: zone filename(see "Zone file format")
* `-K / --ksk  <ksk.yaml>`: KSK configration YAML file
* `-Z / --zsk  <zsk.yaml>`: ZSK configration YAML file
* `-d / --debug`: debug mode

### Example

```
 ./fuzz_server -z example.com -f data/example.com.zone.full -K data/example.com.ksk.yaml -Z data/example.com.zsk.yaml
```

### Zone file Fomrat

`fuzz_server` does not support the common zone file defined at RFC1035. The zone file, which can be load by `fuzz_server`,
is generated by `named-checkzone -s full` command from RFC1035 zone file.

#### Example

RFC1035 zone file `example.com.zone`.

```
$ cat example.com.zone

$TTL 3600
example.com. 3600       IN SOA  ns01.example.co. hostmaster.example.com.  (
                                        20170531        ; serial
                                        3600    ; refresh
                                        1800    ; retry
                                        8640000 ; expire
                                        3600 )  ; minimum
        3600 IN NS      ns01
        3600 IN NS      ns02
        3600 IN MX 10   mail


ns01    86400 IN A      192.168.33.11
ns02    86400 IN A      192.168.33.12
www     3600  IN A      192.168.33.101
www     3600  IN A      192.168.33.102
mail    3600  IN A      192.168.33.111
```

Convert from RFC1035 zone file by `named-checkzone`.

```
$ named-checkzone  -s full -o example.com.zone.full example.com example.com.zone
```

Converted zone file.

```
$ cat example.com.zone.full
example.com.                                  3600 IN SOA       ns01.example.co. hostmaster.example.com. 20170531 3600 1800 8640000 3600
example.com.                                  3600 IN NS        ns01.example.com.
example.com.                                  3600 IN NS        ns02.example.com.
example.com.                                  3600 IN MX        10 mail.example.com.
mail.example.com.                             3600 IN A         192.168.33.111
ns01.example.com.                             86400 IN A        192.168.33.11
ns02.example.com.                             86400 IN A        192.168.33.12
www.example.com.                              3600 IN A         192.168.33.101
www.example.com.                              3600 IN A         192.168.33.102
```

### KSK, ZSK configration YAML files format

KSK, ZSK configration files are following format. These files can be generated by `keygen.sh` script.

```
---
- domain: <domain_0>
  type: <ksk_or_zsk_0>
  algorithm: <algorithm_0>
  not_before: <not_before_epoch_0>
  not_after: <not_after_epoch_0>
  key_file: <path_to_private_key_0>
```

* <domain_n>: KSK, ZSK domainname.
* <ksk_or_zsk_N>: `ksk` or `zsk`
* <algorithm_N>: `RSASHA1` or `RSASHA256` or `ECDSAP256SHA256`
* <not_before_epoch_N>: epoch time.
* <not_after_epoch_N>: epoch time.
* <path_to_private_key_N>: path to private key.


#### KSK configration example

```
---
- domain: example.com
  type: ksk
  algorithm: RSASHA256
  not_before: 1523457711
  not_after: 1524321711
  key_file: /dns/data/keys/example.com.ksk.0.key
```

#### ZSK configration example

```
---
- domain: example.com
  type: zsk
  algorithm: RSASHA256
  not_before: 1523457711
  not_after: 1524321711
  key_file: /dns/data/keys/example.com.zsk.0.key
```


### Generate KSK, ZSK

KSK/ZSK is gerarated bt `openssl` command.

#### Example

```
$ openssl genrsa -out /dns/data/keys/example.com.zsk.0.key
$ openssl ecparam -genkey -name prime256v1 -out /dns/data/keys/example.com.ksk.0.key
```


## fuzz_client

`fuzz_client` command sends DNS queries to full-resolver server for fuzzing.
The QNAMEs of DNS Queries is specified `-b` option argument, The QTYPE and QCLASS are specified randomly.

### Example of QNAME

If `fuzz_client` contains `-b example.com` option, QNAMEs of DNS Queries is following, 
 `-b example.com`を指定すると、以下のようにQNAMEを生成します。
* example.com
* www.example.com
* yyy.example.com
* www.yyy.example.com
...

### Command Argments

```
fuzz_client -s <server> [ -p <port> ] -b <base> [-i <interval> ]
```

* `-h/--help`: print this message
* `-s/--server <server>`: IP address of destination full-resolver server.
* `-p/--port <port>`:  Port number of destination full-resolver server.
* `-b/--base <base>`: QNAME suffix.
* `-i/--interval <interval>`: Interval of each DNS queries(msec).

## keygen.sh

`keygen.sh` generates KSK, ZSK, and their configration files.

### Command arguments

```
keygen.sh <domain> <algorithm>
```

* <domain>: domainname.
* <algorithm>: KSK,ZSK algorithm. `RSASHA1` or `RSASHA256` or `ECDSAP256SHA256`

### Example

```
$ ./keygen.sh example.com RSASHA256
```


