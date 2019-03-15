#!/bin/bash

set -e

if [ -z "$SSH_KEY_E" ]; then
  echo "No SSH key found in environment, set it as \$SSH_KEY_E" >&2
fi

base64 -d <<< "$SSH_KEY_E" | gunzip -c > ~/.ssh/id_rsa

git remote add mirror git@github.com:iBug/OSH-2019-Labs.git
git push mirror +master
