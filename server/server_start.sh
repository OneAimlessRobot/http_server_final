#!/bin/bash

executable_name="server.exe"

sudo setcap 'cap_net_bind_service=+ep' $(pwd)/"${executable_name}"

$(pwd)/"${executable_name}"
