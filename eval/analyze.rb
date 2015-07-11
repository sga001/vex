require 'rubygems'
require 'json'
require 'fileutils'
require 'csv'

# Regular expressions
$get_measurement = %r/[[:digit:]]*\.?[[:digit:]]+/

# Macro
$kMsToSec = 1.0/(10**3)
$kUsToSec = 1.0/(10**6)
$kUsToMs  = 1.0/(10**3)

# Statistics
module Enumerable

  def percentile(pct)
    return self.sort[(pct * self.length.to_f).ceil - 1]
  end

  def sum
    return self.inject(0){|accum, i| accum + i}
  end

  def mean
    return self.sum/self.length.to_f
  end

  def median
    return self.percentile(0.5)
  end

  def variance
    m = self.mean
    sum = self.inject(0){|accum, i| accum + (i-m)**2 }
    return sum/(self.length - 1).to_f
  end

  def standard_deviation
    return Math.sqrt(self.variance)
  end

  def mode
    m = self.inject(Hash.new(0)){ |h,v| h[v] +=1; h}
    return self.sort_by {|v| m[v] }.last
  end

end


# Returns header for exp_name.rslt file
def CreateRsltHeader(exp_name, iterations, party, opt)
  rslt_header = Hash.new
  rslt_header["exp_name"] = exp_name
  rslt_header["party"] = party
  rslt_header["iterations"] = iterations
  rslt_header["log_folder"] = $log_dir
  rslt_header["git_commit"] = %x(cat "#{$git_dir}/.git/refs/heads/master")
  rslt_header["date"] = $ts
  opt.each do |entry|
    rslt_header[entry[:id]]= entry[:value]
  end
  return rslt_header
end


def ProcessOnlineTputLog(exp_name, bidders, id, executable)

  filename = "#{$log_dir}/#{exp_name}-#{bidders}bidders-#{id}-#{executable}-#{$ts}.log"
  measurements = []

  File.open(filename).each do |line|
    measurements.push(1.0 / (Float($get_measurement.match(line).to_s) * $kUsToSec)) 
  end
 
  mean = measurements.mean.round(2)
  std = measurements.standard_deviation.round(2) 

  return mean, std
end

def ProcessAuditTputLog(exp_name, bidders, sale_price, id, executable)

  filename = "#{$log_dir}/#{exp_name}-#{bidders}bidders-#{sale_price}sp-#{id}-#{executable}-#{$ts}.log"
  measurements = []

  File.open(filename).each do |line|
    measurements.push(1.0 / (Float($get_measurement.match(line).to_s) * $kUsToSec)) 
  end
 
  mean = measurements.mean.round(2)
  std = measurements.standard_deviation.round(2) 

  return mean, std
end



def AnalyzeOnlineTput(exp_name, iterations, num_bidders, variants, parties)
  
  puts "\n================================================\n"
  puts "Analyzing throughput results"

  parties.each do |party|
    out_file = File.new("#{$analyze_dir}/#{exp_name}_#{party}.rslt", "w+")
    rslt = Hash.new

    rslt["Header"] = CreateRsltHeader(exp_name, iterations, party, [])
    data = Hash.new
  
    num_bidders.each do |bidders|
      data[bidders] = Hash.new

      variants.each do |bin|

        next if bin[:id] == "vex-nocc" and party != :adx
        next if bin[:id] == "vex-cp" and party == :seller

        mean, std = ProcessOnlineTputLog(exp_name, bidders, bin[:id], bin[party])
        data[bidders][bin[:id]] = {"mean" => mean, "std" => std} 
      end
    end

    rslt["Data"] = data
    out_file.puts JSON.dump(rslt)
    out_file.close() 
  end   
end

def AnalyzeAuditTput(exp_name, iterations, num_bidders, sale_prices, variants, parties)
  
  puts "\n================================================\n"
  puts "Analyzing throughput results"

  is_bidders = num_bidders.size > 1

  parties.each do |party|
    out_file = File.new("#{$analyze_dir}/#{exp_name}_#{party}.rslt", "w+")
    rslt = Hash.new

    rslt["Header"] = CreateRsltHeader(exp_name, iterations, party, [])
    data = Hash.new
 
    sale_prices.each do |sale_price| 
      num_bidders.each do |bidders|
        data[bidders] = Hash.new if is_bidders
        data[sale_price] = Hash.new unless is_bidders

        variants.each do |bin|

          mean, std = ProcessAuditTputLog(exp_name, bidders, sale_price, bin[:id], bin[party])
          data[bidders][bin[:id]] = {"mean" => mean, "std" => std} if is_bidders
          data[sale_price][bin[:id]] = {"mean" => mean, "std" => std} unless is_bidders
        end
      end
    end

    rslt["Data"] = data
    out_file.puts JSON.dump(rslt)
    out_file.close() 
  end   
end


def ProcessLatencyLog(exp_name, request_rate, id, iteration)

  filename = "#{$log_dir}/#{exp_name}-#{request_rate}rps-#{iteration}it-#{id}-#{$ts}.log"
  measurements = []

  File.open(filename).each do |line|
    parsed_line = line.split("time:")[1]
    measurements.push(Float($get_measurement.match(parsed_line).to_s) * $kUsToMs) 
  end

  #measurements = [1000.0] if id == "vex" and request_rate > 50

  return measurements
end


def AnalyzeLatency(exp_name, iterations, offered_loads, variants, exp_time)

  puts "Analyzing latency results\n"

	out_file = File.new("#{$analyze_dir}/#{exp_name}.rslt", "w+")	

  rslt = Hash.new
  opt = {:id => "exp_time", :value => exp_time}
	rslt["Header"] = CreateRsltHeader(exp_name, iterations, :seller, [opt])
	
  data_points = Hash.new

	variants.each do |bin|

    stats = []

    offered_loads.each do |offered_load| 
  
      if bin[:id] == "vex" and offered_load > 20
        stats.push([1000, 1000, 1000])
        next
      end

      measurements = []    
      
      (1..iterations).each do |iteration|
        measurements.concat(ProcessLatencyLog(exp_name, offered_load, bin[:id], iteration))
      end
      
      per_load_stats = []      
      per_load_stats.push(measurements.median)
      per_load_stats.push(measurements.percentile(0.05))
      per_load_stats.push(measurements.percentile(0.95))

      stats.push(per_load_stats) # Add: [median, low, high] per load per variant
    end
    
    data_points["#{bin[:id]}"] = Array.new(stats)
   
  end

  data_points["offered_load"] = offered_loads
  rslt["Data"] = data_points

  out_file.puts(JSON.dump(rslt))
  out_file.close()
end

