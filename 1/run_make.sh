#!/bin/bash
PATH=$(dirname "$0")/../tools/bin:$PATH exec make "$@"
