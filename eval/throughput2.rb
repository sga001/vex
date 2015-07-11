def TputOnline()
  
  num_bidders = [10, 20, 50, 100]
  total_auctions = 100
  iterations = 100
  exp_name = "online_tput"

  base_bin = {:id => "base", :adx => "adx", \
              :seller => "seller", :bidder => "bidder"}

  vex_bin = {:id => "vex", :adx => "vex_adx", \
             :seller => "vex_seller", :bidder => "vex_bidder"}

  vex_nocc_bin = {:id => "vex-nocc", :adx => "vex_adx_nocc"}

  vex_cp_bin = {:id => "vex-cp", :adx => "vex_adx_cp", \
                :bidder => "vex_bidder_cp"}
  
  variants = [base_bin, vex_bin, vex_nocc_bin, vex_cp_bin]

  parties = [:adx, :seller, :bidder]

#------------------------ End Configuration ---------------------------#

  puts "Starting throughput experiment"

  num_bidders.each do |bidders|
    variants.each do |bin|
      parties.each do |party|
        
        next if bin[:id] == "vex-nocc" and party != :adx
        next if bin[:id] == "vex-cp" and party == :seller
        
        num_auctions = total_auctions
        num_auctions = total_auctions * 100 if bin[:id] == "base" 

        command = "#{$code_dir}/throughput_#{bin[party]}"
        args = "-a #{num_auctions} -i #{iterations} -b #{bidders}"
        args << " > #{$log_dir}/#{exp_name}-#{bidders}bidders-#{bin[:id]}-#{bin[party]}-#{$ts}.log"
     
        puts "\n-----------------------------------------------------\n"
        puts "Starting #{bin[:id]}, #{bin[party]}, #{bidders} bidders"
        system "#{command} #{args}"
        puts "Experiment done"
      
      end
    end
  end

  AnalyzeOnlineTput(exp_name, iterations, num_bidders, variants, parties)
end

def TputAudit(exp_name, num_bidders, sale_prices)
 
  total_auctions = 10
  iterations = 5

  vex_bin = {:id => "vex", :audit => "vex_prover"}

  vex_nocc_bin = {:id => "vex-nocc", :audit => "vex_prover_nocc"}

  vex_cp_bin = {:id => "vex-cp", :audit => "vex_prover_cp"}

  verifier_bin = {:id => "verifier", :audit => "vex_verifier"}
  
  parties = [:audit] 
  
  variants = [vex_bin, vex_nocc_bin, vex_cp_bin, verifier_bin]

#------------------------ End Configuration ---------------------------#

  puts "Starting audit throughput experiment"


  variants.each do |bin|
    sale_prices.each do |sale_price|
      num_bidders.each do |bidders|
        parties.each do |party|
          
          command = "#{$code_dir}/throughput_#{bin[party]}"
          args = "-a #{total_auctions} -i #{iterations} -b #{bidders} -r #{$prng.rand(10000)} -m #{sale_price}"
          args << " -c 25" if bin[:id] == "vex-cp"
          args << " > #{$log_dir}/#{exp_name}-#{bidders}bidders-#{sale_price}sp-#{bin[:id]}-#{bin[party]}-#{$ts}.log"
       
          puts "\n-----------------------------------------------------\n"
          puts "Starting #{bin[:id]}, #{bin[party]}, #{bidders} bidders, sale price #{sale_price}"
          system "#{command} #{args}"
          puts "Experiment done"
        
        end
      end
    end
  end

  AnalyzeAuditTput(exp_name, iterations, num_bidders, sale_prices, variants, parties)
end


def ThroughputRun()
  TputOnline()          
end

def AuditThroughputRun()
#  TputAudit("audit_tput_fixedsp", [10, 20, 50, 100], [1000])
  TputAudit("audit_tput_fixedbidders", [20], [2000, 5000, 7000, 9000])
end
