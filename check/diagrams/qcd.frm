* Compares the diagram generator with others.
*
* When run outside the testsuite, this file must be on the include path.
* For example, run:
*   form -D TEST=XXX -I check/diagrams check/diagrams/qcd.frm
*
#ifndef `TEST'
  #message Use -D TEST=XXX
  #terminate
#else
  #include `NAME_' # `TEST'
#endif
.end

*--#[ qgraf_qcd_qua_qua_3_none :
#include- diagrams.h
#call DoComparison(qgraf,qcd,in=qua,out=qua,loops=3,form_options=,qgraf_options=)
.end
assert succeeded?
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_qua_qua_3_none : 
*--#[ qgraf_qcd_qua_qua_4_none :
#include- diagrams.h
#call DoComparison(qgraf,qcd,in=qua,out=qua,loops=4,form_options=,qgraf_options=)
.end
assert succeeded?
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_qua_qua_4_none : 
