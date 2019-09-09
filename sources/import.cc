#ifdef HAVE_CONFIG_H
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
#include <config.h>
#endif
#endif

#include <fstream>
#include <regex>
#include <string>

extern "C" {
#include "form3.h"
}

namespace {

bool FileExists(const std::string& filename) {
  return std::ifstream(filename).is_open();
}

std::string GetHomeDirectory() {
  // TODO: portability
  const char* home = std::getenv("HOME");
  return home;
}

std::string JoinPath() { return ""; }

template <typename First, typename... Rest>
std::string JoinPath(const First& first, const Rest&... rest) {
  std::string s1 = first;
  std::string s2 = JoinPath(rest...);
  if (!s1.empty()) {
    if (s1[s1.length() - 1] == SEPARATOR ||
        s1[s1.length() - 1] == ALTSEPARATOR) {
      s1.erase(s1.length() - 1);
    }
  }
  if (!s2.empty()) {
    if (s2[0] == SEPARATOR || s2[0] == ALTSEPARATOR) {
      s2.erase(0);
    }
  }
  if (!s2.empty()) {
    if (s2[s2.length() - 1] == SEPARATOR ||
        s2[s2.length() - 1] == ALTSEPARATOR) {
      s2.erase(s2.length() - 1);
    }
  }
  return s1 + SEPARATOR + s2;
}

bool is404(const std::string filename) {
  char buf[3];
  std::ifstream file;
  file.open(filename, std::ios::in | std::ios::binary);
  file.read(buf, 3);
  return file && buf[0] == '4' && buf[1] == '0' && buf[2] == '4';
}

int DeployPackage(const std::string path, const std::string url) {
  int err;
  std::string cmd;

  std::string pid = (const char*)GetPreVar((UBYTE*)"PID_", WITHERROR);

  std::string tmp_dir = "formpkg_" + pid;

  cmd = "mkdir -p " + tmp_dir;
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  std::string tmp_name = std::string(url);
  std::replace(tmp_name.begin(), tmp_name.end(), SEPARATOR, '_');
  std::replace(tmp_name.begin(), tmp_name.end(), ALTSEPARATOR, '_');
  std::replace(tmp_name.begin(), tmp_name.end(), ':', '_');
  std::replace(tmp_name.begin(), tmp_name.end(), '@', '_');
  std::replace(tmp_name.begin(), tmp_name.end(), ' ', '_');
  tmp_name = JoinPath(tmp_dir, tmp_name);

  cmd = "curl -L -o " + tmp_name + " " + url;
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  if (is404(tmp_name)) {
    cmd = "cat " + tmp_name;
    DoSystem((UBYTE*)cmd.c_str());
    return -1;
  }

  cmd = "cd " + tmp_dir + " && tar xfz *.tar.gz";
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  cmd = "mkdir -p $(dirname " + path + ")";
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  cmd = "mv $(ls -d " + tmp_dir + "/*/) " + path;
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  cmd = "touch " + JoinPath(path, ".complete");
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  cmd = "rm -rf " + tmp_dir;
  err = DoSystem((UBYTE*)cmd.c_str());
  if (err) return err;

  return 0;
}

}  // unnamed namespace

extern "C" int DoImport(UBYTE* s) {
  int err;

  if (AP.PreSwitchModes[AP.PreSwitchLevel] != EXECUTINGPRESWITCH) return (0);
  if (AP.PreIfStack[AP.PreIfLevel] != EXECUTINGIF) return (0);

  std::string args{(const char*)s};
  std::regex syntax{R"(\s*([^/@\s]+)/([^/@\s]+)@([^/@\s]+)\s*)"};
  std::smatch match;

  if (!std::regex_match(args, match, syntax)) {
    MesPrint("@Improper syntax for %#import");
    return -1;
  }

  std::string owner = match[1].str();
  std::string package = match[2].str();
  std::string revision = match[3].str();

  std::string package_path =
      JoinPath(GetHomeDirectory(), ".form", owner, package, revision);

  std::string complete_file = JoinPath(package_path, ".complete");

  if (!FileExists(complete_file)) {
    MesPrint("@Download package %s",
             (owner + "/" + package + "@" + revision).c_str());
    std::string package_url = "https://github.com/" + owner + "/" + package +
                              "/archive/" + revision + ".tar.gz";

    err = DeployPackage(package_path, package_url);
    if (err) return err;
  }

  std::string include_path = "\"" + package_path + "\"";
  err = DoPreAppendPath((UBYTE*)include_path.c_str());
  if (err) return err;

  std::string header_file = package + ".h";
  err = DoInclude((UBYTE*)header_file.c_str());
  if (err) return err;

  return 0;
}
