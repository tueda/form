* Compares the diagram generator with others.
*
* When run outside the testsuite, this file must be on the include path.
* For example, run:
*   form -D TEST=XXX -I check check/diagrams.frm
*
#ifndef `TEST'
  #message Use -D TEST=XXX
  #terminate
#else
  #include `NAME_' # `TEST'
#endif
.end

*--#[ qgraf_qcd_qua_qua_3_0_none :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=qua,out=qua,loops=3,form_options=,qgraf_options=,options=forcecacheread)
.end
# Too many subexpressions for the 32-bit version.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 1001;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_qua_qua_3_0_none : 
*--#[ qgraf_qcd_glu_glu_3_0_none :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=glu,out=glu,loops=3,form_options=,qgraf_options=,options=forcecacheread)
.end
# Too many subexpressions for the 32-bit version.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 2829;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_glu_glu_3_0_none : 
*--#[ qgraf_qcd_gho_gho_3_0_none :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=gho,out=gho,loops=3,form_options=,qgraf_options=,options=forcecacheread)
.end
# Too many subexpressions for the 32-bit version.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 1001;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_gho_gho_3_0_none : 
*--#[ qgraf_qcd_qua_qua_3_16777472_onepifloop :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=qua,out=qua,loops=3,form_options=ONEPI_,FLOOP_,qgraf_options=onepi,floop,options=forcecacheread)
.end
# The option does not fit into a 16-bit WORD.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 89;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_qua_qua_3_16777472_onepifloop : 
*--#[ qgraf_qcd_glu_glu_3_16777472_onepifloop :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=glu,out=glu,loops=3,form_options=ONEPI_,FLOOP_,qgraf_options=onepi,floop,options=forcecacheread)
.end
# The option does not fit into a 16-bit WORD.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 222;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_glu_glu_3_16777472_onepifloop : 
*--#[ qgraf_qcd_gho_gho_3_16777472_onepifloop :
#include- diagrams/diagrams.h
#call DoComparison(qgraf,qcd,in=gho,out=gho,loops=3,form_options=ONEPI_,FLOOP_,qgraf_options=onepi,floop,options=forcecacheread)
.end
# The option does not fit into a 16-bit WORD.
#require wordsize >= 4
assert succeeded?
assert nterms("F1", 0) == 89;
assert nterms("F1") == 0;
assert nterms("F2") == 0;
*--#] qgraf_qcd_gho_gho_3_16777472_onepifloop : 
