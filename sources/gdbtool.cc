extern "C" {
#include "form3.h"
}

#include <sstream>

static char *statusexpr[] = {
  "LOCALEXPRESSION",
  "SKIPLEXPRESSION",
  "DROPLEXPRESSION",
  "DROPPEDEXPRESSION",
  "GLOBALEXPRESSION",
  "SKIPGEXPRESSION",
  "DROPGEXPRESSION",
  "UNKNOWN",
  "STOREDEXPRESSION",
  "HIDDENLEXPRESSION",
  "HIDELEXPRESSION",
  "DROPHLEXPRESSION",
  "UNHIDELEXPRESSION",
  "HIDDENGEXPRESSION",
  "HIDEGEXPRESSION",
  "DROPHGEXPRESSION",
  "UNHIDEGEXPRESSION",
  "INTOHIDELEXPRESSION",
  "INTOHIDEGEXPRESSION",
};

std::string expr_str(EXPRESSIONS e) {
  std::stringstream ss;
  ss << "[expression " 
  << (const char *)(AC.exprnames->namebuffer + e->name)
  << "(#" << (WORD)(e-Expressions) << "), status: "
  << statusexpr[e->status] << "(#" << e->status << "), hidelevel: " << e->hidelevel << ", buffer: " << e->whichbuffer
  << ", position: " << e->onfile.p1
  << "]";
  return ss.str();
}

std::string expr_str(int i) {
  return expr_str(&Expressions[i]);
}