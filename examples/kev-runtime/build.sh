#!/bin/bash

# compile the firmware with Kevoree Core
make
# create version with all symbols to support dynamic loading
# read Contiki's blog if you don't get this
make kev-runtime.iotlab-m3
make CORE=kev-runtime.iotlab-m3 kev-runtime.iotlab-m3
make CORE=kev-runtime.iotlab-m3 kev-runtime.iotlab-m3

# compile components
make components

# compile the client, this fake application we created to make our life as developers easier
make client

# compile the server implementing the software repository
make repository-server
