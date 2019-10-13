Installation
==================

1. Please first download the zip archive from the link we provided. (You should have completed this step)

2. Unzip the archive. You can either (a) double click the archive in the file explorer (namely nautilus) and click "Extract" on the top left corner of the window to unzip or (b) using command line `unzip master.zip`. The unzipping process will automatically create a folder named `VMCAI-2020-AE-master`, within which there are the directory of the artifact: `ILAng`, the license and this readme.

3. Open a terminal and change directory into the `VMCAI-2020-AE-master` folder. You can do this by first using the file explorer to navigate into the `VMCAI-2020-AE-master` folder and right clicking in the blank space of the file explorer and select "Open in Terminal".


4. Install dependent Debian packages, in the terminal opened in the previous step, run 

    ```
    bash install-pkgs.sh
    ```
   
   When prompted for password, please type your password ("vmcai2020" for this VM) and press `<Enter>` to grant root permission. 
 
5. Install dependent tools (Yosys/CoSA/Z3/ABC/CVC4): in the terminal used in the previous step, run 

    ```
    bash install-tools.sh
    ```

   When prompted for password, please type your password ("vmcai2020" for this VM) and press `<Enter>` to grant root permission.
 
6. Install ILAng framework: in the terminal just used in the previous step, run 

    ```
    bash build-install-ilang.sh
    ```

   When prompted for password, please type your password ("vmcai2020" for this VM) and press `<Enter>` to grant root permission.

7. Build the benchmarks: in the terminal used in the previous step, run 

    ```
    bash build-benchmarks.sh
    ```

8. Build the benchmarks: in the terminal used in the previous step, run 

    ```
    bash build-benchmarks.sh
    ```

9. Test whether the installation is successful: in the terminal used in the previous step, run 

    ```
    bash test-install.sh
    ```
    
   This will test the installation of Z3, CoSA, Yosys, CVC4, and ABC.
   If the tests print "Okay" then the corresponding tool has been installed successfully.


Structure of the Artifact
=====================

  * `packages`, `deptools` , and `ILA-Tool` subfolders are for the dependent packages, tools and framework.
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
  * `grm` : the grammar used by Grain
  * `app` : the functional equivalence checking and invariant synthesis procedure

  In the `app` subfolder, there are five source files in the following format:
  ```
  main-xxxx.cc
  ```
  Where `xxxx` stands for the five methods we evaluate: `pdrchc`, `pdrabc`, `relchc`, `cvc4sy` and `grain`.



   
Reproduce Experiment Results
====================================

Experiment Environment
------------------------------------------------------
The experiments were originally conducted on Ubuntu 18.04 on
Dell XPS 9570 with i5-8300H CPU, 32GB memory and 1TB PCIe SSD.
When running in the artifact evalution virtual machine, the outcome
could differ in the following ways:

  1. As the virtual machine is equipped with a smaller RAM size (8GB),
     some experiments (for comparison purpose) may hit memory limit first
     and terminate due to out-of-memory rather than time-out.

  2. For faster evaluation, we set a smaller time-out limit (2 hours per run)
     We originally use 10 hours as the time-out limit.
  
  3. We shipped our artifact with a pre-compiled version of Z3 (release version 4.8.5), 
     which seems to differ from the latest Github version (Hash: 224cc8f) in the invariants 
     it generates. This could affect the number of CEGAR needed for method `PdrChc`.

  4. Due to license issue, function equivalence checking part of Gaussin-blur accelerator that
     uses Cadence JasperGold could not be packaged into this artifact. We save the invariant
     synthesis problems in each CEGAR iteration which are used to reproduce the results in invariant
     synthesis. We skip the equivalence checking part of this benchmark.


The above changes shall not affact the overall claim that Grain with its SyGuS-based method complements
existing PDR-based and SyGuS-based method for environment invariant synthesis problem in modular
hardware verification.


Experiment 1: Grain on All Testcases
------------------------------------------------------
This experiment is to show that Grain is able to finish five
test cases with a time-out of 7200 seconds per run.

In the `VMCAI-2020-AE-master/testcases` folder, run
```
python runGrain.py
```

The first run (RC) will finish relatively quickly
(in several seconds), the synthesized invariants can be found in 
`VMCAI-2020-AE-master/testcases/RC/verifaction/Grain/inv.txt`. 

The second run (SP) will finish in around several minutes, followed
by AES, Pico, and GB. For the three practical design cases, due to 
their large sizes, the running time is about two hours in total.

When finished successfully, the number of CEGAR iteration, synthesis time
and functional equivalence time will be reported. (For RelChc method, it does 
not use CEGAR, therefore only the total time will be reported).


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

If you would like to use a customized time limit, run:
```
python -t <time-limit-in-seconds> runRC.py 
python -t <time-limit-in-seconds> runSP.py 
```


Experiment 3: Comparative Experiments on AES, Pico, and GB
------------------------------------------------------
This set of experiments run the four methods (RelChc, PdrChc, PdrAbc, Cvc4Sy)
for comparison purpose. Only two runs (PdrChc and Cvc4Sy on GB can complete
succesfully within the time and memory limit). By default, the script runs only
the two successful method on GB.
In the `VMCAI-2020-AE-master/testcases` folder, run:

```
python runAES-Pico-GB.py 
```
This will take around an hour to finish.


If you would like to run all methods (whether succesful or not) until 
they hit the time-out limit (this could take more than 20 hours), in the 
`VMCAI-2020-AE-master/testcases` folder,

```
python -a runAES-Pico-GB.py 
```


The Overall Result
------------------------------------------------------
The above experiments shows that Grain could finish all five examples within 
2 hours. On the other hand, the other methods, under the same time and memory
limit, fail in most cases on the three practical designs (AES, Pico and GB).



ILA and ILAng Documentation
====================================

In this section, we'd like to point to more ILA documentations (available online),
if you would like to have a further understanding of modeling and function equivalencec 
checking utilities in ILAng. 

   1. Introduction for ILAng: 
      * https://bo-yuan-huang.gitbook.io/ilang/
   2. ILAng API references:
      * https://bo-yuan-huang.github.io/ILAng-Doc/doxygen-output-html/namespaceilang.html


