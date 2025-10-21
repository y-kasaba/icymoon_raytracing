
for FREQ in 6.510338783264160156e5
do

#for HIG in `seq 0 100 3000`
for HIG in 0
do 

RAYTRACE='./testing'

#ray-Pganymede_nonplume_100e2_0.1e2-Mtest_simple-benchmark-LO-Z-2500-FR3.984813988208770752e5-STEP10e3-PREC0.0000009
# euclid指定の場合は、惑星半径単位で。
# polar指定の場合は、MKS単位で。
#COORD="polar"   # (euclid|polar)
COORD="euclid" # (euclid|polar9
#SX=70           # (source.x|MLAT)
#SY=23           # (source.y|MLT)
#SZ=2e6         # (source.z|altitude)
#SZ=1.1e8        # (source.z|altitude)

# SX=-6000  # (past)
# 以下の初期位置の指定は計算に使用されない・初期位置はtesting/position.txtで指定される
SX=-6500 # ganymede .. -6500, europa .. -4000, callisto .. -6500
SY=0
SZ=${HIG}


## モデルを選択 ##
PLASMA="lathys_europa_3d"    #(null|test_null|simple|titan_gaussian|test_simple|europa_plume|europa_nonplume|ganymede_nonplume|sato|nsumei|lathys_europa_3d|devine_garrett)
MAGNET="test_simple"         #(null|test_null|simple|test_simple|igrf|igrf4|vip4)
PLANET="benchmark"        #(earth(?)|jupiter|benchmark)

## 宇宙の時刻を指定 ##
DATE="2000/1/1"  # year/month/day
TIME="0:0.0"     # hour:minutes.sec

## 波動特性を設定 ##
FREQ=${FREQ}      # 周波数[Hz]
MODE="RX"       # 波動モード(LO|RX)
# RAY_L=4e8     # (past) トレースする最大の光路長
RAY_L=1.3e7     # トレースする最大の光路長 [m](ganymede .. 1.3e7, europa .. 0.8e7, callisto ..1.3e7)
PITCH=0        # 磁場に対するピッチ角
SEGMENT=10000    # 出力する光路上の点の数
MAX_STEP=10000 # トレース・ステップの最大数
STEP_LENGTH=50e3   # １ステップで進む最大の光路長 (1step毎に進める最大長を[m]で指定する)
#STEP_LENGTH=${STEP_LEN}  # １ステップで進む最大の光路長 (1step毎に進める最大長を[m]で指定する)
PRECISION=0.000001  # １ステップ間のベクトル誤差の許容率
# PRECISION=${PREC}  # １ステップ間のベクトル誤差の許容率


#TIME_RANGE="4e-6:1e-13"  # １ステップ間の時間分解能レンジ  (1step毎に進める時間の最大値・最小値を指定する。)
TIME_RANGE="1:1e-100"  # １ステップ間の時間分解能レンジ  (1step毎に進める時間の最大値・最小値を指定する。)
THREAD=1

## plasma cavity ##
# --cavity [fp/fc]/[ilat]:[ilat range]/[mlt]:[mlt range]/[height upper]:[height bottom]
# すべてMKSA単位系
CAVITY_LIST=(                      \
  '--cavity 0.03/70:3/0:1/3e7:5e6' \
) # cavityの数だけオプションを指定

## 出力ファイル名を指定する。
#OUTPUT="ray-P${PLASMA}_1e2_10e2-M${MAGNET}-${PLANET}-${MODE}-Z${SZ}-FR${FREQ}"
OUTPUT="ray-P${PLASMA}_4.5e2_6e2-M${MAGNET}-${PLANET}-${MODE}-Z${SZ}-FR${FREQ}"

##OUTPUT="ray-P${PLASMA}-M${MAGNET}-${PLANET}-${MODE}-X${SX}-FR${FREQ}-PITCH${PITCH}"
LOG="${0}.log"

## 終了時にメールを送る ##
MAIL_TO="" # 終了メールを送る先を指定する。
MAIL_FROM="${USER}" # ここでFromを明示することができる。
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
	  --thread       ${THREAD}      \
	  --ray-path-segment ${SEGMENT} \
	  --planet       ${PLANET}      \
	  --parallel                    \
	  --verbose                     \
	  ${CAVITY_LIST}                \
	  2>&1                          \
	  1> ${OUTPUT}                  \
	  | tee ${LOG}

#使っていないオプション
#	  --back-trace                  \
#	  --without-plot-startptr       \
#	  --parallel                    \


	echo "END (${OUTPUT}) at " `date` >> ${LOG}
	send_mail

done

done
