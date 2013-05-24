targets=test1

all:
	$(foreach target, $(targets), make -C src $(target))
