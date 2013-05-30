targets=test1

all: $(targets)

$(targets) clean:
	make -C src $@
