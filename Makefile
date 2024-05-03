C_COMPILER = gcc
C_OPTS = -Wextra  -std=c99 -pedantic -g
EXECUTABLE = swarm_router

swarm_router:
	${C_COMPILER} ${C_OPTS} *.c -o ${EXECUTABLE} -lpthread 

clean:
	rm -f ${EXECUTABLE}