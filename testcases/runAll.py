import os
import shutil
import subprocess
import argparse
import signal
import time
import datetime
from customized_timeout import TimeoutException,TimeoutError

proc_to_kill = {"PdrChc":"z3", "Cvc4Sy": "cvc4", "PdrAbc":"abc", "RelChc":"z3", "Grain":"bv"}

TestSucc = \
  [("RC", [("RCrelchc","RelChc"),  ("RCpdrabc","PdrAbc"),   ("RCpdrchc","PdrChc"),   ("RCcvc4sy","Cvc4Sy"),   ("RCgrain","Grain")]),
  ("SP",  [("SPrelchc","RelChc"),  ("SPpdrabc","PdrAbc"),   ("SPpdrchc","PdrChc"),   ("SPgrain","Grain")]),
  ("AES", [("AESgrain","Grain")]),
  ("Pico",[("PICOgrain","Grain")]),
  ("GB",  [("GBpdrchc","PdrChc"),   ("GBcvc4sy","Cvc4Sy"),   ("GBgrain","Grain")])]
  
TestAll = \
  [("RC", [("RCrelchc","RelChc"),  ("RCpdrabc","PdrAbc"),   ("RCpdrchc","PdrChc"),   ("RCcvc4sy","Cvc4Sy"),   ("RCgrain","Grain")]),
  ("SP",  [("SPrelchc","RelChc"),  ("SPpdrabc","PdrAbc"),   ("SPpdrchc","PdrChc"),   ("SPcvc4sy","Cvc4Sy"),   ("SPgrain","Grain")]),
  ("AES", [("AESrelchc","RelChc"), ("AESpdrabc","PdrAbc"),  ("AESpdrchc","PdrChc"),  ("AEScvc4sy","Cvc4Sy"),  ("AESgrain","Grain")]),
  ("Pico",[("PICOrelchc","RelChc"),("PICOpdrabc","PdrAbc"), ("PICOpdrchc","PdrChc"), ("PICOcvc4sy","Cvc4Sy"), ("PICOgrain","Grain")]),
  ("GB",  [("GBrelchc","RelChc"),  ("GBpdrabc","PdrAbc"),   ("GBpdrchc","PdrChc"),   ("GBcvc4sy","Cvc4Sy"),   ("GBgrain","Grain")])]
  
def CountRuns(tests):
  ret_len = 0
  for d,l in tests:
    ret_len += len(l)
  return ret_len
  
  

def getNumbers(fin):
  res = fin.readline()
  total_time = float(fin.readline())
  cegar = int(fin.readline())
  syn_time = float(fin.readline())
  eq_time = float(fin.readline())
  return res, cegar, syn_time, eq_time, total_time
  

def ClearVerifOutput(tests):
  cwd = os.getcwd()
  for directory, test_list in tests:
    if directory == 'GB':
      continue
    base_dir = os.path.join(cwd, directory)
    for prg,outDir in test_list:
      result_dir = os.path.join(base_dir,"verification/"+outDir)
      if os.path.exists(result_dir):
        shutil.rmtree(result_dir)
      os.mkdir(result_dir)

def RunTests(tests, timeout, total):
  cwd = os.getcwd()
  idx = 1
  for directory, test_list in tests:
    base_dir = os.path.join(cwd, directory)
    test_prg_dir = os.path.join(base_dir,"build")
    #print test_list
    for prg,outDir in test_list:
      # clear result
      test_result_file = os.path.join(base_dir,"verification/"+outDir+'/result-stat.txt')
      if os.path.exists(test_result_file):
          os.remove(test_result_file)
      full_prg = os.path.join(test_prg_dir, prg)
      
      print '--------------------------'
      print '|        Job: (%3d/%3d)  |' % (idx, total)
      print '--------------------------'
      print 'Start time:', datetime.datetime.now()
      print 'Run:', full_prg
      print 'Design:',directory
      idx += 1
      if not os.path.exists( full_prg ):
        print 'skipped'
        continue

      os.chdir(test_prg_dir)
      #result_log = open('running_result.log','w')
      process = subprocess.Popen(['./' + prg,str(timeout)], stdout = subprocess.PIPE, stderr = subprocess.PIPE)
      print 'PID:', process.pid
      print 'Method:',outDir
      proc_name = proc_to_kill[outDir] if outDir in proc_to_kill else ''
      #print (proc_name)
      #os.setpgid(process.pid, process.pid)
      #process.getpgid(process.pid)
      pythonkilled = False
      try:
        with TimeoutException(int(timeout)+5):
            process.communicate()
      except KeyboardInterrupt:
        print 'Try killing subprocess...',
        pythonkilled = True
        try:
          process.terminate()
          print 'Done'
        except OSError:
          print 'Unable to kill'
        process.wait()  
      except TimeoutError:
        pythonkilled = True
        print 'Try killing subprocess...',
        try:
          process.terminate()
          print 'Done'
        except OSError:
          print 'Unable to kill'
        process.wait()   
        
      if len(proc_name)>0:
          pkill_result = os.system('pkill -n '+proc_name) 
          if pkill_result == 256:
            time.sleep(1)
            pkill_result = os.system('pkill -n '+proc_name) 
          
      if pythonkilled:  
        print '--------------------------'
        print '|       Result           |'
        print '--------------------------'
        print 'Status :   ','KILLED'
        print '--------------------------'
          
      elif os.path.exists(test_result_file):
        with open(test_result_file) as fin:
          res, cegar_iter, syn_time, eq_time,total_time = getNumbers(fin)
          if outDir == 'RelChc':
            print 
            print '--------------------------'
            print '|       Result           |'
            print '--------------------------'
            print 'Status :   ',res
            if 'KILLED' not in res:
              print 't(total)  =',total_time
            print '--------------------------'
            print
            
          else:
            print 
            print '--------------------------'
            print '|       Result           |'
            print '--------------------------'
            print 'Status :    ',res
            if 'KILLED' not in res:
              print '#(iter)    =',cegar_iter
              print 't(syn)     =',syn_time
              print 't(eq)      =',eq_time
              print 't(syn+eq)  =',syn_time+eq_time
            print '--------------------------'
            print
      else:
        print '--------------------------'
        print '|       Result           |'
        print '--------------------------'
        print 'skipped'
        print '--------------------------'
        print
      # if exists
      # execute with timeout

parser = argparse.ArgumentParser(description='Run Experiments on Redundant Counters (RC)')
parser.add_argument('-t','--timeout',
                    default=2*60*60,
                    help='The time limit in seconds')

parser.add_argument('-a','--all', action='store_true',
                    default=False,
                    help='Run all the tests (default: only run those that can succeed)')                    
                    
args = parser.parse_args()
testset = TestAll if args.all else TestSucc

print '--------------------------'
print '|        Jobs            |'
print '--------------------------'
print 'Will launch (%d) jobs ' % CountRuns(testset)
print 'Time-out limit (sec): ',args.timeout
print '--------------------------'
print 

ClearVerifOutput(testset)
RunTests(testset, args.timeout, CountRuns(testset))
#ClearVerifOutput(TestsAll)
    

