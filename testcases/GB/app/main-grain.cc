#include "artifact_utility.h"
#include <env.h>

#include <ilang/util/fs.h>
#include <ilang/util/str_util.h>
#include <ilang/smt-inout/yosys_smt_parser.h>
#include <ilang/vtarget-out/vtarget_gen.h>
#include <ilang/vtarget-out/inv-syn/inv_syn_cegar.h>

using namespace ilang;

int main (int argc, char ** argv) {

  int timeout = get_timeout(argc, argv);

  int total_cegar = 3;

  int n_cegar = 0;
  double t_eq = 0;
  double t_syn= 0;
  double t_total = 0;
  int n_cand = 0;
  bool succeed = true;

  std::string cwd = os_portable_getcwd();
  std::string verif_path = os_portable_append_dir(cwd, "../verification/Grain/");
  set_timeout(timeout, verif_path, &n_cegar, &t_syn, & t_eq);

  for (; n_cegar < total_cegar;  n_cegar ++) {
    if (!os_portable_chdir(verif_path) )
      std::cerr << "Failed to switch to " << verif_path << std::endl;
    std::string sub_path = verif_path + std::to_string(n_cegar);
    if (!os_portable_chdir(sub_path) )
      std::cerr << "Failed to switch to " << sub_path << std::endl;
    std::string execpath = os_portable_append_dir(std::string (FREQHORNPath), "bv");
    std::cerr << "At path:" << os_portable_getcwd() << std::endl;
    auto res = os_portable_execute_shell({execpath, 
      "--ante-size","0","--conseq-size","0","--cw","32","--skip-cnf","--skip-stat-collect","--skip-const-check","--find-one-clause","--cnf","inv_grm","wrapper.smt2"
    }, "../grain_result.txt", redirect_t::BOTH, 0);
    t_syn += res.seconds;
    t_total = t_eq + t_syn;
    n_cand += loadCandFromFile("../grain_result.txt");
  }

  set_result(verif_path, succeed,  t_syn + t_eq , n_cegar , t_syn , t_eq);

  { 
    os_portable_chdir(cwd);
    int NumOfDesignStateBits = 0, NumOfDesignStateVars = 0;
    { // load smt check
      std::string fn = os_portable_append_dir(cwd , "../smt/__design_smt.smt2");
      std::ifstream fin(fn);
      if (not fin.is_open()) {
        std::cerr << "Unable to read from : " << fn;
      } else{
        std::stringstream sbuf;
        sbuf << fin.rdbuf();
        smt::YosysSmtParser design_smt_info(sbuf.str());
        for (auto && st : design_smt_info.get_module_flatten_dt("wrapper")) {
          if ( st.verilog_name.find("hls_U.") == 0 ) {
            NumOfDesignStateBits += st._type.GetBoolBvWidth();
            NumOfDesignStateVars ++;
          } 
        }
      }
    } // load smt check
    std::ofstream fout(verif_path+"stat.txt");
    int ncs, ncio, ndsrc, nddst, nvargrp;
    get_grm_stat("../grm/gb.gmr", ncs, ncio, ndsrc, nddst, nvargrp);
    fout <<"State bits: " << NumOfDesignStateBits << std::endl;
    fout <<"State vars: " << NumOfDesignStateVars << std::endl;
    fout <<"#(Ctrl-state): " << ncs << std::endl;
    fout <<"#(Ctrl-inout): " << ncio << std::endl;
    fout <<"#(Data-src): " << ndsrc << std::endl;
    fout <<"#(Data-dst): " << nddst << std::endl;
    fout <<"#(Var-grp): " << nvargrp << std::endl;
    fout <<"#(cand): " << n_cand << std::endl;
  }

  return 0;
}
