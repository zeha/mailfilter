#define _MFD_MODULE "comi18n.cpp"
#include <MailFilter.h>

#include "comi18n.h"
//#include <malloc.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

static char basis_64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define TAB 9
#define XX 127
/*
 * Table for decoding base64
 */
static char index_64[256] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
    52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
    XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
    XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
};
#define CHAR64(c)  (index_64[(unsigned char)(c)])

#define NO_Q_ENCODING_NEEDED(ch)  \
  (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || \
   ((ch) >= '0' && (ch) <= '9') ||  (ch) == '!' || (ch) == '*' ||  \
    (ch) == '+' || (ch) == '-' || (ch) == '/')

static const char hexdigits[] = "0123456789ABCDEF";

static char index_hex[256] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
     0, 1, 2, 3,  4, 5, 6, 7,  8, 9,XX,XX, XX,XX,XX,XX,
    XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
};
#define HEXCHAR(c)  (index_hex[(unsigned char)(c)])

static void convert_htab(char *s)
{
  for (; *s; ++s) {
    if (*s == TAB)
      *s = ' ';
  }
}

static char *intlmime_decode_q(const char *in, unsigned length)
{
  char *out, *dest = 0;

  out = dest = (char *)_mfd_malloc(length+1,"intlmime_decode_q");
  if (dest == NULL)
    return NULL;
  memset(out, 0, length+1);
  while (length > 0) {
    switch (*in) {
    case '=':
      if (length < 3 || HEXCHAR(in[1]) == XX ||
          HEXCHAR(in[2]) == XX) goto badsyntax;
      *out++ = (HEXCHAR(in[1]) << 4) + HEXCHAR(in[2]);
      in += 3;
      length -= 3;
      break;

    case '_':
      *out++ = ' ';
      in++;
      length--;
      continue;

    default:
      if (*in & 0x80) goto badsyntax;
      *out++ = *in++;
      length--;
    }
  }
  *out++ = '\0';
  convert_htab(dest);
  return dest;

 badsyntax:
  _mfd_free(dest,"intlmime_decode_q");
  return NULL;
}

static char *intlmime_decode_b (const char *in, unsigned length)
{
  char *out, *dest = 0;
  int c1, c2, c3, c4;

  out = dest = (char *)_mfd_malloc(length+1,"intlmime_decode_b");
  if (dest == NULL)
    return NULL;

  while (length > 0) {
    while (length > 0 && CHAR64(*in) == XX) {
      if (*in == '=') goto badsyntax;
      in++;
      length--;
    }
    if (length == 0) break;
    c1 = *in++;
    length--;
    
    while (length > 0 && CHAR64(*in) == XX) {
      if (*in == '=') goto badsyntax;
      in++;
      length--;
    }
    if (length == 0) goto badsyntax;
    c2 = *in++;
    length--;
    
    while (length > 0 && *in != '=' && CHAR64(*in) == XX) {
      in++;
      length--;
    }
    if (length == 0) goto badsyntax;
    c3 = *in++;
    length--;
    
    while (length > 0 && *in != '=' && CHAR64(*in) == XX) {
      in++;
      length--;
    }
    if (length == 0) goto badsyntax;
    c4 = *in++;
    length--;

    c1 = CHAR64(c1);
    c2 = CHAR64(c2);
    *out++ = ((c1<<2) | ((c2&0x30)>>4));
    if (c3 == '=') {
      if (c4 != '=') goto badsyntax;
      break; /* End of data */
    }
    c3 = CHAR64(c3);
    *out++ = (((c2&0x0F) << 4) | ((c3&0x3C) >> 2));
    if (c4 == '=') {
      break; /* End of data */
    }
    c4 = CHAR64(c4);
    *out++ = (((c3&0x03) << 6) | c4);
  }
  *out++ = '\0';
  return dest;

badsyntax:
  _mfd_free(dest,"intlmime_decode_b");
  return NULL;
}

