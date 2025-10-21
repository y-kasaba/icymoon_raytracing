#!/bin/bash
#============ Slurm Options ===========
#SBATCH --job-name=jobarray
#SBATCH --time=01:00:00
#SBATCH --nodes=1 --ntasks-per-node=2
#SBATCH --partition=short
#SBATCH --mem=40gb 
#SBATCH --mail-user=clement.hottier@obspm.fr
#SBATCH --output=slurm-jobarray-%A_%a.out
#SBATCH --mail-type=ALL
#SBATCH --array=0-7%4
#============ Shell Script ============

#for HIG in -300 -200 -100 0 100 500 600 1000 1100 1500 1600 #2000 2100 2500 2600 3000 3100 #3500 3600 4000 4100 4500 4600 5000 5100 5500 5600 6000 6100 6500 6600 7000 7100 7500 7600 8000 8100
#for HIG in `seq -100 2 600`
for HIG in `seq 100 2 700`
do

#for FREQ in 3.984813988208770752e5 4.395893216133117676e5 4.849380254745483398e5 5.349649786949157715e5 5.901528000831604004e5 6.510338783264160156e5 7.181954979896545410e5 7.922856807708740234e5 8.740190267562866211e5 9.641842246055603027e5 1.063650846481323242e6 1.173378825187683105e6 1.294426321983337402e6 1.427961349487304688e6 1.575271964073181152e6 1.737779378890991211e6 1.917051434516906738e6 2.114817380905151367e6 2.332985162734985352e6 2.573659420013427734e6 2.839162111282348633e6 3.132054328918457031e6 3.455161809921264648e6 3.811601638793945312e6 4.204812526702880859e6 4.638587474822998047e6 5.117111206054687500e6 5.644999980926513672e6
#for FREQ in 3.984813988208770752e5 4.395893216133117676e5 4.849380254745483398e5 5.349649786949157715e5 5.901528000831604004e5 6.510338783264160156e5 7.181954979896545410e5 7.922856807708740234e5 8.740190267562866211e5 9.641842246055603027e5 1.063650846481323242e6 1.173378825187683105e6 1.294426321983337402e6 1.427961349487304688e6 1.575271964073181152e6
#for FREQ in 1.737779378890991211e6 1.917051434516906738e6 2.114817380905151367e6 2.332985162734985352e6 2.573659420013427734e6 2.839162111282348633e6 3.132054328918457031e6 3.455161809921264648e6 3.811601638793945312e6 4.204812526702880859e6 4.638587474822998047e6 5.117111206054687500e6 5.644999980926513672e6
for FREQ in 6.510338783264160156e5 
do

RAYTRACE='./testing'

## パラメータを設定 ##############################
## 発生源を指定 ##
# euclid指定の場合は、惑星半径単位で。
# polar指定の場合は、MKS単位で。
#COORD="polar"   # (euclid|polar)
COORD="euclid" # (euclid|polar9
#SX=70           # (source.x|MLAT)
#SY=23           # (source.y|MLT)
#SZ=2e6         # (source.z|altitude)
#SZ=1.1e8        # (source.z|altitude)

SX=-6000
SY=0
SZ=${HIG}

## モデルを選択 ##
PLASMA="ganymede_nonplume"    #(null|test_null|simple|test_simple|europa_plume|europa_nonplume|ganymede_nonplume|sato|nsumei|devine_garrett)
MAGNET="test_simple"         #(null|test_null|simple|test_simple|igrf|igrf4|vip4)
PLANET="benchmark"        #(earth(?)|jupiter|benchmark)

## 宇宙の時刻を指定 ##
DATE="2000/1/1"  # year/month/day
TIME="0:0.0"     # hour:minutes.sec

## 波動特性を設定 ##
FREQ=${FREQ}      # 周波数[Hz]
MODE="LO"       # 波動モード(LO|RX)
RAY_L=4e8     # トレースする最大の光路長
PITCH=0        # 磁場に対するピッチ角
SEGMENT=300     # 出力する光路上の点の数
#MAX_STEP=1300 # トレース・ステップの最大数
MAX_STEP=45000 # トレース・ステップの最大数
#STEP_LENGTH=100  # １ステップで進む最大の光路長 (1step毎に進める最大長を[m]で指定する)
STEP_LENGTH=10000000  # １ステップで進む最大の光路長 (1step毎に進める最大長を[m]で指定する)
PRECISION="10000"  # １ステップ間のベクトル誤差の許容率
#TIME_RANGE="1e-4:1e-6"  # １ステップ間の時間分解能レンジ  (1step毎に進める時間の最大値・最小値を指定する。)
TIME_RANGE="4e-6:1e-13"  # １ステップ間の時間分解能レンジ  (1step毎に進める時間の最大値・最小値を指定する。)

## plasma cavity ##
# --cavity [fp/fc]/[ilat]:[ilat range]/[mlt]:[mlt range]/[height upper]:[height bottom]
# すべてMKSA単位系
CAVITY_LIST=(                      \
  '--cavity 0.03/70:3/0:1/3e7:5e6' \
) # cavityの数だけオプションを指定

## 出力ファイル名を指定する。
OUTPUT="ray-P${PLASMA}_1e2_10e2-M${MAGNET}-${PLANET}-${MODE}-Z${SZ}-FR${FREQ}"

##OUTPUT="ray-P${PLASMA}-M${MAGNET}-${PLANET}-${MODE}-X${SX}-FR${FREQ}-PITCH${PITCH}"
LOG="${0}.log"

## 終了時にメールを送る ##
MAIL_TO="" # 終了メールを送る先を指定する。
MAIL_FROM="${USER}" # メールの送信元を指定する。
MAIL_SUBJECT="[mkray] ${OUTPUT} was completed."

##################################################

send_mail()
{
	if [ ${MAIL_TO} ]; then
		echo "${MAIL_SUBJECT}" | mail "${MAIL_TO}" -s "${MAIL_SUBJECT}" -- -f ${MAIL_FROM}
	fi
}

## main ##########################################
	echo "BEGIN (${OUTPUT}) at " `date` >> ${LOG}

	$RAYTRACE \
	  --plot ray                    \
	  --verbose                     \
	  --source-coord ${COORD}       \
	  --source-x     ${SX}          \
	  --source-y     ${SY}          \
	  --source-z     ${SZ}          \
	  --plasma-model ${PLASMA}      \
	  --magnet-model ${MAGNET}      \
	  --freq         ${FREQ}        \
	  --ray-mode     ${MODE}        \
	  --ray-length   ${RAY_L}       \
	  --step-count   ${MAX_STEP}    \
	  --step-length  ${STEP_LENGTH} \
	  --time-range   ${TIME_RANGE}  \
	  --precision    ${PRECISION}   \
	  --pitch        ${PITCH}       \
	  --ray-path-segment ${SEGMENT} \
	  --planet       ${PLANET}      \
	  ${CAVITY_LIST}                \
	  2>&1                          \
	  1> ${OUTPUT}                  \
	  | tee ${LOG}

# 使っていないオプション
#	  --back-trace                  \
#	  --without-plot-startptr       \
#	  --parallel                    \


	echo "END (${OUTPUT}) at " `date` >> ${LOG}
	send_mail

done

done