[![CC0 1.0 Universal](https://img.shields.io/badge/license-CC0-orange.svg)](https://creativecommons.org/publicdomain/zero/1.0/legalcode)

How to reproduce VEX's results

First, install VEX. See INSTALL file for instructions.


Running VEX on a local machine

(1) To reproduce the experiments in figure 10:

  run the master script in eval with the -t (throughput) option.

    $ cd eval
    $ ./master.rb -t

  the logs will be stored in the "logs" folder. These logs will be processed
  automatically by the analyze.rb script. The results will be stored in 
  the "results" folder. The results are json files that should be straightforward
  to parse.

  if you wish to reduce the number of iterations, the number of bidders, or the
  number of auctions, modify the values in the first few lines of throughput2.rb



(2) To reproduce the experiments in figure 11:

  run the master script in eval with the -d (delay) option.

    $ cd eval
    $ ./master.rb -d

  the logs will be stored in the "logs" folder. Like above, these logs
  will be processed automatically by the analye.rb script. Note that only
  the *_seller_*.log files contain measurement information (all others only
  contain debug information). The results are json files that are stored in
  the "results" folder.

  if you wish to reduce the number of iterations, number of bidders, etc,
  modify the first few lines of delay.rb


  (3) To reproduce the experiments in figure 13:

  run the master script in eval with the -a (audit) option.

    $ cd eval
    $ ./master.rb -a

  The results will be json files in the "results" folder.


Running VEX on Emulab

  (1) Set the correct path to the binaries and the target folders in 
      master.rb (right now it says "emulab-path").


  (2) Create the desired emulab network topology. A sample file is
      included (see 10-bidders.ns). 

  (3) Create a connections.txt file with the following format:

    Seller IP,Port
    Bidder 1 IP,Port
    Bidder 2 IP,Port


  A sample file is found in src/connections.txt

  (4) Follow the instructions in running VEX for local machine, but also pass in the
      -e (emulab) flag. 

  Note that the logs and result folders will be on emulab (not on the local machine).
