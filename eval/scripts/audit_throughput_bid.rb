require 'rubygems'
require 'fileutils'
require 'json'

file = File.new("../out/throughput-fixed_bidders.rslt")
j_struct = JSON.load(file)

out_file = File.new("out3.txt", "w+")

line = "0 #{j_struct["Data"]["vex"].join(" ")} vex prover\n"
line << "1 #{j_struct["Data"]["vex-opt"].join(" ")} vex-opt prover\n"
line << "2 #{j_struct["Data"]["audit"].join(" ")} auditor\n"
line << "3 #{j_struct["Data"]["sale price"].join(" ")} sale price"
out_file.puts line

#(0...j_struct["Data"]["sale price"].size).each do |i|

  #line =  "#{j_struct["Data"]["sale price"][i]} "
  #line << "#{j_struct["Data"]["vex"][i]} "
  #line << "#{j_struct["Data"]["vex-opt"][i]} "
  #line << "#{j_struct["Data"]["audit"][i]}"
 # out_file.puts(line)
#end

out_file.close()
