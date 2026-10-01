#!/bin/bash

EAB_KID="YOjleVTHA5vuFSIadmm92Q"
EAB_HMAC_KEY="_vkySok1pFKnJ2dHldZj8GlNX9UMZAST0a-KpA0zrwPtr37Qx1PEv7w_6ebE24SjXovB5SJs5pqpQ2gxdB49sQ"
the_curr_directory=$(pwd)
echo "The current directory is: \"${the_curr_directory}\""
the_email_address="thrashbrat@gmail.com"
the_server="https://acme.zerossl.com/v2/DV90"
the_domains=("sablinasemporium.ddns.net" "lenasattic.ddns.net" "localhost")
the_domains_formatted_for_command=()
num_of_domains="${#the_domains[@]}"

#echo "${num_of_domains}"

#echo "${the_domains[@]}"

for(( i= 0; i< $num_of_domains; i++));
do

	the_domains_formatted_for_command[$i]=" -d ${the_domains[$i]} "
	#echo "${the_domains_formatted_for_command[$i]}"

done;

num_of_formatted_domains="${#the_domains_formatted_for_command[@]}"

echo "${num_of_formatted_domains}"

the_full_domain_list_string="${the_domains_formatted_for_command[@]}"

#only for registering!

#echo "${the_email_address}" | sudo certbot register --server "${the_server}" --eab-kid "${EAB_KID}" --eab-hmac-key "${EAB_HMAC_KEY}" --agree-tos

the_command="sudo certbot  certonly   -v   --server ${the_server}  --webroot -w ${the_curr_directory}/ $the_full_domain_list_string  --force-interactive"

echo "${the_email_address}" && echo "2"  && echo "E" | $the_command

#this one will be really old and bad tho
#sudo certbot certonly --key-type rsa --rsa-key-size 2048 -d "${the_domain}"
