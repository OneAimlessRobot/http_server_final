#!/bin/bash


the_working_dir=$(pwd)

the_target_dir_name="assets"

the_target_dir="../${the_target_dir_name}"

the_asset_list_file_name=".assetlist.txt"


the_full_asset_list_file_path="${the_working_dir}/${the_asset_list_file_name}"

rm -rf "${the_asset_list_file_name}"


touch "${the_asset_list_file_name}"


pushd "${the_target_dir}/.."


while IFS= read -r filename;
do

	echo "${filename}" >> "${the_full_asset_list_file_path}"



done < <(find "${the_target_dir_name}" -type  f )




