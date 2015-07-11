
set terminal postscript eps enhanced monochrome font "Times-Roman, 35"

in_file = 'out.txt'

set macro

#Labels

#set title "Audit Throughput"
set xlabel "Number of bidders"
set ylabel "Throughput (audits/sec)" offset 0, -0.7
set key top right

# Ranges and size

set xrange [0:100]
set yrange [0:300]
set size noratio 1.5, 1.0

# Ticks and border

set xtics 25 nomirror
set ytics 100 nomirror
set mytics 2
set mxtics 2
unset border
set border 3

# Global style

set style data linespoints
set pointsize 1.5
set bars 2.0

# Individual style

set style line 1 lt 2 lw 2 pt 9
set style line 2 lt 1 lw 2 pt 5
set style line 3 lt 32 lw 2 pt 7

VEX = "in_file using 1:2 ls 1 title 'vex auctioneer'" 
VEXOPT = "in_file using 1:3 ls 2 title 'vex-opt auctioneer'"
AUDITOR = "in_file using 1:4 ls 3 title 'auditor'"

plot @VEX, @VEXOPT, @AUDITOR
