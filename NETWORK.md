# Network Operation
For the shamir secret sharing protocol with honest majority, aka malicious shamir sharing.

Example of running a node:

```shell
malicious-shamir-party.x --prime $PRIME --hostname ${LEADER} --nparties 3 --threshold 1 --player 0 average
```

where, in the above, `PRIME` and `LEADER` are environment variables, e.g.:

```bash
# .env file

PRIME=52435875175126190479447740508185965837690552500527637822603658699938581184513
LEADER=node0
```

Note that MP-SPDZ requires some kind of leader for coordination. According to the [MP-SPDZ documentation][mp-spdz docs]:

> There are two ways of communicating hosts and individually setting ports:
>
> 1. All parties first connect to a coordination server, which broadcasts the data for all parties. This is the default with the coordination server being run as a thread of party 0. The hostname of the coordination server has to be given with the command-line parameter `--hostname`, and the coordination server runs on the base port number, thus defaulting to 5000. Furthermore, you can specify a party's listening port using `--my-port`.
>
> 2. The parties read the information from a local file, which needs to be the same everywhere. The file can be specified using `--ip-file-name` and has the following format:
>
>     ```
>     <host0>[:<port0>]
>     <host1>[:<port1>]
>     ...
>     ```
>
> The hosts can be both hostnames and IP addresses. If not given, the ports default to base plus party number.

Using the second approach, the command to run a node would be like:


```shell
malicious-shamir-party.x --prime $PRIME --ip-file-name network.config --nparties 3 --threshold 1 --player 0 average
```

> **Note**
> When using a network configuration file, it is not clear whether MP-SPDZ still requires a coordination server.


## Example with docker-compose
The docker compose file, `network.yml` can be used to run a small network of three nodes.

```shell
docker-compose --file network.yml up
```

> **Note**
The SSL keys used for encrypting messages are all in the same images for convenience, for prototyping, in a research context. Each node should only have its own private key, and the public keys of the other members. The `network.yml` and related `Dockerfile`will be modified to reflect this.


## Encryption
When using honest majority protocols such as shamir secret sharing, communication between nodes is encrypted. It's possible to disable encryption via the command-line argument `-u` or `--unencrypted`. However, this can only work if the offline phase (preprocessing) is insecure, meaning that it's "fake".

The following error will occur if the MP-SPDZ binary was compiled for a secure preprocessing phase.

```shell
You are trying to use insecure benchmarking functionality for unencrypted communication.
You can activate this at compile time by adding -DINSECURE to the compiler options.
Make sure to run 'make clean' as well before compiling.
```

It does not seem possible with the current implementation of MP-SPDZ to not use encryption with secure preprocessing. A question has been asked at https://gitter.im/MP-SPDZ/community?at=62d9dcc10a5226479868cfca.



[mp-spdz docs]: https://mp-spdz.readthedocs.io/en/latest/networking.html 
