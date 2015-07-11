def DelayExperiment()
  
  num_bidders = 20
  exp_time = 30                         # in seconds
  offered_load = [10, 20, 50, 100, 200] # requests per second
  iterations = 100
 
  adx_port = 9998
  seller_port = 9999
  bidders_start_port = 10000
  exp_name = "delay-#{num_bidders}bidders"

  base_bin = {:id => "base", :adx => "adx_main", :seller => "seller_main", \
              :bidder => "bidder_main"}

  vex_bin = {:id => "vex", :adx => "vex_adx_main", \
             :seller => "vex_seller_main", :bidder => "vex_bidder_main"}
 
  vex_nocc_bin = {:id => "vex-nocc", :adx => "vex_adx_nocc_main", \
                 :seller => "vex_seller_main", :bidder => "vex_bidder_main"}

  vex_cp_bin = {:id => "vex-cp", :adx => "vex_adx_cp_main", \
                :seller => "vex_seller_main", :bidder => "vex_bidder_cp_main"}
 
  variants = [base_bin, vex_bin, vex_nocc_bin, vex_cp_bin]
#  variants = [vex_nocc_bin]

#  emulab_ips = ["10.1.1.3", "10.1.2.3"]
  emulab_ips = ["155.98.38.226", "155.98.38.233"]
  emulab_idx = 0;

  #----------------- End configuration -------------------#

  
  puts "Starting delay experiment"
  (1..iterations).each do |iteration|
    offered_load.each do |request_rate|
  
      total_auctions = exp_time * request_rate

      variants.each do |bin|
        next if request_rate > 20 and bin[:id] == "vex"

        # Setup all of the bidders
        bidders = []
        (1..num_bidders).each do |bidder_i|

          ip = $emulab ? emulab_ips[emulab_idx] : "127.0.0.1"
          emulab_idx = 1 - emulab_idx if $emulab              #alternate

          port = bidders_start_port + bidder_i

          command = "#{$code_dir}/#{bin[:bidder]}"
          args = "-p #{port} -s #{$prng.rand(10000)} > "
          args << "#{$debug_dir}/#{exp_name}-#{bin[:id]}-#{bin[:bidder]}-#{bidder_i}-#{$ts}.dlog"
          
          bidders.push({:ip => ip, :port => port, :command => command, :args => args})
        end


        # Setup the seller
        ip = $emulab ? emulab_ips[emulab_idx] : "127.0.0.1"
        emulab_idx = 1 - emulab_idx if $emulab

        command = "#{$code_dir}/#{bin[:seller]}"
        args = "-p #{seller_port} -t #{total_auctions} -l #{request_rate} -s #{$prng.rand(10000)} > "
        args << "#{$log_dir}/#{exp_name}-#{request_rate}rps-#{iteration}it-#{bin[:id]}-#{$ts}.log"

        seller = {:ip => ip, :port => seller_port, :command => command, :args => args}

        # Write connections file
        WriteConnectionsFile(seller, bidders)

        # Setup the ADX
        ip = $emulab ? "127.0.0.1" : "127.0.0.1"
        command = "#{$code_dir}/#{bin[:adx]}"
        args = "#{$connections_file} "
        args << "-c 25 " if bin[:id] == "vex-cp"
        args << "> #{$debug_dir}/#{exp_name}-#{bin[:id]}-#{bin[:adx]}-#{$ts}.dlog"
        adx = {:ip => ip, :port => adx_port, :command => command, :args => args}

        puts "\n-----------------------------------------------------\n"
        puts "Starting #{bin[:id]}, #{num_bidders} bidders, #{request_rate} auctions/sec, iteration #{iteration}"
        
        StartParty(bidders)
        sleep(5)

        puts "Launching bidders"

        StartParty([seller])
        sleep(2)

        puts "Launching seller"

        adx_pid = StartParty([adx])
        sleep(2)
        puts "Launching adx"

        WaitPids(adx_pid)

        puts "Experiment done"
        sleep(2)
      end
    end
  end

  AnalyzeLatency(exp_name, iterations, offered_load, variants, exp_time)
 
end


def DelayRun()
  DelayExperiment()

end