static bool intl_is_utf8(const char *input, unsigned len)
{
  int c;
  /*
   * Input which contains legal HZ sequences should not be detected
   * as UTF-8.
   */
  enum { hz_initial, /* No HZ seen yet */
         hz_escaped, /* Inside an HZ ~{ escape sequence */
         hz_seen, /* Have seen at least one complete HZ sequence */
         hz_notpresent /* Have seen something that is not legal HZ */
  } hz_state;

  hz_state = hz_initial;
  while (len) {
    c = (unsigned char)*input++;
    len--;
    if (c == 0x1B) return false;
    if (c == '~') {
      switch (hz_state) {
      case hz_initial:
      case hz_seen:
        if (*input == '{') {
          hz_state = hz_escaped;
        } else if (*input == '~') {
          /* ~~ is the HZ encoding of ~.  Skip over second ~ as well */
          hz_state = hz_seen;
          input++;
          len--;
        } else {
          hz_state = hz_notpresent;
        }
        break;

      case hz_escaped:
        if (*input == '}') hz_state = hz_seen;
        break;
      }
      continue;
    }
    if ((c & 0x80) == 0) continue;
    hz_state = hz_notpresent;
    if ((c & 0xE0) == 0xC0) {
      if (len < 1 || (*input & 0xC0) != 0x80 ||
        ((c & 0x1F)<<6) + (*input & 0x3f) < 0x80) {
        return false;
      }
      input++;
      len--;
    } else if ((c & 0xF0) == 0xE0) {
      if (len < 2 || (input[0] & 0xC0) != 0x80 ||
        (input[1] & 0xC0) != 0x80) {
        return false;
      }
      input += 2;
      len -= 2;
    } else if ((c & 0xF8) == 0xF0) {
      if (len < 3 || (input[0] & 0xC0) != 0x80 ||
        (input[1] & 0xC0) != 0x80 || (input[2] & 0xC0) != 0x80) {
        return false;
      }
      input += 2;
      len -= 2;
    } else {
      return false;
    }
  }
  if (hz_state == hz_seen) return false;
  return true;
}

static void intl_copy_uncoded_header(char **output, const char *input,
  unsigned len, const char *default_charset)
{
  int c;
  char *dest = *output;

  if (!default_charset) {
    memcpy(dest, input, len);
    *output = dest + len;
    return;
  }

  // Copy as long as it's US-ASCII.  An ESC may indicate ISO 2022
  // A ~ may indicate it is HZ
  while (len && (c = (unsigned char)*input++) != 0x1B && c != '~' && !(c & 0x80)) {
    *dest++ = c;
    len--;
  }
  if (!len) {
    *output = dest;
    return;
  }
  input--;

  // If not UTF-8, treat as default charset
//  nsAutoString tempUnicodeString;
//  if (!intl_is_utf8(input, len))    {
//    if (NS_FAILED(ConvertToUnicode(default_charset, nsCAutoString(input, len).get(), tempUnicodeString))) {
/*      // Failed to convert. Populate the outString with Unicode Replacement Char
      tempUnicodeString.Truncate();
      for (unsigned i = 0; i < len; i++) {
        tempUnicodeString.Append((PRUnichar)0xFFFD);
      }
    }
    NS_ConvertUCS2toUTF8 utf8_text(tempUnicodeString);
    int output_len = utf8_text.Length();
    memcpy(dest, utf8_text.get(), output_len);
    *output = dest + output_len;
  } else {*/
    memcpy(dest, input, len);
    *output = dest + len;
//  }
}

static const char especials[] = "()<>@,;:\\\"/[]?.=";

