subdirs:=src

all: $(subdirs)

$(subdirs): FORCE
	make -C $@

%: FORCE
	$(foreach dir,$(subdirs), make -C $(dir) $@)

clean:
	$(foreach dir,$(subdirs), make -C $(dir) $@)

FORCE: ;
