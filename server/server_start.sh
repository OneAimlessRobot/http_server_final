#!/bin/bash

executable_name="server.exe"
tmp_dir_name="/tmp/http_server_final_tmp_folder"
#sudo setcap 'cap_net_bind_service=+ep' $(pwd)/"${executable_name}"

rm -rf "${tmp_dir_name}"
$(pwd)/"${executable_name}"
rm -rf "${tmp_dir_name}"
