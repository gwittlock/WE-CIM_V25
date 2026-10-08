#ifndef _DSPCONSTS_H
#define _DSPCONSTS_H

const int BYTES_PER_ROW = 8;
const int MAXROWS	= 12;
const int MAXBUF	= (BYTES_PER_ROW * MAXROWS);

const int OFFPREC	= 7;	// number of digits in displayed offset (eg. 22793:)
const int BYTEPAD	= 2;	// two spaces per byte, eg. " 0101 0101"
const int EOLWID	= 2;	// "\r\n" at end of each line

#endif