static
char *intl_decode_mime_part2_str(const char *header,
  const char *default_charset, bool override_charset)
{
  char *output_p = NULL;
  char *retbuff = NULL;
  const char *p, *q, *r;
  char *decoded_text;
  const char *begin; /* tracking pointer for where we are in the input buffer */
  int last_saw_encoded_word = 0;
  const char *charset_start, *charset_end;
  char charset[80];
  //nsAutoString tempUnicodeString;

  // initialize charset name to an empty string
  charset[0] = '\0';

  /* Assume no more than 3X expansion due to UTF-8 conversion */
  retbuff = (char *)_mfd_malloc(3*strlen(header)+1,"intl_decode_mime_part2_str");

  if (retbuff == NULL)
    return NULL;

  output_p = retbuff;
  begin = header;

  while ((p = strstr(begin, "=?")) != 0) {
    if (last_saw_encoded_word) {
      /* See if it's all whitespace. */
      for (q = begin; q < p; q++) {
        if (!strchr(" \t\r\n", *q)) break;
      }
    }

    if (!last_saw_encoded_word || q < p) {
      /* copy the part before the encoded-word */
      intl_copy_uncoded_header(&output_p, begin, p - begin, default_charset);
      begin = p;
    }

    p += 2;

    /* Get charset info */
    charset_start = p;
    charset_end = 0;
    for (q = p; *q != '?'; q++) {
      if (*q <= ' ' || strchr(especials, *q)) {
        goto badsyntax;
      }

      /* RFC 2231 section 5 */
      if (!charset_end && *q == '*') {
        charset_end = q; 
      }
    }
    if (!charset_end) {
      charset_end = q;
    }

    /* Check for too-long charset name */
    if ((unsigned)(charset_end - charset_start) >= sizeof(charset)) goto badsyntax;
    
    memcpy(charset, charset_start, charset_end - charset_start);
    charset[charset_end - charset_start] = 0;

    q++;
    if (*q != 'Q' && *q != 'q' && *q != 'B' && *q != 'b')
      goto badsyntax;

    if (q[1] != '?')
      goto badsyntax;

    r = q;
    for (r = q + 2; *r != '?'; r++) {
      if (*r < ' ') goto badsyntax;
    }
    if (r[1] != '=')
	    goto badsyntax;
    else if (r == q + 2) {
	    // it's empty, skip
	    begin = r + 2;
	    last_saw_encoded_word = 1;
	    continue;
    }


    if(*q == 'Q' || *q == 'q')
      decoded_text = intlmime_decode_q(q+2, r - (q+2));
    else
      decoded_text = intlmime_decode_b(q+2, r - (q+2));

    if (decoded_text == NULL)
      goto badsyntax;

    // Override charset if requested.  Never override labeled UTF-8.
    if (override_charset && 0 != strcmp(charset, "UTF-8")) {
      strncpy(charset, default_charset, sizeof(charset)-1);
      charset[sizeof(charset)-1] = '\0';
    }

/*    if (NS_SUCCEEDED(ConvertToUnicode(charset, decoded_text, tempUnicodeString))) {
      NS_ConvertUCS2toUTF8 utf8_text(tempUnicodeString.get());
      PRInt32 utf8_len = utf8_text.Length();

      memcpy(output_p, utf8_text.get(), utf8_len);
      output_p += utf8_len;
    } else {
      PL_strcpy(output_p, "\347\277\275"); // UTF-8 encoding of U+FFFD 
      output_p += 3;
    }*/
    strcpy(output_p,decoded_text);
    output_p += strlen(decoded_text);

    _mfd_free(decoded_text,"intl_decode_mime_part2_str");

    begin = r + 2;
    last_saw_encoded_word = 1;
    continue;

  badsyntax:
    /* copy the part before the encoded-word */
    strncpy(output_p, begin, p - begin);
    output_p += p - begin;
    begin = p;
    last_saw_encoded_word = 0;
  }

  /* put the tail back  */
  intl_copy_uncoded_header(&output_p, begin, strlen(begin), default_charset);
  *output_p = '\0';
  convert_htab(retbuff);

  return retbuff;
}


////////////////////////////////////////////////////////////////////////////////
// BEGIN PUBLIC INTERFACE
extern "C" {
#define PUBLIC

//const char *default_charset,
//                                       bool override_charset,
//                                       bool eatContinuations
extern "C" char *MIME_DecodeMimeHeader(const char *header)
{
  char *result = NULL;

  if (header == 0)
	return NULL;

	// If not MIME encoded -> do nothing, otherwise decode the input.
	if (strstr(header, "=?")
  /* ||
      (default_charset && !intl_is_utf8(header, strlen(header)))*/) {
		result = intl_decode_mime_part2_str(header, "", false);
	} else {
		result = _mfd_strdup(header,"DecodeMimeHeader");
	}
  
/*   else if (eatContinuations && 
             (strchr(header, '\n') || strchr(header, '\r'))) {
    result = strdup(header);
  } else {
    eatContinuations = false;
  }
  if (eatContinuations)
    result = MIME_StripContinuations(result);
*/
	return result;
}  

}