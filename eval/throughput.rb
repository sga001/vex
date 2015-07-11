#!/usr/bin/env ruby

require 'rubygems'
require 'json'
require 'fileutils'

def FixedSalePriceExperiment()
  ids = ["vex", "vex-cp", "vex-nocc", "auditor"]
  num_bidders = [10, 20, 50, 100]
  num_trials = 100
  cp = 10
  ip = "127.0.0.1"
  rand_seed = $prng.rand(1..10000000)
  $exp_name = $exp_name + "-vs-bidders"

  num_bidders.each do |bidders| 

    (1..num_trials).each do |trial|
      
      ids.each do |id|

        command = "#{$git_folder}/src/vex/prover" unless id == "auditor"
			  command = "#{$git_folder}/src/vex/auditor" if id == "auditor"
        args = "--simulate 1 --log_out #{$log_folder}/#{id}-#{$exp_name}-#{trial}-#{bidders}.log "
        args << "--benchmark --key_file #{$git_folder}/src/vex/keys/seller.priv " unless id == "auditor"
			  args << "--key_file #{$git_folder}/src/vex/keys/seller.pub " if id == "auditor"
        args << "--file_name #{$git_folder}/eval/audit_files/auction-#{bidders}- "
     	  args << "--num_commitments #{bidders} --rseed #{rand_seed + trial} " unless id == "auditor"
        args << "--fixmax --bmax 9000 " unless id == "auditor"
			  args << "--ccheck" if id == "vex"
        args << "--ccheck --cp #{cp}" if id == "vex-cp"

      
        party = [[ip, command, args]]
        pid_party = StartParty(party)
        WaitPids(pid_party)
      end
    end
    
    ids.each do |id|
      %x(ssh #{ip} 'cat #{$log_folder}/#{id}-#{$exp_name}-*-#{bidders}.log* > #{$log_folder}/#{id}-#{$exp_name}-final-#{bidders}.log')
    end 
  end

  sleep(2)
  keywords = ["proof generation", "audit time"]
  ind_variable = "bidders"
  AnalyzeThroughput(ids, keywords, ind_variable)
  
end

def FixedBiddersExperiment()
  ids = ["vex", "vex-cp", "vex-nocc", "auditor"]
  bidders = 20
  num_trials = 100
  max_sp = [7000, 7500, 8000, 8500, 9000, 9500, 10000]
  cp = 10  

  $exp_name.chomp!("-vs-bidders")
  $exp_name = $exp_name + "-vs-sp"  

  ip = "127.0.0.1"
  rand_seed = $prng.rand(1..10000000)
  
  
  max_sp.each do |sale_price| 

    (1..num_trials).each do |trial|
      
      ids.each do |id|

        command = "#{$git_folder}/src/vex/prover" unless id == "auditor"
			  command = "#{$git_folder}/src/vex/auditor" if id == "auditor"
        args = "--simulate 1 --log_out #{$log_folder}/#{id}-#{$exp_name}-#{trial}-#{sale_price}.log "
        args << "--benchmark --key_file #{$git_folder}/src/vex/keys/seller.priv " unless id == "auditor"
			  args << "--key_file #{$git_folder}/src/vex/keys/seller.pub " if id == "auditor"
        args << "--file_name #{$git_folder}/eval/audit_files/auction-#{sale_price}- "
     	  args << "--num_commitments #{bidders} --rseed #{rand_seed + trial} " unless id == "auditor"
        args << "--fixmax --bmax #{sale_price} " unless id == "auditor"
			  args << "--ccheck" if id == "vex"
        args << "--ccheck --cp #{cp}" if id == "vex-cp"
      
        party = [[ip, command, args]]
        pid_party = StartParty(party)
        WaitPids(pid_party)
      end
    end
    
    ids.each do |id|
      %x(ssh #{ip} 'cat #{$log_folder}/#{id}-#{$exp_name}-*-#{sale_price}.log* > #{$log_folder}/#{id}-#{$exp_name}-final-#{sale_price}.log')
    end 
  end

  sleep(2)
  keywords = ["proof generation", "audit time"]
  ind_variable = "sale price"
  AnalyzeThroughput(ids, keywords, ind_variable)
  
end


def OnlineThroughputExperiment()
	num_trials = 5
  iterations = 20
	cp = 10
	ip = "127.0.0.1"
	bidders = [10, 20, 50, 100]
	max_bid = 10000
  $num_bidders = 100

  bidders.each do |n_bidder|
    (1..iterations).each do |it|
	    rand_seed = $prng.rand(1..100000)
		
	    command = "#{$git_folder}/src/vex/bmark_tput"
	    args = "--log_out #{$log_folder}/bmark-#{$exp_name}-b#{n_bidder}-i#{it}-#{num_trials}.log "
	    args << "--key_file #{$git_folder}/src/vex/keys/seller.priv "
	    args << "--num_commitments #{n_bidder} --b_max #{max_bid} "
	    args << "--trials #{num_trials} --cp #{cp} --rseed #{rand_seed} "
	
	    party = [[ip, command, args]]	
	    pid_party = StartParty(party)
	    WaitPids(pid_party)
    end
  end

	sleep(2)
  AnalyzeOnline(bidders, iterations)
end

def AuditThroughputExperiment()
  FixedSalePriceExperiment()
  FixedBiddersExperiment()

#  $exp_name = $exp_name + "-fixed_sp"  
#  ids = ["vex", "vex-opt", "audit"]
#  keywords = ["proof generation", "audit time"]
#  ind_variable = "bidders"
#  AnalyzeThroughput(ids, keywords, ind_variable)
end
