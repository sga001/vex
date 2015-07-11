
set terminal postscript eps enhanced monochrome font "Times-Roman, 35"

in_file = 'out2.txt'

set macro

#Labels

#set title "Audit Throughput"
set xlabel "Auction's sale price"
set ylabel "Throughput (audits/sec)" offset 0, -0.7
set key top left

# Ranges and size

set auto x
set yrange [0:300]
set size noratio 1.5, 1.0

# Ticks and border

set xtics 1000 nomirror
set ytics 100 nomirror
set mytics 2
set mxtics 2
unset border
set border 3

# Global style

set style data histogram
set style histogram cluster gap 1


#set pointsize 1.5
#set bars 2.0

# Individual style

#set style line 1 lt 0 lw 2 pt 1
#set style line 2 lt 0 lw 2 pt 1
#set style line 3 lt 0 lw 2 pt 1

set style fill solid border rgb "black"

set style line 1 lt 1 lc rgb "gray20"
set style line 2 lt 1 lc rgb "gray50"
set style line 3 lt 1 lc rgb "gray70"

VEX = "in_file using 2:xtic(1) ls 1 title 'vex auctioneer'" 
VEXOPT = "in_file using 3:xtic(1) ls 2 title 'vex-opt auctioneer'"
AUDITOR = "in_file using 4:xtic(1) ls 3 title 'auditor'"

plot @VEX, @VEXOPT, @AUDITOR
