#!/bin/bash

MINIMUM_SIZE=("100" "75" "50" "45" "40" "35" "30" "25" "20" "15" "10" "10" "10" "10" "10" "10" "10" "10")
THRESHOLD=("250" "200" "175" "150" "135" "120" "105" "90" "75" "60" "45" "30" "15" "10" "5" "1" "0.5" "0.1")

# Score
for I in `seq 0 17`
do
    # reFH
    ../bin/refh_cli ../data/BSDS500/images/test_2/ --sigma 0 --threshold "${THRESHOLD[$I]}" --minimum-size "${MINIMUM_SIZE[$I]}" -o ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -w
    ../bin/eval_summary_cli ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" ../data/BSDS500/images/test_2 ../data/BSDS500/csv_groundTruth/test_2 --append-file ../output/refh.csv --vis
    find ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -type f -name '*[^summary|correlation|results].csv' -delete
    
    # FH
    ../bin/fh_cli ../data/BSDS500/images/test_2/ --sigma 0 --threshold "${THRESHOLD[$I]}" --minimum-size "${MINIMUM_SIZE[$I]}" -o ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -w
    ../bin/eval_summary_cli ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" ../data/BSDS500/images/test_2 ../data/BSDS500/csv_groundTruth/test_2 --append-file ../output/fh.csv --vis
    find ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -type f -name '*[^summary|correlation|results].csv' -delete
done
