EXECUTABLES = print-devices transpose transpose-soln hello-mpi \
  hello-mpi-soln

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

hello-mpi: hello-mpi.c
	mpicc -std=gnu99 -lrt -o$@ $^

hello-mpi-soln: hello-mpi-soln.c
	mpicc -std=gnu99 -lrt -o$@ $^

%.o : %.c %.h
	mpicc -c $(CL_CFLAGS) -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o
