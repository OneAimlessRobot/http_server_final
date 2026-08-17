#!/bin/bash

EAB_KID="YOjleVTHA5vuFSIadmm92Q"
EAB_HMAC_KEY="_vkySok1pFKnJ2dHldZj8GlNX9UMZAST0a-KpA0zrwPtr37Qx1PEv7w_6ebE24SjXovB5SJs5pqpQ2gxdB49sQ"
the_curr_directory=$(pwd)
echo "The current directory is: \"${the_curr_directory}\""
the_email_address="thrashbrat@gmail.com"
the_server="https://acme.zerossl.com/v2/DV90"
the_domain="lenasattic.ddns.net"
#try this one
#sudo certbot register --server "${the_server}" --email "${the_email_address}" --agree-tos

#echo "${the_email_address}" | sudo certbot register --server "${the_server}" --eab-kid "${EAB_KID}" --eab-hmac-key "${EAB_HMAC_KEY}" --agree-tos
echo "${the_email_address}" && echo "2" | sudo certbot  certonly   -v   --server "${the_server}"  --webroot -w "${the_curr_directory}/" -d "${the_domain}" --force-interactive

#this one will be really old and bad tho
#sudo certbot certonly --key-type rsa --rsa-key-size 2048 -d "${the_domain}"
