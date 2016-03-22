/*************************************************************
 *                                                           *
 * Assembly Guidance Systems X86_64 test                     *
 * Copyright 2015.                                           *
 *                                                           *
 * Test Name:  sinusoid1                                     *
 * Command  :  "sinusoid1"                                   *
 * Input    :    NONE.                                       *
 * Description:  Test to generate a 30hz sinusoid @ 22K      *
 *               samples per second for 1 second.  Loop      *
 *               provides a continuous wave output.          *
 *               Each sample maps position to a wave that    *
 *               cycles at 30 Hz. Each cycle on a sine       *
 *               wave's domain goes from 0 to 2 * PI,        *
 *               and we're going to do that 30 times         *
 *               or once in 22000/30 samples.                *
 *                                                           *
 ************************************************************/
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <linux/laser_api.h>
#include "sinusoid.h"

static int doWriteDevCmdNoData(uint32_t fd_laser, uint32_t command)
{
  int    rc=0;

  rc = write(fd_laser, (char *)&command, sizeof(uint32_t));
  if (rc < 0)
    syslog(LOG_ERR,"\nCMDW-NODATA: cmd %d, ERROR rc%d, errno %d\n", command, rc, errno);
  return(rc);
}

static int doWriteDevCmd32(uint32_t fd_laser, uint32_t command, uint32_t write_val)
{
  int    rc=0;
  struct cmd_rw_base *p_cmd_data;

  p_cmd_data = (struct cmd_rw_base *)malloc(sizeof(struct cmd_rw_base));
  if (!p_cmd_data)
    return(-1);
  
  p_cmd_data->cmd = command;
  p_cmd_data->dat32.val32 = write_val;
  p_cmd_data->length = sizeof(uint32_t);
  rc = write(fd_laser, (char *)p_cmd_data, sizeof(struct cmd_rw_base));
  if (rc < 0)
    syslog(LOG_ERR,"\nCMDW-DATA32: cmd %d, ERROR rc%d, errno %d\n", command, rc, errno);
  free(p_cmd_data);
  return(rc);
}
static int doLoadWriteNum(uint32_t fd_laser, uint32_t write_count)
{
  return(doWriteDevCmd32(fd_laser, CMDW_LOADWRTCNT, write_count));
}
static int doSetClock(uint32_t fd_laser, uint32_t clock_rate)
{
  return(doWriteDevCmd32(fd_laser, CMDW_SETCLOCK, clock_rate));
}
static int doDevDisplay(uint32_t fd_laser)
{
  return(doWriteDevCmdNoData(fd_laser, CMDW_DISPLAY));
}

int main( int argc, char ** argv )
{
  struct lg_xydata *pXYData;
  struct cmd_rw *cmd_buff;
  short sample;            // Sample position in wave
  short dacvolts = 0;      // DAC voltage
  unsigned char hi, lo;
  int  i;
  int  fd_laser;
  uint32_t  plen=0;

  openlog("agsd", 0, LOG_USER);
  syslog(LOG_NOTICE,"Sinusoid driver test");
  cmd_buff = (struct cmd_rw *)malloc(sizeof(struct cmd_rw));
  if (!cmd_buff)
    {
      syslog(LOG_ERR,"Unable to malloc buffer");
      exit(EXIT_FAILURE);
    }
  memset((char *)cmd_buff, 0, sizeof(struct cmd_rw));
  fd_laser = open("/dev/laser", O_RDWR);
  if (fd_laser < 0)
    {
      syslog(LOG_ERR,"laser device %d not opened,errno %d", fd_laser, errno);
      exit(EXIT_FAILURE);
    }
  // Collect xy sine wave data
  for (sample = 0; sample <= SAMPLERATE; sample++)
    {
      pXYData = (struct lg_xydata *)((char *)cmd_buff + (sizeof(struct lg_xydata) * sample));
      // convert from [-1.0,1.0] level to signed int [-32768,32767]:
      dacvolts = convert_level_to_signed_16_bit(sample);

      // Calculate hi & low bytes to send to FPGA->DAC
      pXYData->xdata = dacvolts & 0x7FFF; 
      pXYData->ydata = dacvolts & 0x7FFF;
      pXYData->ctrl_flags = BEAMONISSET | LASERENBISSET | BRIGHTBEAMISSET;
      plen+=sizeof(struct lg_xydata);
    }
  if (plen > MAX_LG_BUFFER)
    {
      syslog(LOG_ERR,"Bad size %d",plen);
      exit(EXIT_FAILURE);
    }
  doWriteDevCmdNoData(fd_laser, CMDW_STOP);
  // Write XY data to large buffer, then start display all through
  // kernel driver (drivers/misc/laserguide.c)
  cmd_buff->base.cmd = CMDW_BUFFER;
  cmd_buff->base.test_mode = DO_TEST_DISPLAY;
  cmd_buff->base.length = plen;
  write(fd_laser, cmd_buff, sizeof(struct cmd_rw));
  doLoadWriteNum(fd_laser, plen);
  doSetClock(fd_laser, KETIMER_50U);
  doDevDisplay(fd_laser);
  closelog();
  exit(EXIT_SUCCESS);
}
