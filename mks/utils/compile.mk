COMPILE_OUT = ./output/

${COMPILE_OUT}: cleanexe
	python3 ./facet_compiler/facetc.py -f ${APP} -o ${COMPILE_OUT}