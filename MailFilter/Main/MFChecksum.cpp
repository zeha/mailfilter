/* MFChecksum.c
 * 
 */


#define MF_CHECKSUM_ADLER32_BASE 65521L /* largest prime smaller than 65536 */
#define MF_CHECKSUM_ADLER32_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define MF_CHECKSUM_ADLER32_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define MF_CHECKSUM_ADLER32_DO2(buf,i)  MF_CHECKSUM_ADLER32_DO1(buf,i); MF_CHECKSUM_ADLER32_DO1(buf,i+1);
#define MF_CHECKSUM_ADLER32_DO4(buf,i)  MF_CHECKSUM_ADLER32_DO2(buf,i); MF_CHECKSUM_ADLER32_DO2(buf,i+2);
#define MF_CHECKSUM_ADLER32_DO8(buf,i)  MF_CHECKSUM_ADLER32_DO4(buf,i); MF_CHECKSUM_ADLER32_DO4(buf,i+4);
#define MF_CHECKSUM_ADLER32_DO16(buf)   MF_CHECKSUM_ADLER32_DO8(buf,0); MF_CHECKSUM_ADLER32_DO8(buf,8);

/* ========================================================================= */
/*uLong ZEXPORT adler32(adler, buf, len)
    uLong adler;
    const Bytef *buf;
    uInt len;*/
unsigned long MF_CheckSum_Adler32 (unsigned long adler, const unsigned char *buf, unsigned int len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    unsigned int k;

    if (len == 0) return 1L;
//    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < MF_CHECKSUM_ADLER32_NMAX ? len : MF_CHECKSUM_ADLER32_NMAX;
        len -= k;
        while (k >= 16) {
            MF_CHECKSUM_ADLER32_DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= MF_CHECKSUM_ADLER32_BASE;
        s2 %= MF_CHECKSUM_ADLER32_BASE;
    }
    return (s2 << 16) | s1;
}
