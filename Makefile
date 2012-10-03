EXECUTABLES = print-devices transpose transpose-soln \
	      mpi-hello mpi-hello-soln \
	      mpi-send-soln mpi-2send-soln \
	      mpi-nonblock-soln mpi-neighbor \
	      mpi-periodic-send2-soln

all: $(EXECUTABLES)

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

print-devices: print-devices.c cl-helper.o
	mpicc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@ $^

transpose: transpose.c cl-helper.o
	mpicc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@ $^

transpose-soln: transpose-soln.c cl-helper.o
	mpicc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@ $^

mpi%: mpi%.c
	mpicc -std=gnu99 -lrt -o$@ $^

%.o : %.c %.h
	mpicc -c $(CL_CFLAGS) -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o
