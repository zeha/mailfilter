/*
 * ix_adler32.c
 *
 * adler32 is under the zlib license, adopted for use within iXplat.
 *
 * 
 * Modifications Copyright 2002 Christian Hofstaedtler.
 * 
*/ 


#define IX_CHECKSUM_ADLER32_BASE 65521L /* largest prime smaller than 65536 */
#define IX_CHECKSUM_ADLER32_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define IX_CHECKSUM_ADLER32_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define IX_CHECKSUM_ADLER32_DO2(buf,i)  IX_CHECKSUM_ADLER32_DO1(buf,i); IX_CHECKSUM_ADLER32_DO1(buf,i+1);
#define IX_CHECKSUM_ADLER32_DO4(buf,i)  IX_CHECKSUM_ADLER32_DO2(buf,i); IX_CHECKSUM_ADLER32_DO2(buf,i+2);
#define IX_CHECKSUM_ADLER32_DO8(buf,i)  IX_CHECKSUM_ADLER32_DO4(buf,i); IX_CHECKSUM_ADLER32_DO4(buf,i+4);
#define IX_CHECKSUM_ADLER32_DO16(buf)   IX_CHECKSUM_ADLER32_DO8(buf,0); IX_CHECKSUM_ADLER32_DO8(buf,8);

/* ========================================================================= */
/*uLong ZEXPORT adler32(adler, buf, len)
    uLong adler;
    const Bytef *buf;
    uInt len;*/
unsigned long ix_adler32 (unsigned long adler, const unsigned char *buf, unsigned int len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    unsigned int k;

    if (len == 0) return 1L;
//    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < IX_CHECKSUM_ADLER32_NMAX ? len : IX_CHECKSUM_ADLER32_NMAX;
        len -= k;
        while (k >= 16) {
            IX_CHECKSUM_ADLER32_DO16(buf);
	    	buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= IX_CHECKSUM_ADLER32_BASE;
        s2 %= IX_CHECKSUM_ADLER32_BASE;
    }
    return (s2 << 16) | s1;
}
