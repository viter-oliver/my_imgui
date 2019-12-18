/*
* uart.h
*
*  Created on: 2016-4-14
*      Author: viter
*/

#include  <stdio.h>
#include  <string.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <errno.h>
#include  <termios.h>
#include  <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#ifndef UART_H_
#define UART_H_

extern int setport(int fd, int baud, int databits, int stopbits, int parity);
extern int openport(const char *Dev);
extern void wait_fd_write_eable(int fd);
extern void wait_fd_read_eable(int fd);
#endif /* UART_H_ */
