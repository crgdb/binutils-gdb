# Target: arc processor
TDEPFILES = arc-tdep.o remote-arc.o
TM_FILE = tm-arc.h

REMOTE_OBS = dcache.o remote-utils.o # remote-sim.o

# This isn't supported yet and prevents gdb from building.
#SER_HARDWIRE= ser-go32-para.o
