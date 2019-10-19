#include <rc_ila.h>
#include <env.h>
#include "artifact_utility.h"

#include <ilang/util/log.h>
#include <ilang/util/fs.h>
#include <ilang/vtarget-out/vtarget_gen.h>
#include <ilang/vtarget-out/inv-syn/inv_syn_cegar.h>

using namespace ilang;


int loglevel(int argc, char **argv) {
  for (int idx = 1; idx < argc; ++idx)
    if(std::string(argv[idx]) == "fulllog")
      return 0;
  return 2;
}

int main (int argc, char ** argv) {
  SetLogLevel(loglevel(argc,argv));
  int timeout = get_timeout(argc, argv);

  auto ila_model = BuildModel();


  VerilogVerificationTargetGenerator::vtg_config_t cfg;
  cfg.InvariantSynthesisReachableCheckKeepOldInvariant = false;
  cfg.CosaAddKeep = false;
  cfg.VerificationSettingAvoidIssueStage = true;
  cfg.YosysSmtFlattenDatatype = true; // let's test flatten datatype also
  cfg.YosysSmtFlattenHierarchy = true;
  cfg.YosysUndrivenNetAsInput = true;
  cfg.YosysPath = YOSYSPath;
  cfg.CosaPyEnvironment = COSAEnvPath;
  cfg.CosaPath = COSAPath;
  cfg.Z3Path = "N/A";
  cfg.FreqHornPath = FREQHORNPath;
  cfg.FreqHornHintsUseCnfStyle = true;
  cfg.FreqHornOptions = {
    "--skip-cnf --skip-const-check --skip-stat-collect --ante-size 1 --conseq-size 1  --grammar rc.gmr --use-arith-bvnot --no-const-enum-vars-on m1.v,m1.imp"};
  cfg.CosaSolver = "btor";

  auto dirName = std::string("../");
  auto outDir  = dirName + "verification/Grain/";

  int n_cegar = 0;
  double t_eq = 0;
  double t_syn = 0;
  double t_total = 0;
  bool succeed = true;
  set_timeout(timeout, outDir, &n_cegar, &t_syn, & t_eq);
  
  /*
  InvariantInCnf var_in_cnf;
  { // save grammar file
    os_portable_mkdir(outDir + "inv-syn");
    InvariantInCnf::clause cl;
    InvariantInCnf::VarsToClause( {
      "m1.imp", "m1.v"
      } , cl);
    var_in_cnf.InsertClause(cl);
    std::ofstream fout(outDir + "inv-syn/cnt-no-group.cnf");
    if (fout.is_open())
      var_in_cnf.ExportInCnfFormat(fout);
    else
      succeed = false;
  } */ // save grammar file

  InvariantInCnf var_in_cnf;
  { // save grammar file
    os_portable_mkdir(outDir + "inv-syn");
    os_portable_copy_file_to_dir(dirName+"grm/rc.gmr", outDir + "inv-syn");
  } 

  InvariantSynthesizerCegar vg(
      {},                          // no include
      {dirName + "verilog/opposite.v"}, //
      "opposite",                // top_module_name
      dirName + "refinement/vmap.json", // variable mapping
      dirName + "refinement/cond.json", outDir, ila_model.get(),
      VerilogVerificationTargetGenerator::backend_selector::COSA,
      VerilogVerificationTargetGenerator::synthesis_backend_selector::FreqHorn,
      cfg);


    do{
      vg.GenerateVerificationTarget();
      if(vg.RunVerifAuto("INC")) // the ADD
        break; // no more cex found
      vg.ExtractVerificationResult();
      vg.GenerateSynthesisTarget(); // you will need fp engine
      if(vg.RunSynAuto()) {
        succeed = false;
        break; 
      }
      vg.ExtractSynthesisResult(); // very weired, it throw away something in arg

    auto design_stat = vg.GetDesignStatistics();
    t_eq = design_stat.TimeOfEqCheck;
    t_syn = design_stat.TimeOfInvSyn;
    t_total = design_stat.TimeOfEqCheck + design_stat.TimeOfInvSyn;
    n_cegar ++;

    }while(not vg.in_bad_state());


  vg.GetInvariants().ExportToFile(outDir+"inv.txt",false);
  set_result(outDir, succeed,  t_syn + t_eq , n_cegar , t_syn , t_eq);

  {
    std::ofstream fout(outDir+"stat.txt");
    int ncs, ncio, ndsrc, nddst, nvargrp;
    get_grm_stat((dirName+"grm/rc.gmr").c_str(), ncs, ncio, ndsrc, nddst, nvargrp);
    auto design_stat = vg.GetDesignStatistics();
    fout <<"State bits: " << design_stat.NumOfDesignStateBits << std::endl;
    fout <<"State vars: " << design_stat.NumOfDesignStateVars << std::endl;
    fout <<"#(Ctrl-state): " << ncs << std::endl;
    fout <<"#(Ctrl-inout): " << ncio << std::endl;
    fout <<"#(Data-src): " << ndsrc << std::endl;
    fout <<"#(Data-dst): " << nddst << std::endl;
    fout <<"#(Var-grp): " << nvargrp << std::endl;
    fout <<"#(cand): " << vg.total_freqhorn_cand << std::endl;
  }

  return 0;
}