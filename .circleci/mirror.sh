#!/bin/bash

set -e

if [ -z "$GH_TOKEN" ]; then
  echo "Missing \$GH_TOKEN" >&2
  exit 1
fi

git remote add mirror "https://$GH_TOKEN@github.com/iBug/OSH-2019-Labs.git"
git push -q mirror +master
