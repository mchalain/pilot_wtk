targets=test1 test1-2 test0

all: $(targets)

$(targets) clean:
	make -C src $@
