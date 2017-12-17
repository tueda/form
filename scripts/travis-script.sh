#!/bin/bash
set -eu
set -o pipefail

if [ -d `pwd`/mpich/bin ]; then
  export PATH=`pwd`/mpich/bin:$PATH
fi
if [ -d `pwd`/texlive/bin ]; then
  export PATH=`pwd`/texlive/bin/`uname -m`-linux:$PATH
fi
if [ -d `pwd`/formlib ]; then
  export FORMPATH=`pwd`/formlib
fi

# Print all executed commands to the log.
set -x

case $CI_TARGET in
  form)
    autoreconf -iv
    ./configure --disable-dependency-tracking --enable-scalar --disable-threaded --disable-parform --with-gmp --with-zlib
    make
    ./check/check.rb ./sources/form --stat
    ./check/check.rb ./sources/form --stat -C forcer --timeout 60
    ;;
  tform)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --enable-threaded --disable-parform --with-gmp --with-zlib
    make
    ./check/check.rb ./sources/tform --stat
    ./check/check.rb ./sources/tform --stat -C forcer --timeout 60
    ;;
  parform)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --disable-threaded --enable-parform --with-gmp --with-zlib
    make
    ./check/check.rb ./sources/parform --stat
    ;;
  form-i386)
    # Use Docker (travis-ci/travis-ci#5770).
    docker exec -i -t build_test /bin/sh -c "export CI_TARGET=form && cd $(pwd) && linux32 --32bit i386 ./scripts/travis-script.sh"
    ;;
  tform-i386)
    # Use Docker (travis-ci/travis-ci#5770).
    docker exec -i -t build_test /bin/sh -c "export CI_TARGET=tform && cd $(pwd) && linux32 --32bit i386 ./scripts/travis-script.sh"
    ;;
  sanitize-vorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --enable-scalar --disable-threaded --disable-parform --enable-debug --enable-sanitize --with-gmp --with-zlib
    make -C sources vorm
    ./check/check.rb ./sources/vorm --stat --timeout 60
    ./check/check.rb ./sources/vorm --stat -C forcer Forcer_example --timeout 60
    ;;
  sanitize-tvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --enable-threaded --disable-parform --enable-debug --enable-sanitize --with-gmp --with-zlib
    make -C sources tvorm
    ./check/check.rb ./sources/tvorm --stat --timeout 60
    ./check/check.rb ./sources/tvorm --stat -C forcer Forcer_example --timeout 60
    ;;
  sanitize-parvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --disable-threaded --enable-parform --enable-debug --enable-sanitize --with-gmp --with-zlib
    make -C sources parvorm
    ./check/check.rb ./sources/parvorm --stat --timeout 60
    ;;
  coverage-vorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --enable-scalar --disable-threaded --disable-parform --enable-debug --enable-coverage --with-gmp --with-zlib
    make -C sources vorm
    ./check/check.rb ./sources/vorm --stat --timeout 30
    ./check/check.rb ./sources/vorm --stat -C forcer Forcer_example --timeout 30
    ;;
  coverage-tvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --enable-threaded --disable-parform --enable-debug --enable-coverage --with-gmp --with-zlib
    make -C sources tvorm
    ./check/check.rb ./sources/tvorm --stat --timeout 30
    ./check/check.rb ./sources/tvorm --stat -C forcer Forcer_example --timeout 30
    ;;
  coverage-parvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --disable-threaded --enable-parform --enable-debug --enable-coverage --with-gmp --with-zlib
    make -C sources parvorm
    ./check/check.rb ./sources/parvorm --stat --timeout 30
    ;;
  valgrind-vorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --enable-scalar --disable-threaded --disable-parform --enable-debug --with-gmp --with-zlib
    make -C sources vorm
    ./check/check.rb valgrind ./sources/vorm --stat $TEST
    [ $TEST = 'examples.frm' ] && ./check/check.rb valgrind ./sources/vorm --stat -C forcer Forcer_example
    :
    ;;
  valgrind-tvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --enable-threaded --disable-parform --enable-debug --with-gmp --with-zlib
    make -C sources tvorm
    ./check/check.rb valgrind ./sources/tvorm --stat $TEST
    [ $TEST = 'examples.frm' ] && ./check/check.rb valgrind ./sources/tvorm --stat -C forcer Forcer_example
    :
    ;;
  valgrind-parvorm)
    autoreconf -iv
    ./configure --disable-dependency-tracking --disable-scalar --disable-threaded --enable-parform --enable-debug --with-gmp --with-zlib
    make -C sources parvorm
    ./check/check.rb valgrind ./sources/parvorm --stat $TEST
    ;;
  src-release)
    distname=form-`./scripts/git-version-gen.sh -r | sed '2q;d' | sed 's/^v//'`
    distdir=$distname
    autoreconf -iv
    ./configure --disable-dependency-tracking
    make distdir=$distdir distcheck
    ls -l $distdir.tar.gz && file $distdir.tar.gz
    ;;
  doc-pdf-release)
    distname=form-`./scripts/git-version-gen.sh -r | sed '2q;d' | sed 's/^v//'`
    distname=$distname-manual
    autoreconf -iv
    ./configure --disable-dependency-tracking
    make pdf
    cp doc/manual/manual.pdf $distname.pdf
    ls -l $distname.pdf && file $distname.pdf
    ;;
  doc-html-release)
    distname=form-`./scripts/git-version-gen.sh -r | sed '2q;d' | sed 's/^v//'`
    distdir=$distname-manual-html
    autoreconf -iv
    ./configure --disable-dependency-tracking
    make -C doc/manual latex2html
    (
      cd doc/manual/manual
      rm -f images.aux images.idx images.log images.pl images.tex internals.pl labels.pl WARNINGS
    )
    cp -r doc/manual/manual $distdir
    tar c $distdir/* | gzip -c -9 > $distdir.tar.gz
    ls -l $distdir.tar.gz && file $distdir.tar.gz
    ;;
  bin-release)
    distname=form-`./scripts/git-version-gen.sh -r | sed '2q;d' | sed 's/^v//'`
    distdir=$distname-`uname -m`-$TRAVIS_OS_NAME
    autoreconf -iv
    if [ "x$TRAVIS_OS_NAME" = xosx ]; then
      # --static fails on macOS but we want to statically link to brewed gmp.
      # The linker supports neither -Wl,-static nor -l:libgmp.a.
      # Make a library directory with libgmp.a but without libgmp.dylib.
      mkdir static-lib
      ln -s /usr/local/opt/gmp/lib/libgmp.a static-lib/libgmp.a
      export LIBRARY_PATH="`pwd`/static-lib:${LIBRARY_PATH:-}"
      ./configure --disable-dependency-tracking --disable-native  --enable-scalar --enable-threaded
    else
      ./configure --disable-dependency-tracking --enable-static-link --disable-native --enable-scalar --enable-threaded
    fi
    make
    make check TEST_OPTS=--stat
    mkdir $distdir
    cp sources/form sources/tform $distdir
    tar c $distdir/* | gzip -c -9 > $distdir.tar.gz
    ls -l $distdir.tar.gz && file $distdir.tar.gz sources/form sources/tform
    if [ "x$TRAVIS_OS_NAME" = xosx ]; then
      otool -L sources/form sources/tform
      # Check if gmp is statically linked.
      if otool -L sources/form sources/tform | grep -q gmp; then
        echo 'Error: failed to statically link to gmp' >&2
        exit 1
      fi
    fi
    ;;
  *)
    echo "Error: unknown CI_TARGET=$CI_TARGET" >&2
    exit 1
    ;;
esac
