targets=test1 test1-2

all: $(targets)

$(targets) clean:
	make -C src $@
