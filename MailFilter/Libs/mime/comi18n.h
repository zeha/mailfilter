#ifndef _COMI18N_H_
#define _COMI18N_H_

#ifndef kMAX_CSNAME
#define kMAX_CSNAME 64
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Decode MIME header to UTF-8.
 * Uses MIME_ConvertCharset if the decoded string needs a conversion.
 *
 *
 * @param header      [IN] A header to decode.
 * @param default_charset     [IN] Default charset to apply to ulabeled non-UTF-8 8bit data
 * @param override_charset    [IN] If PR_TRUE, default_charset used instead of any charset labeling other than UTF-8
 * @param eatContinuations    [IN] If PR_TRUE, unfold headers
 * @return            Decoded buffer (in C string) or return NULL if the header needs no conversion
 */
 char *MIME_DecodeMimeHeader(const char *header);
/*, 
                                       const char *default_charset,
                                       bool override_charset,
                                       bool eatContinuations);*/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // _COMI18N_H_

