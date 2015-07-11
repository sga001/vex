#!/usr/bin/env ruby

require 'rubygems'
require 'json'
require 'fileutils'
require 'csv'
require 'optparse'
require_relative 'analyze.rb'    # analyze experiment results
require_relative 'delay.rb'      # delay-related experiments
require_relative 'throughput2.rb' # throughput-related experiments

$emulab = false
$delay = false
$throughput = false
$audit = false

OptionParser.new do |opts|
  opts.banner = "Usage: master.rb [options]"

  opts.on("-e", "--emulab", "Run on emulab") do |e|
    $emulab = e
  end

  opts.on("-d", "--delay", "Enable delay experiment") do |d|
    $delay = d
  end
  
  opts.on("-a", "--audit", "Enable audit experiment") do |a|
    $audit = a
  end

  opts.on("-t", "--throughput", "Enable throughput experiment") do |t|
    $throughput = t
  end
  
end.parse!

$git_dir        = File.expand_path("..",Dir.pwd)
$code_dir = $emulab ? "/emulab-path/vex-code/bin" : "#{$git_dir}/bin"
$current_dir    = Dir.pwd
$analyze_dir = $emulab ? "/emulab-path/vex-results" : "#{$current_dir}/results" 
$log_dir     = $emulab ? "/emulab-path/vex-logs" : "#{$current_dir}/logs"
$debug_dir   = "#{$log_dir}/debug"
$connections_file = "#{$current_dir}/connections.txt"

$ts             = Time.now.strftime("%Y-%m-%d-%H-%M-%S")
$prng           = Random.new(22352212345)
$emu_command    = "ssh -o StrictHostKeyChecking=no"

FileUtils.mkdir_p($log_dir)
FileUtils.mkdir_p($debug_dir)
FileUtils.mkdir_p($analyze_dir)

#--------------------- End of configuration --------------------#




# Updates and compiles the code
def Setup()
  puts "\nUpdating to the latest version of VEX"

  system "cd #{$git_dir} && git pull"     # Update git repo
  system "cd #{$code_dir} && git pull"        # Compile VEX

  puts "Setup is complete"
  puts "=====================================================\n\n"
end


# Wait for a process or processes to be done. Block until then.
# List of ip, pid pairs
def WaitPids(list)
  list.each{|map|
    command = ""
    command << "#{$emu_command} #{map[:ip]} '" if $emulab
    command << "ps -p #{map[:pid]}"
    command << "'" if $emulab
    
    while (%x(#{command}).include? "#{map[:pid]}") do
      sleep(2)
    end
  }
end


# Write the file that contains all the connection information
def WriteConnectionsFile(seller, bidders)
  f = File.new("#{$connections_file}", "w+")

  f.puts("\# Seller")
  f.puts("#{seller[:ip]},#{seller[:port]}")

  f.puts("\# Bidders")
  bidders.each do |bidder|
    f.puts("#{bidder[:ip]},#{bidder[:port]}")
  end	

  f.close()
  sleep(2)
end


# Launches parties and returns their ip, pid mapping
# party = (ip, command, arguments) tuple
def StartParty(parties)
  map_list = [] 

  parties.each{ |party| 
    pid_map = Hash.new
    pid_map[:ip] = party[:ip]

    command = ""
    command << "#{$emu_command} #{party[:ip]} '" if $emulab
    command << "nohup #{party[:command]} #{party[:args]} 2>&1"
    command << "& echo $!"
    command << "'" if $emulab

    pid_map[:pid] = Integer(%x(#{command}).chomp!)  
    map_list.push(pid_map) 
    sleep(0.2)
  }

  return map_list
end

# Stops all parties in the list.
def StopParty(parties)

  parties.each{ |party|
    command = ""
    command << "#{$emu_command} #{party[:ip]} '" if $emulab
    command << "kill -9 #{party[:pid]}"
    command << "'" if $emulab

    system "#{command}"
  } 
end

# Run experiments
def Run()
  Setup()           
  puts "Emulab mode activated" if $emulab
  DelayRun() if $delay                # Defined in delay.rb
  ThroughputRun() if $throughput      # Defined in throughput2.rb
  AuditThroughputRun() if $audit      # Defined in throughput2.rb
end


Run()
