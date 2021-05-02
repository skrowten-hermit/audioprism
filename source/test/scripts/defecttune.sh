#!/bin/bash -x

cparm1="Loading Audio file|Identifying Audio Signal Defects"
cparm2="Clicks Present :|Clicks Starts :|Clicks Ends :|No. of Clicks :"
cparm3="Gaps Present :|Gaps Locations :|Gaps Amplitudes :|No. of Gaps :"
cparm4="Noise Bursts :|No. of Noise Bursts :"
cparm5=".ClickExists -->|NumClicks -->|GapsExists -->|NumGaps -->|NumNoiseBursts -->"
params="$cparm1|$cparm2|$cparm3|$cparm4|$cparm5"

separator=$(printf "%-80s" "*")

echo -e "\n" >> $2
echo -e "${separator// /*}" >> $2
echo -e "\n" >> $2
echo -e $1 >> $2
echo -e "\n" >> $2
grep -E "$params" >> $2
echo -e "\n" >> $2
echo -e "${separator// /*}" >> $2
echo -e "\n" >> $2
