#ifndef USER_ENCODE_H
#define USER_ENCODE_H

//extern volatile long ulEncoderCount;

extern unsigned char flgSgn;
extern unsigned char flgSgnOld;

extern void EncoderInit(void);
extern void EncoderDirSet(void);
extern long getEncoderCount(void);
extern void setEncoderCount(long val);

#endif
