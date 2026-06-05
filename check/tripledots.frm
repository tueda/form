#ifndef `TEST'
  #message Use -D TEST=XXX
  #terminate
#else
  #include `NAME_' # `TEST'
#endif
.end

*--#[ triple-dot-operator-1 :
#write "f(a1+...+a5)"
#write "f(b1-...-b5)"
#write "f(c1+...-c5)"
#write "f(d1-...+d5)"
#write "f(e1*...*e5)"
#write "f(f1/.../f5)"
#write "f(g1,...,g5)"
#write "f(h1:...:h5)"
#write "f(i1^...^i5)"
#write "f(j1#...#j5)"
.end
assert succeeded?
assert stdout =~ exact_pattern("f(a1+a2+a3+a4+a5)")
assert stdout =~ exact_pattern("f(b1-b2-b3-b4-b5)")
assert stdout =~ exact_pattern("f(c1+c2-c3+c4-c5)")
assert stdout =~ exact_pattern("f(d1-d2+d3-d4+d5)")
assert stdout =~ exact_pattern("f(e1*e2*e3*e4*e5)")
assert stdout =~ exact_pattern("f(f1/f2/f3/f4/f5)")
assert stdout =~ exact_pattern("f(g1,g2,g3,g4,g5)")
assert stdout =~ exact_pattern("f(h1.h2.h3.h4.h5)")
assert stdout !~ exact_pattern("f(i1^i2^i3^i4^i5)")
assert stdout !~ exact_pattern("f(j1#j2#j3#j4#j5)")
*--#] triple-dot-operator-1 :
*--#[ triple-dot-operator-e1 :
P "f(a1+...,a5)";
.end
assert compile_error?("Improper operators for ...")
*--#] triple-dot-operator-e1 :
*--#[ triple-dot-operator-e2 :
S a,...,a5;
.end
assert compile_error?("No first number in ... operator")
*--#] triple-dot-operator-e2 :
*--#[ triple-dot-operator-e3 :
S a1,...,a;
.end
assert compile_error?("No second number in ... operator")
*--#] triple-dot-operator-e3 :
*--#[ triple-dot-operator-e4 :
CF f;
S a1,a2,a3,a4,a5;
id f(a1?,...,a5) = 0;
.end
assert compile_error?("Improper use of ? in ... operator")
*--#] triple-dot-operator-e4 :
