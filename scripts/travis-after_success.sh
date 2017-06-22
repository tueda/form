#!/bin/bash
set -eu
set -o pipefail

case $CI_TARGET in
  *coverage*)
    bash <(curl -s https://codecov.io/bash)
    ;;
esac
