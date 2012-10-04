#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define BLK 16

kernel void transpose(
    global double *a,
    global double *b,
    long n)
{
#if 0
  long i = get_global_id(0);
  long j = get_global_id(1);

  if (i > j)
  {
    double tmp = a[n*i + j];
    a[n*i+j] = a[i + n*j];
    a[i + n*j] = tmp;
  }
#endif
#if 0
  long i = get_global_id(0);
  long j = get_global_id(1);

  b[i + n*j] = a[n*i + j];
#endif
#if 0
  long ig = get_group_id(0);
  long jg = get_group_id(1);
  long il = get_local_id(0);
  long jl = get_local_id(1);

  long i = ig * BLK + il;
  long j = jg * BLK + jl;

  b[i + n*j] = a[n*i + j];
#endif
#if 1
  local double l_a[16][16];
  long ig = get_group_id(0);
  long jg = get_group_id(1);
  long il = get_local_id(0);
  long jl = get_local_id(1);

  long i = ig * BLK + il;
  long j = jg * BLK + jl;

  l_a[il][jl] = a[i + n*j];
  barrier(CLK_LOCAL_MEM_FENCE);
  b[jg*BLK + n*ig*BLK + il + jl*n ] = l_a[jl][il] ;
#endif
}
