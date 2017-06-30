#!/bin/bash
set -eu
set -o pipefail

# Print all executed commands to the log.
set -x

if [ "x$TRAVIS_OS_NAME" = xlinux ]; then
  case $CI_TARGET in
    *coverage*)
      pip install --user cpp-coveralls
      ;;
  esac
  case $CI_TARGET in
    *doc*)
      # Install TeX Live to "./texlive".
      if [ ! -e ./texlive/bin/`uname -m`-linux/tlmgr ]; then
        wget http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz -O - | tar -x --gzip
        echo "
        selected_scheme scheme-minimal
        TEXDIR ./texlive
        TEXMFCONFIG ~/.texlive2016/texmf-config
        TEXMFHOME ~/texmf
        TEXMFLOCAL ./texlive/texmf-local
        TEXMFSYSCONFIG ./texlive/texmf-config
        TEXMFSYSVAR ./texlive/texmf-var
        TEXMFVAR ~/.texlive2016/texmf-var
        collection-fontsrecommended 1
        collection-latex 1
        option_doc 0
        option_src 0
        " | sed -e 's/^ *//' >texlive.profile
        ./install-tl-20*/install-tl --profile texlive.profile
      fi
      export PATH=`pwd`/texlive/bin/`uname -m`-linux:$PATH
      ;;
  esac
  case $CI_TARGET in
    *doc-html*)
      # Install LaTeX2HTML to the TeX Live directory.
      if [ ! -e ./texlive/bin/`uname -m`-linux/latex2html ]; then
        wget http://mirrors.ctan.org/support/latex2html/latex2html-2017.2.tar.gz -O - | tar -x --gzip
        (
          cd latex2html-*
          ./configure --prefix=$TRAVIS_BUILD_DIR/texlive/texmf-local/latex2html
          make install
        )
        (
          cd texlive/bin/`uname -m`-linux
          ln -s ../../texmf-local/latex2html/bin/latex2html
          ln -s ../../texmf-local/latex2html/bin/pstoimg
          ln -s ../../texmf-local/latex2html/bin/texexpand
        )
      fi
      ;;
  esac
fi

if [ "x$TRAVIS_OS_NAME" = xosx ]; then
  case $CI_TARGET in
    *parform*|*parvorm*)
      brew update
      brew install mpich
      ;;
  esac
  case $CI_TARGET in
    *valgrind*)
      brew update
      # valgrind 3.11.0
      brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/7a4dabfc1a2acd9f01a1670fde4f0094c4fb6ffa/Formula/valgrind.rb
      ;;
  esac
  case $CI_TARGET in
    *coverage*)
      # NOTE: Python needs a manual setup on osx: travis-ci/travis-ci#2312.
      if type pyenv >/dev/null 2>&1; then :;else
        brew update
        brew install pyenv
      fi
      eval "$(pyenv init -)"
      pyenv install 2.7.12
      pyenv global 2.7.12
      pyenv rehash
      pip install cpp-coveralls
      pyenv rehash
      ;;
  esac
fi

case $CI_TARGET in
  form|tform)
    # Install Forcer to "./formlib".
    mkdir -p formlib
    wget https://github.com/benruijl/forcer/archive/v1.0.0.tar.gz -O - | tar -x --gzip
    mv forcer-1.0.0/forcer.h formlib
    mv forcer-1.0.0/forcer formlib
    rm -rf forcer-1.0.0
    ;;
esac

case $CI_TARGET in
  *form*|*vorm*)
    gem install test-unit
    ;;
esac
