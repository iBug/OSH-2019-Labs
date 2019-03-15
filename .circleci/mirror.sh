#!/bin/bash

set -e

if [ -z "$SSH_KEY_E" ]; then
  echo "No SSH key found in environment, set it as \$SSH_KEY_E" >&2
  exit 1
fi

echo "$SSH_KEY_E" |
  base64 -d |
  gunzip -c > ~/.ssh/id_rsa

git remote add mirror git@github.com:iBug/OSH-2019-Labs.git
git push mirror +master
