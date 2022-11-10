#-
* A simple test for the checkpoint mechanism.
*
* This program must be executed 3 times. For example,
*
*   form checkpoint.frm
*   form -R checkpoint.frm
*   form -R checkpoint.frm
*
* and then the last run should end successfully.
*
#ifndef `N'
  #define N "8"
#endif

#ifndef `FILE1'
  #define FILE1 "a1.tmp"
#endif

#ifndef `FILE2'
  #define FILE2 "a2.tmp"
#endif

#ifdef `QUIET'
  Off stats;
#endif

#procedure FailThenSucceed(filename)
  #remove <`filename'>
  .sort:FailThenSucceed `filename';
  #system -e test -f `filename'
  #define error "`SYSTEMERROR_'"
  #write <`filename'> ""
  #close <`filename'>
  #if `error'
    #message Program intentionally fails...
    #terminate
  #endif
  #remove <`filename'>
#endprocedure

#procedure AssertEqual(expr1,expr2)
  #$AssertEqualExpr1 = `expr1';
  #$AssertEqualExpr2 = `expr2';
  #$AssertEqualExpr3 = $AssertEqualExpr1 - $AssertEqualExpr2;
  #$AssertEqualExpr1 = nterms_($AssertEqualExpr3);
  #if `$AssertEqualExpr1' == 0
    #message Assertion OK: `expr1' == `expr2'
  #else
    #message Assertion Failed: `expr1' == `expr2'
    #terminate
  #endif
#endprocedure

On checkpoint;
Auto S x;
L F = (x1+...-x15)^`N';
#$x = 1;
.sort

#call FailThenSucceed(`FILE1')

#$y = 1;
id x2 = x3;
id x4 = x5;
.sort

#call FailThenSucceed(`FILE2')

id x6 = x7;
id x8 = x9;
id x10 = x11;
id x12 = x13;
id x14 = x15;
id x1 = {`$x'+`$y'};
#ifndef `QUIET'
  Print;
#endif
.sort

#call AssertEqual(F,{2^`N'})
.end
