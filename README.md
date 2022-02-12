Installation
==================

1. Please first download the zip archive from the [link](https://github.com/zhanghongce/VMCAI-2020-AE/archive/refs/heads/master.zip) we provided and put it into the [VMCAI2020 Artifact Evaluation Virtual Machine](https://zenodo.org/record/3533104#.YgfW2_sRUS8). (You should have already completed this step)

2. Unzip the archive if you have not done so. You can either (a) double click the archive in the file explorer (namely nautilus) and click "Extract" on the top left corner of the window to unzip or (b) using command line `unzip VMCAI-2020-AE-master.zip`. The unzipping process will automatically create a folder named `VMCAI-2020-AE-master`. Within this folder you can find this README, installation scripts, license, the dependent tools/packages, and the test cases.

3. Start a terminal and change directory into the `VMCAI-2020-AE-master` folder. You can do this by first using the file explorer to navigate into the `VMCAI-2020-AE-master` folder and right clicking in the blank space of the file explorer and select "Open in Terminal".


4. Install dependent Debian packages, in the terminal opened in the previous step, run 

    ```
    bash install-pkgs.sh
    ```
   
   When prompted for password, please type your password ("vmcai2020" for this VM) and press `<Enter>` to grant root permission. 
 
5. Install dependent tools (Yosys/CoSA/Z3/ABC/CVC4): in the terminal used in the previous step, run 

    ```
    bash install-tools.sh
    ```

   If prompted for password, please enter your password to grant root permission.
 
6. Install ILAng framework: in the terminal just used in the previous step, run 

    ```
    bash build-install-ilang.sh
    ```

   There might be some warnings in the compilation process, it is okay to ignore these
   warningn messages. If prompted for password, please enter your password to grant root permission.

7. Build the benchmarks: in the terminal used in the previous step, run 

    ```
    bash build-benchmarks.sh
    ```

8. Test whether the installation is successful: in the terminal used in the previous step, run 

    ```
    bash test-install.sh
    ```
    
   This will test the installation of Z3, CoSA, Yosys, CVC4, and ABC. The test for CoSA might
   take slightly longer time compared to other tests, this is normal as the initial run of
   CoSA will invoke Cython to compile itself into binary. If all the tests print "Okay" then the tools 
   have been installed successfully.


Structure of the Artifact
=====================

  * `packages`, `deptools` , and `ILA-Tools.zip` are for the dependent packages, tools and framework.
  * the `testcase` folder contains five examples as mentioned in the paper
    - `RC` : redundant counters
    - `SP` : a simple processing pipeline
    - `AES` : the AES block encryption accelerator
    - `Pico` : PicoRV32 processor
    - `GB` : the Gaussian-blur image processing accelerator
    - `configs` : CMake configuration for the project
    - `test` : a self-test utility to check if installation is good

  In each folders of the five examples, the files are orginized in the following way:

  * `cmake` : for CMake scripts
  * `include` : contains headers for time-counting utilities and headers for the ILA models of each design
  * `src` : source for time-counting utilities and the ILA model
  * `rfmap` or `refinement` : the refinement map, in JSON format
  * `verilog` : the Verilog implementation of the designs
  * `grm` : the state variables in the four categories, used by Grain's grammar
  * `app` : the functional equivalence checking and invariant synthesis procedure

  In each of the `app` subfolders, there are five source files in the following format:
  ```
  main-xxxx.cc
  ```
  Where `xxxx` stands for the five methods we evaluate: `pdrchc`, `pdrabc`, `relchc`, `cvc4sy` and `grain`.



   
Reproduce Experiment Results
====================================


Brief Introduction on the Workflow
------------------------------------------------------
Each experiment below includes several runs, each with a different
invariant synthesis methods, or on a different design.
The target in each run is to prove that an instruction-level
abstraction (ILA) is functional equivalent to a register-transfer-level
(RTL) description. This includes (a) functional equivalent checking
and (b) environment invariant synthesis. The two are separate
procedures for PdrChc, PdrAbc, Cvc4sy and Grain, but for RelChc, it uses
Z3 to solve the two problems together. A more detailed description of workflow 
can be found in the paper.


Experiment Environment
------------------------------------------------------
The experiments were originally conducted on Ubuntu 18.04 on
Dell XPS 9570 laptop with i5-8300H CPU (4-core 8-thread, up to 4.0GHz),
32GB memory and 1TB PCIe SSD. When running in the artifact evalution 
virtual machine, the outcome could differ in the following ways:

  1. As the virtual machine is equipped with a smaller RAM size (8GB),
     some experiments, which are for comparison purpose, may hit memory limit first
     and terminate due to out-of-memory error instead of time-out.

  2. To avoid the long running time, we set a smaller time-out limit (2 hours in maximum for 
     each run). We originally use 10 hours as the time-limit.
  
  3. The functional equivalence checking procedure may not always generate the same model 
     (counterexample) on the same failing property and therefore the number of CEGAR iterations 
     could be slightly different.

  4. Instruction-level functional equivalence checking procedure of Gaussin-blur accelerator uses 
     Cadence JasperGold. Due to license issue, Cadence JasperGold could not be packed into this artifact. 
     Instead, we save the invariant synthesis problems in each CEGAR iteration and only reproduce the
     invariant synthesis results for Gaussin-blur accelerator in this artifact.

The above changes shall not affact the overall results that Grain with its SyGuS-based method complements
existing PDR-based and SyGuS-based method in the environment invariant synthesis problem for modular
hardware verification.


Experiment 1: Grain on All Testcases
------------------------------------------------------
This experiment is to show that Grain is able to finish five
test cases with a time-out of 7200 seconds per run.

If you are in the `VMCAI-2020-AE-master` folder, type
```
cd testcases
```
to change directory to `VMCAI-2020-AE-master/testcases`, and
then run
```
python runGrain.py
```

The first run (RC) will finish relatively quickly
(in less than a second), the synthesized invariants can be found in 
`VMCAI-2020-AE-master/testcases/RC/verifaction/Grain/inv.txt`, which
shows that the state variable `m1.imp` is 1's-complement of another 
state variable `m1.v`.

The second run (SP) will finish in around several minutes, followed
by AES, Pico, and GB. For the three practical design cases, due to 
their large sizes, the running time is about two hours in total.

When finished successfully, the number of CEGAR iteration, synthesis time
and functional equivalence checking time of one run will be reported. 
(For RelChc method, it does not use CEGAR, therefore only the total time
will be reported). If the solver hits time limit or memory limit, it will
be killed and its status will be reported as "KILLED".


Experiment 2:  All Five Methods on RC and SP
------------------------------------------------------
This experiment runs all five methods on the first two synthetic
designs (RC and SP). Each run should finish within a couple of minutes,
except for Cvc4Sy and RelChc on SP, by default we set a small time
limit (10 mins) for fast evaluation. To use the default time-out,
in the `VMCAI-2020-AE-master/testcases` folder, run:

```
python runRC.py
python runSP.py
```

Within the 10-min time limit, PdrAbc, PdrChc and Grain will be able to finish, 
the other two (RelChc and Cvc4Sy) will time out. If you increase time-limit to 
1800 seconds, only Cvc4Sy will time-out.

If you would like to use a customized time-limit, run:
```
python runRC.py -t <time-limit-in-seconds>
python runSP.py -t <time-limit-in-seconds>
```


Experiment 3: Comparative Experiments on AES, Pico, and GB
------------------------------------------------------
This set of experiments run the four methods (RelChc, PdrChc, PdrAbc, Cvc4Sy)
on three practical designs (AES, PicoRV32 and GB) for comparison purpose. 
Only two runs (PdrChc and Cvc4Sy on GB) can complete
succesfully within the time (2 hours) and memory limit (8GB in total). 
By default, the script only launches the two successful runs.
In the `VMCAI-2020-AE-master/testcases` folder, you can run with:

```
python runAES-Pico-GB.py 
```
This will take around an hour to finish.


If you would like to run all methods (whether succesful or not) until 
they hit the time-out limit (this could take more than 20 hours), in the 
`VMCAI-2020-AE-master/testcases` folder,

```
python runAES-Pico-GB.py -a
```

By default, linux's out-of-memory (OOM) killer should be able to find the 
right process to kill when a test run occupies too much memory. If you want
to manually set a limit, you can use command `ulimit -Sv <size>`, where 
`<size>` has a unit of 1KB.



The Overall Result
------------------------------------------------------
The above experiments show that Grain could finish all five examples within 
2 hours. On the other hand, the other methods, under the same time and memory
limit, mostly fail on the three practical designs (AES, Pico and GB).



ILA and ILAng Documentation
====================================

In this section, we'd like to point to more ILA documentations (available online),
if you would like to have a further understanding of modeling and function equivalencec 
checking utilities in ILAng. 

   1. Introduction for ILAng: 
      * https://bo-yuan-huang.gitbook.io/ilang/
   2. ILAng API references:
      * https://bo-yuan-huang.github.io/ILAng-Doc/doxygen-output-html/namespaceilang.html


