#!/bin/bash

curr_dir=$(pwd)
pushd "${curr_dir}/.."
bash ./updateRepoMasterBranch.sh
popd
