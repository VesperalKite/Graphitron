COMPILE_OUT = ./output/

${COMPILE_OUT}: cleanexe
	python3 ./graphitron_compiler/graphitronc.py -f ${APP} -o ${COMPILE_OUT}