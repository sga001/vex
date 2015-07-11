require 'rubygems'
require 'fileutils'
require 'json'

file = File.new("../out/throughput-fixed_sp.rslt")
j_struct = JSON.load(file)

out_file = File.new("out.txt", "w+")

(0...j_struct["Data"]["bidders"].size).each do |i|
  line =  "#{j_struct["Data"]["bidders"][i]} "
  line << "#{j_struct["Data"]["vex"][i]} "
  line << "#{j_struct["Data"]["vex-opt"][i]} "
  line << "#{j_struct["Data"]["audit"][i]}"
  out_file.puts(line)
end

out_file.close()
