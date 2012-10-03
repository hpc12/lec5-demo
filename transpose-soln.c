#include "timing.h"
#include "cl-helper.h"




int main(int argc, char **argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "need two arguments!\n");
    abort();
  }

  const cl_long n = atol(argv[1]);
  const cl_long size = n*n;
  const int ntrips = atoi(argv[2]);

  cl_context ctx;
  cl_command_queue queue;
  create_context_on("NVIDIA", "590", 0, &ctx, &queue, 0);

  // --------------------------------------------------------------------------
  // load kernels 
  // --------------------------------------------------------------------------
  char *knl_text = read_file("transpose-soln.cl");
  cl_kernel knl = kernel_from_string(ctx, knl_text, "transpose", NULL);
  free(knl_text);

  // --------------------------------------------------------------------------
  // allocate and initialize CPU memory
  // --------------------------------------------------------------------------
  double *a = (double *) malloc(sizeof(double) * size);
  if (!a) { perror("alloc x"); abort(); }
  double *b = (double *) malloc(sizeof(double) * size);
  if (!b) { perror("alloc y"); abort(); }

  for (size_t j = 0; j < n; ++j)
    for (size_t i = 0; i < n; ++i)
      a[i + j*n] = i + j*n;

  // --------------------------------------------------------------------------
  // allocate device memory
  // --------------------------------------------------------------------------
  cl_int status;
  cl_mem buf_a = clCreateBuffer(ctx, CL_MEM_READ_WRITE, 
      sizeof(double) * size, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  cl_mem buf_b = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
      sizeof(double) * size, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  // --------------------------------------------------------------------------
  // transfer to device
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clEnqueueWriteBuffer, (
        queue, buf_a, /*blocking*/ CL_FALSE, /*offset*/ 0,
        size * sizeof(double), a,
        0, NULL, NULL));

  CALL_CL_GUARDED(clEnqueueWriteBuffer, (
        queue, buf_b, /*blocking*/ CL_FALSE, /*offset*/ 0,
        size * sizeof(double), b,
        0, NULL, NULL));

  // --------------------------------------------------------------------------
  // run code on device
  // --------------------------------------------------------------------------

  CALL_CL_GUARDED(clFinish, (queue));

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for (int trip = 0; trip < ntrips; ++trip)
  {
    SET_3_KERNEL_ARGS(knl, buf_a, buf_b, n);
    size_t ldim[] = { 16, 16 };
    size_t gdim[] = { n, n };
    CALL_CL_GUARDED(clEnqueueNDRangeKernel,
        (queue, knl,
         /*dimensions*/ 2, NULL, gdim, ldim,
         0, NULL, NULL));
  }

  CALL_CL_GUARDED(clFinish, (queue));

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2)/ntrips;
  printf("%f s\n", elapsed);
  printf("%f GB/s\n",
      2*size*sizeof(double)/1e9/elapsed);

  CALL_CL_GUARDED(clEnqueueReadBuffer, (
        queue, buf_b, /*blocking*/ CL_FALSE, /*offset*/ 0,
        size * sizeof(double), b,
        0, NULL, NULL));

  CALL_CL_GUARDED(clFinish, (queue));

  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      if (a[i + j*n] != b[j + i*n])
      {
        printf("bad %d %d\n", i, j);
        abort();
      }

  // --------------------------------------------------------------------------
  // clean up
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clFinish, (queue));
  CALL_CL_GUARDED(clReleaseMemObject, (buf_a));
  CALL_CL_GUARDED(clReleaseMemObject, (buf_b));
  CALL_CL_GUARDED(clReleaseKernel, (knl));
  CALL_CL_GUARDED(clReleaseCommandQueue, (queue));
  CALL_CL_GUARDED(clReleaseContext, (ctx));

  return 0;
}
