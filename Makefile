C_COMPILER = gcc
C_OPTS = -Wextra  -std=c99 -pedantic -g
EXECUTABLE = swarm_router
BUILD_DIR = build
SOURCE_DIR = src

swarm_router: 
	${C_COMPILER} ${C_OPTS} ${SOURCE_DIR}/*.c -o ${BUILD_DIR}/${EXECUTABLE} -lpthread 

clean:
	rm -f ${BUILD_DIR}/${EXECUTABLE}