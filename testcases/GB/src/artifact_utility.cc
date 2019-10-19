#include "artifact_utility.h"
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define T +3600

int get_timeout(int argc, char ** argv) {
  int timeout;
  if (argc <= 1)
    return T*10;
  else {
    try {
      timeout = std::stoi(argv[1]);
    } catch (...) {
      timeout=T*10;
    }
  }
  return timeout;
}

void set_result(const std::string & outDir, bool succeeded, double total_sec, int cegar_iter, double syn_time, double eq_time) {
  std::ofstream fout (outDir + "result-stat.txt");
  std::cerr << "CWD:" << get_current_dir_name() << std::endl;
  std::cerr << "Opening " << outDir + "result-stat.txt" << " for write" << std::endl;
  if (!fout.is_open()) {
    std::cerr << "Error open " << outDir + "result-stat.txt" << " for write" << std::endl;
    std::cerr << "Error: " << strerror(errno);
    return;
  }
  if (succeeded)
    fout << "DONE\n";
  else
    fout << "KILLED\n";
  fout << total_sec << std::endl;
  fout << cegar_iter<< std::endl; // iter
  fout << syn_time << std::endl; // syn
  fout << eq_time << std::endl; // eqcheck
  fout.flush();
}

/*volatile*//*sig_atomic_t*/ int * glb_cegar_iter = NULL;
/*volatile*//*sig_atomic_t*/ double * glb_syn_time = NULL;
/*volatile*//*sig_atomic_t*/ double * glb_eq_time = NULL;
/*volatile*//*sig_atomic_t*/ std::string glb_outDir;
std::string init_cwd;

long long initial_timeout = 0;
long long initial_timestamp = 0;
 
void handle_alarm( int sig ) {
  int cegar_iter    = glb_cegar_iter ? * glb_cegar_iter : 0;
  double syn_time   = glb_syn_time ? * glb_syn_time : 0;
  double eq_time    = glb_eq_time ? * glb_eq_time : 0;
  chdir(init_cwd.c_str());
  set_result(glb_outDir, false, syn_time + eq_time, cegar_iter, syn_time, eq_time);

  kill(-getpid(), SIGTERM);
  exit(0);
}

void set_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time) {
  std::cout << "set time (total) = " << sec << std::endl;
  glb_cegar_iter = cegar_iter;
  glb_syn_time   = syn_time;
  glb_eq_time    = eq_time;
  glb_outDir     = outDir;
  init_cwd = get_current_dir_name();
  //std::cout << "pid:" << getpid()<<std::end;
  if (sec < 0) {
    handle_alarm(0);
    return ;    
  }

  signal( SIGALRM, handle_alarm );
  initial_timeout = sec;
  initial_timestamp = (long long ) time(NULL);
  alarm (sec);
}



void handle_alarm_verif( int sig ) {
  int cegar_iter    = glb_cegar_iter ? * glb_cegar_iter : 0;
  double syn_time   = glb_syn_time ? * glb_syn_time : 0;
  double eq_time    = glb_eq_time ? * glb_eq_time : 0;
  chdir(init_cwd.c_str());
  set_result(glb_outDir, true, syn_time + eq_time, cegar_iter, syn_time, eq_time);

  kill(-getpid(), SIGTERM);
  exit(0);
}


void set_verify_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time) {
  std::cout << "set time (verify) = " << sec << std::endl;
  glb_cegar_iter = cegar_iter;
  glb_syn_time   = syn_time;
  glb_eq_time    = eq_time;
  glb_outDir     = outDir;
  init_cwd = get_current_dir_name();
  //std::cout << "pid:" << getpid()<<std::end;

  signal( SIGALRM, handle_alarm_verif );
  alarm (sec);
}

void cancel_verify_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time) {
  long long current_timestamp = time(NULL);
  long long next_timeoutvalue = current_timestamp - initial_timestamp;
  set_timeout(initial_timeout - next_timeoutvalue, outDir, cegar_iter, syn_time, eq_time);
}


std::set<std::string> Split2Set(const std::string& str,
                               const std::string& delim) {
  std::set<std::string> tokens;
  size_t prev = 0, pos = 0;
  do {
    pos = str.find(delim, prev);
    if (pos == std::string::npos)
      pos = str.length();
    std::string token = str.substr(prev, pos - prev);
    if (!token.empty())
      tokens.insert(token);
    prev = pos + delim.length();
  } while (pos < str.length() && prev < str.length());
  return tokens;
}



/// Replace all occurrance of substring a by substring b
std::string ReplaceAll(const std::string& str, const std::string& a,
                       const std::string& b) {
  std::string result;
  size_t find_len = a.size();
  size_t pos, from = 0;
  while (std::string::npos != (pos = str.find(a, from))) {
    result.append(str, from, pos - from);
    result.append(b);
    from = pos + find_len;
  }
  result.append(str, from, std::string::npos);
  return result;
}


void get_grm_stat(const char * grm_fname, int & ncs, int & ncio, int & ndsrc, int & nddst, int & nvargrp) {
    std::set<std::string> CSvar, COvar, DIvar, DOvar;
    std::string CSnames, CInames, COnames, DInames, DOnames;
    std::string Group;
    std::vector<std::set<std::string>> Groupings;
    std::string buf;
    std::ifstream gf(grm_fname);
    while (gf.good())
    {
      getline(gf, buf);
      if (buf.substr(0, 12) == "CTRL-STATE: ") CSnames += buf.substr(12);
      else if (buf.substr(0, 9) == "CTRL-IN: ") CInames += buf.substr(9);
      else if (buf.substr(0, 10) == "CTRL-OUT: ") COnames += buf.substr(10);
      else if (buf.substr(0, 9) == "CTRL-IO: ") COnames += buf.substr(9);
      else if (buf.substr(0, 9) == "DATA-IN: ") DInames += buf.substr(9);
      else if (buf.substr(0, 10) == "DATA-SRC: ") DInames += buf.substr(10);
      else if (buf.substr(0, 10) == "DATA-OUT: ") DOnames += buf.substr(10);
      else if (buf.substr(0, 10) == "DATA-DST: ") DOnames += buf.substr(10);
      else if (buf.substr(0, 11) == "VAR-GROUP: ") Groupings.push_back( Split2Set(ReplaceAll(buf.substr(11)," ", ""), ",") );
    }
    CSvar = Split2Set(ReplaceAll(CSnames," ", ""), ",");
    COvar = Split2Set(ReplaceAll(COnames," ", ""), ",");
    DIvar = Split2Set(ReplaceAll(DInames," ", ""), ",");
    DOvar = Split2Set(ReplaceAll(DOnames," ", ""), ",");
    ncs = CSvar.size();
    ncio= COvar.size();
    ndsrc=DIvar.size();
    nddst=DOvar.size();
    nvargrp = Groupings.size();
}



int retrieveColonEol(const std::string & msg, const std::string & label) {
  size_t pos_1, endl_1;
  pos_1 = msg.find(label);
  endl_1 = msg.find('\n', pos_1);
  return std::stoi(msg.substr(pos_1 + label.length(),endl_1)) T;
}

int loadCandFromFile(const std::string & f) {
  std::stringstream sbuf;
  std::ifstream fin(f);
  sbuf << fin.rdbuf();
  return retrieveColonEol (sbuf.str(), "TotalCand:") ;
}