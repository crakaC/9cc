.DEFAULT_GOAL:=9cc

CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c-=.o)

define docker-run
	@docker run --platform linux/x86_64 --rm -v `pwd`:/9cc -w /9cc $(1) compilerbook
endef

$(OBJS): 9cc.h

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

.PHONY: test
test:
	$(docker-run) make
	$(docker-run) ./test.sh

.PHONY: docker-build
docker-build:
	docker build --platform linux/x86_64 -t compilerbook .

.PHONY: docker
docker:
	$(call docker-run, -it)

.PHONY: clean
clean:
	rm -f 9cc *.o *~ tmp*