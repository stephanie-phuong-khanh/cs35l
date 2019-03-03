#include "randcpuid.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static bool
writebytes (unsigned long long x, int nbytes)
{
  int ndigits = nbytes * 2;
  do
    {
      if (putchar ("0123456789abcdef"[x & 0xf]) < 0)
        return false;
      x >>= 4;
      ndigits--;
    }
  while (0 < ndigits);

  return 0 <= putchar ('\n');
}


/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
        perror (argv[1]);
      else
        valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library. Dynamic linking here.  */
  unsigned long long (*rand64) (void);
  void *dl_handle;
  if (rdrand_supported ()) { //hardware
    dl_handle = dlopen("randlibhw.so", RTLD_LAZY);
    if (!dl_handle) {
      fprintf(stderr, "dlopen() error for randlibhw.so - %s\n", dlerror());
      return 1;
    }
    rand64 = dlsym(dl_handle, "rand64");
    if(dlerror()){
      fprintf(stderr, "dlsym() randlibhw error - %s\n", dlerror());
      return 1;
    }
  } else { //software
    dl_handle = dlopen("randlibsw.so", RTLD_LAZY);
    if (!dl_handle) {
      fprintf(stderr, "dlopen() error for randlibsw.so - %s\n", dlerror());
      return 1;
    }
    rand64 = dlsym(dl_handle, "rand64");
    if(dlerror()){
      fprintf(stderr, "dlsym() randlibsw error - %s\n", dlerror());
      return 1;
    }
  }

  /* After dynamic linking */
  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  do
    {
      unsigned long long x = rand64 ();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes (x, outbytes))
        {
          output_errno = errno;
          break;
        }
      nbytes -= outbytes;
    }
  while (0 < nbytes);

  if (dlclose(dl_handle)){
    fprintf (stderr, "dlclose() error - %s\n", dlerror());
    return 1;
  }

  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
      return 1;
    }
  
  return 0;
}
