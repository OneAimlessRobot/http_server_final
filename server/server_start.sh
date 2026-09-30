#!/bin/bash

executable_name="server.exe"

set tmp_file_dir

tmp_file_dir="/tmp/http_server_final_tmp_folder"

#sudo setcap 'cap_net_bind_service=+ep' $(pwd)/"${executable_name}"

rm -rf "${tmp_file_dir}"

#gdb version

#tmp_file_dir="${tmp_file_dir}" gdb "$(pwd)/${executable_name}"

tmp_file_dir="${tmp_file_dir}" $(pwd)/"${executable_name}"

rm -rf "${tmp_file_dir}"
