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
SOURCES := $(wildcard *.Z80)
TARGETS := $(SOURCES:.Z80=.COM)
DRIVER := cpmulator -input=stty

.PHONY: all clean test

# Build all COM files
all: $(TARGETS)

# Test with a single driver - exclude "TICKER" and "COMPLETE"
test: all
	@echo "Testing with driver: $(DRIVER) FILE.COM"
	@for i in *.COM ; do \
		case "$$i" in \
			TICKER.COM|COMPLETE.COM) continue ;; \
		esac; \
		printf "\t$$i\t"; $(DRIVER) $$i ; \
	done

# Test with all the known drivers, assuming they're available.
test-all:
	@make test
	@make test DRIVER=iz-cpm
	@make test DRIVER=ntvcm


# Run the tests, update README.md with the results
updates-tests:
	./setup.sh
	./run.sh | col -b | tee results.md
	sed -i '/^### Test Results/,$$d' README.md
	cat results.md >> README.md
	rm results.md


# Assemble .z80 -> .com
%.COM: %.Z80
	$(PASMO) $< $@

# Remove generated COM files
clean:
	rm -f $(TARGETS) SEQ_RDWT.DAT
