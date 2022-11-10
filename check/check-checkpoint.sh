#!/bin/sh
# This is intended to be run from "make check".
# NOTE: FORMrecv.tmp will be removed, if exists.
trap 'exit 1' 1 2 13 15
status=0
for form in $TEST_BINS; do
  case $form in
    *parform*|*parvorm*)
      # TODO: how to find an appropriate mpirun?
      ;;
    *tform*|*tvorm*)
      rm -f FORMrecv.tmp "a$$-1.tmp" "a$$-2.tmp"
      "$form" -v | head -1
      "$form" -w4 -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp"    "$srcdir/checkpoint/checkpoint.frm"
      "$form" -w4 -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp" -R "$srcdir/checkpoint/checkpoint.frm"
      "$form" -w4 -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp" -R "$srcdir/checkpoint/checkpoint.frm" || status=1
      rm -f FORMrecv.tmp "a$$-1.tmp" "a$$-2.tmp"
      ;;
    *form*|*vorm*)
      rm -f FORMrecv.tmp a$$-1.tmp a$$-2.tmp
      "$form" -v | head -1
      "$form" -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp"    "$srcdir/checkpoint/checkpoint.frm"
      "$form" -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp" -R "$srcdir/checkpoint/checkpoint.frm"
      "$form" -q -D QUIET -D "FILE1=a$$-1.tmp" -D "FILE2=a$$-2.tmp" -R "$srcdir/checkpoint/checkpoint.frm" || status=1
      rm -f FORMrecv.tmp "a$$-1.tmp" "a$$-2.tmp"
      ;;
  esac
done
exit $status
