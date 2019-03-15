#!/bin/bash

set -e

if [ -z "$SSH_KEY_E" ]; then
  echo "No SSH key found in environment, set it as \$SSH_KEY_E" >&2
  exit 1
fi

echo "$SSH_KEY_E" |
  base64 -d |
  gunzip -c > ~/.ssh/m.id_rsa

cat >> ~/.ssh/config << EOF
Host GHMirror
  HostName github.com
  User git
  Port 22
  IdentityFile ~/.ssh/m.id_rsa
EOF
git remote add mirror GHMirror:iBug/OSH-2019-Labs.git
git push mirror +master
