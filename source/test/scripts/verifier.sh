indir_full="../../../inputs/recorded/full/"
indir_shrt="../../../inputs/recorded/short/"
indir_dfct="../../../inputs/references/custom/speech/"

ap_full="../../../bin/x64_linux/audioprism"
full_src="../../../inputs/references/custom/speech/male_8k.wav"
shrt_src="../../../inputs/references/custom/speech/male_8k_short.wav"
full_sil="../../../inputs/references/custom/speech/male_8k_silence.wav"
shrt_sil="../../../inputs/references/custom/speech/male_8k_silence_short.wav"

cparm1="Loading Audio file|Comparing Audio files"
cparm2=".CorrVectorPeak -->|.GainFactor -->|.GainDecibels -->|.Loudness -->"
cparm3=".AudioExists -->"
params="$cparm1|$cparm2|$cparm3"


runap()
{
  ./"$1" -s "$2" -t "$3" | grep -E "$params" >> "$4"
  echo -e "\n" >> "$4"
}


echo -e "\n********************Full vs Full Comparison*********************\n\n" >> full.log

runap "$ap_full" "$full_src" "male_full.wav" "full.log"
runap "$ap_full" "$full_src" "$full_sil" "full.log"
for file in $(find "$indir_full" -name '*.wav')
do
    runap "$ap_full" "$full_src" "$file" "full.log"
done

echo -e "\n********************Full vs Short Comparison*********************\n\n" >> full.log

runap "$ap_full" "$full_src" "male_short.wav" "full.log"
runap "$ap_full" "$full_src" "$shrt_sil" "full.log"
for file in $(find "$indir_shrt" -name '*.wav')
do
    runap "$ap_full" "$full_src" "$file" "full.log"
done

echo -e "\n********************Short vs Short Comparison*********************\n\n" >> short.log

runap "$ap_full" "$shrt_src" "male_short.wav" "short.log"
runap "$ap_full" "$shrt_src" "$shrt_sil" "short.log"
for file in $(find "$indir_shrt" -name '*.wav')
do
    runap "$ap_full" "$shrt_src" "$file" "short.log"
done

echo -e "\n********************Short vs Full Comparison*********************\n\n" >> short.log

runap "$ap_full" "$shrt_src" "male_full.wav" "short.log"
runap "$ap_full" "$shrt_src" "$full_sil" "short.log"
for file in $(find "$indir_full" -name '*.wav')
do
    runap "$ap_full" "$shrt_src" "$file" "short.log"
done
