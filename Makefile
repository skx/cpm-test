###
### Simple makefile to compile *.z80 into *.com
###
### Once that has been done all the tests can be executed
### against three emulators:
###
###     make cpmulator
###     make iz-cpm
###     make ntvcm
###


#
# Avoid printing messages when running "make test-all":
#
#   make[1]: Leaving directory '../cpm-test'
#   make[1]: Entering directory '../cpm-test'
#
MAKEFLAGS += --no-print-directory



# Z80 assembler to compile the sources with.
PASMO := pasmo

# Sources and targets
SOURCES := $(wildcard *.z80)
TARGETS := $(SOURCES:.z80=.com)
DRIVER := cpmulator -input=stty

.PHONY: all clean test

# Build all COM files
all: $(TARGETS)

# Test with a single driver
test: all
	@echo "Testing with driver: $(DRIVER) FILE.COM"
	@for i in *.com ; do \
		printf "\t$$i\t"; $(DRIVER) $$i ; \
	done

# Test with all the known drivers, assuming they're available.
test-all:
	@make test
	@make test DRIVER=iz-cpm
	@make test DRIVER=ntvcm


# Assemble .z80 -> .com
%.com: %.z80
	$(PASMO) $< $@

# Remove generated COM files
clean:
	rm -f $(TARGETS) SEQ_RDWT.DAT
