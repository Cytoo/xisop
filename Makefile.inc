INC ?= include
SRC ?= src
OBJ ?= obj
BIN ?= bin
LIB ?= lib
DEP ?= dep

PROGRAMS_BIN    = $(PROGRAMS:%=$(BIN)/%)
PROGRAMS_ALLSRC = $(wildcard $(SRC)/*.c)
.SECONDARY:       $(PROGRAMS_ALLSRC:src/%.c=obj/%.o)

OUTDIRS = $(OBJ) $(BIN) $(DEP)
EXTRA_CFLAGS = -Wfloat-equal -Wshadow -Wunreachable-code -pedantic
CFLAGS += -I$(INC) -Werror -Wall -Wextra $(EXTRA_CFLAGS) -std=c99 -O2

all: $(OUTDIRS) $(PROGRAMS_BIN)

$(OUTDIRS):
	mkdir -pv $@

-include $(PROGRAMS_ALLSRC:src/%.c=dep/%.d)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -MM -MF $(DEP)/$*.d -MT $@ $<

PERCENT = %
.SECONDEXPANSION:
$(BIN)/%: disp_$$* $(OBJ)/%.o $$(patsubst $$(PERCENT),obj/$$(PERCENT).o,$$($$*_extrasrc))
	$(CC) $(LDFLAGS) -o $@ $(OBJ)/$*.o $($*_extrasrc:%=obj/%.o)

disp_%:
	@echo ===\> Building $*...

.PHONY: README
README:
	@cp ../template/README $@
	@echo >> $@
	@tree $(INC) $(SRC) >> $@
	@echo >> $@
	@echo "### Questions" >> $@
	@echo >> $@
	@num=1 ; for prog in $(PROGRAMS); do \
		echo Ex$$num:; \
		printf "    "; \
		./$(BIN)/$$prog -h; \
		echo; \
		num=$$((num = num + 1)); \
	done >> $@ 2>&1

clean:
	rm -rf $(OBJ)/*.o
	rm -rf $(DEP)/*.d
	rm -rf $(PROGRAMS_BIN)

cleanall: clean
	rm -rf $(OUTDIRS)
