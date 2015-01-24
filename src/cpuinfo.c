/**
 * @file cpuinfo.c
 * @brief Contains RPi revision detection routines.
 * @author Jeremy Tan
 * @author Ben Croston
 * @copyright MIT License. See LICENSE for more information.
 */

#include <stdio.h>
#include <string.h>
#include "rpio.h"

static int get_cpuinfo_revision(char revision[1024])
{
   FILE *fp;
   char buffer[1024], hardware[8];
   int rpi_found = 0, rev_found = 0;;

   if ((fp = fopen("/proc/cpuinfo", "r")) == NULL) {
      return 0;
   }
  
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "Hardware	: %7s", hardware) == 1) {
            if (!strcmp(hardware, "BCM2708") || !strcmp(hardware, "BCM2835")) {
                rpi_found = 1;
            }
        } else if (sscanf(buffer, "Revision	: %1023s", revision) == 1) {
            rev_found = 1;
        }
    }
    
    fclose(fp);
    
    if (!rpi_found || !rev_found) {
        return 0;
    }
    
    return 1;
}

int rpio_get_rpi_revision(void)
{
   char raw_revision[1024];
   int len;
   char *revision;

   if (!get_cpuinfo_revision(raw_revision))
      return -1;

   // get last four characters (ignore preceeding 1000 for overvolt)
   len = strlen(raw_revision);
   if (len > 4)
      revision = ((char *)raw_revision) + len - 4;
   else
      revision = raw_revision;

   if ((strcmp(revision, "0002") == 0) ||
       (strcmp(revision, "0003") == 0))
      return 1;
   else if ((strcmp(revision, "0004") == 0) ||
            (strcmp(revision, "0005") == 0) ||
            (strcmp(revision, "0006") == 0) ||
            (strcmp(revision, "0007") == 0) ||
            (strcmp(revision, "0008") == 0) ||
            (strcmp(revision, "0009") == 0) ||
            (strcmp(revision, "000d") == 0) ||
            (strcmp(revision, "000e") == 0) ||
            (strcmp(revision, "000f") == 0))
      return 2;  // revision 2
   else if (strcmp(revision, "0011") == 0)
      return 0;  // compute module
   else   // assume B+ (0010) or A+ (0012)
      return 3;
}
