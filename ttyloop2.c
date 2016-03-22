/*************************************************************
 *                                                           *
 * Assembly Guidance Systems X86_64 test                     *
 * Copyright 2015.                                           *
 *                                                           *
 * Test Name:  ttyloop2                                      *
 * Command  :  "ttyloop2"                                    *
 * Input    :    NONE.                                       *
 * Description:  This test exercises the kernel ttyS2 device *
 *               driver.  It writes and reads back a string  *
 *               to the device once a second.   Status is    *
 *               returned to user on success or failure.     *
 *                                                           *
 ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/io.h>
#include <linux/laserguide.h>
#include <string.h>

#define TEST_BUFF_LEN   128
char   *test_buff_out;

int main( int argc, char ** argv )
{
  struct  termios term;
  size_t  test_len=0;
  fd_set  rdset, wrset;
  int     fd_serial;
  int     n, baud;
  int     count=0, index;
  char    cdata_in=0,cdata_out=0;
  
  fd_serial = open("/dev/ttyS2", O_RDWR | O_NONBLOCK | O_NOCTTY);
  if (fd_serial <= 0)
    {
      perror( "Unable to open /dev/ttyS2" );
      exit(EXIT_FAILURE);
    }

  // Get current settings for /dev/ttyS2
  memset(&term, 0, sizeof(term));
  if(tcgetattr(fd_serial, &term) < 0)
    {
      perror("tcgetattr0");
      close(fd_serial);
      exit(EXIT_FAILURE);
    }

  // Set up device to our settings
  term.c_cflag = (term.c_cflag & ~CSIZE) | CS8; // 8 bits
  term.c_cflag &= ~(PARENB | PARODD | CMSPAR);  // no parity 
  term.c_cflag &= ~CSTOPB;                      // One stop bit
  term.c_cflag &= ~(CRTSCTS);                   // Flow control off
  term.c_iflag &= ~(IXON | IXOFF | IXANY);
  term.c_cflag |= CLOCAL;                       // ignore modem status lines  
  baud  = B115200;

  // Disable canonical mode, and set buffer size to 1 byte
    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ECHO;
    term.c_lflag &= ~ISIG;
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    // Set input & output baud rate
    cfsetispeed( &term, baud );
    cfsetospeed( &term, baud );

    // Enable the receiver and set local mode...
    term.c_cflag |= ( CLOCAL | CREAD );
    term.c_cflag &= ~PARENB;
    term.c_cflag &= ~CSTOPB;
    term.c_cflag &= ~CSIZE;
    term.c_cflag |= CS8;

    if (tcflush(fd_serial, TCIOFLUSH))
    {
      perror("CantFlushttyS2");
      close(fd_serial);
      exit(EXIT_FAILURE);
    }
  if( tcsetattr( fd_serial, TCSANOW, &term) < 0 ){
    perror("tcsetattr0");
    close(fd_serial);
    exit(EXIT_FAILURE);
  }
	
  // Initialize input & output test buffers
  test_buff_out = malloc(TEST_BUFF_LEN);
  if (!test_buff_out)
    {
      perror("malloc failed for outbuff");
      close(fd_serial);
      exit(EXIT_FAILURE);
    }
  memset(test_buff_out, 0, TEST_BUFF_LEN);
  sprintf(test_buff_out, "Hello from ttyS2\n");
  test_len = strlen(test_buff_out);

  // Set up file-IO for serial port
  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_SET(fd_serial, &rdset);
  FD_SET(fd_serial, &wrset);
  n = select(fd_serial + 1, &rdset, &wrset, NULL, NULL);
  if (n < 0)
    {
      perror("select failed");
      close(fd_serial);
      exit(EXIT_FAILURE);
    }

  // Start loopback test
  while (1)
    {
       count++;
       n = 0;
       fprintf(stderr, "\nTTYS2 Loop-start %d", count);
       for (index=0; index < test_len; index++)
	 {
	   cdata_out = test_buff_out[index];
	   n = write(fd_serial, &cdata_out, 1);
	   if (n <= 0 && errno == EINTR)
	     {
	       perror("WRITEfail");
	       close(fd_serial);
	       exit(EXIT_FAILURE);
	     }
	   usleep(1000);   // 1 msec wait for data 
	   n = read(fd_serial, &cdata_in, 1);
	   if ( n < 0 ) {
	     if ( errno != EAGAIN && errno != EWOULDBLOCK )
	       {
		 perror("READfail");
		 close(fd_serial);
		 exit(EXIT_FAILURE);
	       }
	   }
	   if (cdata_in != cdata_out)
	     {
	       fprintf(stderr, "\n byte IO mismatch out=%x, in=%x, byte# %d",cdata_out, cdata_in, index);
	       close(fd_serial);
	       exit(EXIT_FAILURE);
	     }
	   // Advance buffer index
	   if ((cdata_out != 0) && (cdata_in != 0))
	     fprintf(stderr, "\n MATCH out=%x, in=%x, index %d",cdata_out, cdata_in, index);
	 }
       if (tcflush(fd_serial, TCIOFLUSH))
	 {
	   perror("CantFlushttyS2");
	   close(fd_serial);
	   exit(EXIT_FAILURE);
	 }
    }
  free(test_buff_out);
  exit(EXIT_SUCCESS);
}
