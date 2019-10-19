#ifndef ARTIFACT_UTILITY_H__
#define ARTIFACT_UTILITY_H__

#include <string>     // std::string, std::stoi

int get_timeout(int argc, char ** argv);
void set_result(const std::string & outDir, bool succeeded, double total_sec, int cegar_iter, double syn_time, double eq_time);
void set_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time);

void set_verify_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time);
void cancel_verify_timeout(int sec,  const std::string & outDir,   int * cegar_iter, double * syn_time, double * eq_time);

void get_grm_stat(const char * grm_fname, int & ncs, int & ncio, int & ndsrc, int & nddst, int & nvargrp);

#endif // ARTIFACT_UTILITY_H__
