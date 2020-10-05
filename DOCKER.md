# Working with Docker
Using `docker-compose`, build the image:

```shell
$ docker-compose build
```

Run a bash session in a container:

```shell
$ docker-compose run --rm mpspdz bash
```

Run the hbmpc mimc test:

```shell
root@9ed3dffcc708:/usr/src/MP-SPDZ# ./Programs/Source/hbmpc_mimc_test.sh
```

Compile a program:

```shell
root@9ed3dffcc708:/usr/src/MP-SPDZ# mpspdz-compile -v -C -F 256 hbmpc_mimc_test
```

## Notes about the Dockerfile
For convenience the `Dockerfile` builds the following (targets from
`Makefile`):

* tldr
* online
* offline
* shamir
* replicated-ring-party.x

The `Dockerfile` installs the `mpir` library so that it does not need to be
built, which speeds up the overall building phase.

The compiler (which includes the console script `mpspdz-compiler`) is also
installed via the `Dockerfile`.
