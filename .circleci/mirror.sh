#!/bin/bash

set -e

if [ -z "$SSH_KEY_E" ]; then
  echo "No SSH key found in environment, set it as \$SSH_KEY_E" >&2
fi

base64 -d <<< "$SSH_KEY_E" | gunzip -c > ~/.ssh/id_rsa
chmod 600 ~/.ssh/id_rsa
echo ls -l ~/.ssh/id_rsa && ls -l ~/.ssh/id_rsa

export GIT_SSH_COMMAND="ssh -vv"
export SSH_AUTH_SOCK=none
#export GIT_SSH_COMMAND="ssh -i $HOME/.ssh/id_rsa"
cat >> ~/.ssh/config << EOF
Host github.com
  User git
  IdentityFile ~/.ssh/id_rsa
EOF

git remote add mirror git@github.com:iBug/OSH-2019-Labs.git
git push mirror +master
