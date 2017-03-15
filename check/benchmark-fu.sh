#!/bin/sh
# This is intended to be run from "make check".

if [ "x$NCPUS" = "x" ]; then
  NCPUS=4
fi

status=0
for form in $TEST_BINS; do
  case "$form" in
    *vorm*)
      ;;
    *tform*)
      "$form" -w$NCPUS -v | head -1
      "$form" -w$NCPUS -q -D QUIET "$srcdir/aap/fu.frm" || status=1
      ;;
    *parform*)
      # mpirun may need the absolute path.
      abs_form=`command -v "$form"`
      mpirun -np $NCPUS "$abs_form" -v | head -1
      mpirun -np $NCPUS "$abs_form" -q -D QUIET "$srcdir/aap/fu.frm" || status=1
      ;;
    *)
      "$form" -v | head -1
      "$form" -q -D QUIET "$srcdir/aap/fu.frm" || status=1
      ;;
  esac
done
exit $status
