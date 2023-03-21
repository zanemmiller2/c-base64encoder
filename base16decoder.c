// --------- includes -------------
#include "baseNencoder.h"

/* b16_isvalidchar:   checks for valid HEX characters. Returns true if valid, false otherwise */
int b16_isvalidchar(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  if (c >= 'A' && c <= 'F')
    return true;
  if (c == '=')
    return true;
  return false;
}


/* decodeBase64:  decodes data in base64 format to ascii */
void decodeBase16(int fd_in) {
  size_t nread, nwrite;
  int i, j, count;
  uint8_t inBuf[DECODE_INBUFFSIZE_16], outBuf[DECODE_OUTBUFFSIZE_16], indexes[DECODE_INBUFFSIZE_16], buffchar[2];
  count = 0;

  // read in 1 byte at a time -- checking for only valid b64 characters
  while ((nread = read(fd_in, buffchar, 1)) != 0) {
    if (nread < 0) {
      perror("error");
      exit(-1);
    }
    
    // stores valid base64 characters in the inBuf for processing
    if(b16_isvalidchar(buffchar[0])){
      inBuf[count] = *buffchar;
      count++;
    }

    // have a full (4 byte) input buffer 
    if (count == DECODE_INBUFFSIZE_16 - 1) {

      // convert the ascii index to its corepsonding base64 index
      for (j = 0; j < count; j++) {
        if (!b16_isvalidchar(inBuf[j])) {
          printf("error: Invalid base16 character");  // should already be taken care of when prepping the input buffer
          exit(-1);
        }
        
        for (i = 0; i < 17; i++) {
          if (alphabet16[i] == inBuf[j]) {
            indexes[j] = i;
            break;
          }
        }
      }

      /* --------------------- Get ASCII Character from Integer --------------------- */
      outBuf[0] = ((indexes[0] << 4) | indexes[1]);

      /* -------------------------- Write -------------------------- */
      for (size_t offset = 0; offset < count * 1 / 2;) {
        if ((nwrite = write(STDOUT_FILENO, offset + (char*)outBuf, count * 1 / 2)) < 0) {
          perror("error");
          exit(-1);
        }
        offset += nwrite;
      }
      count = 0;
    }
  }
}