#ifndef __MAILFILTER_CONFIGURATION_DEFINES_HEADER
#define __MAILFILTER_CONFIGURATION_DEFINES_HEADER

#undef _MAX_PATH
#undef _MAX_SERVER
#undef _MAX_VOLUME
#undef _MAX_DRIVE
#undef _MAX_DIR
#undef _MAX_FNAME
#undef _MAX_EXT
#undef _MAX_NAME

#define _MAX_PATH    255 /* maximum length of full pathname */
#define _MAX_SERVER  48  /* maximum length of server name */
#define _MAX_VOLUME  16  /* maximum length of volume component */
#define _MAX_DRIVE   3   /* maximum length of drive component */
#define _MAX_DIR     255 /* maximum length of path component */
#define _MAX_FNAME   9   /* maximum length of file name component */
#define _MAX_EXT     5   /* maximum length of extension component */
#define _MAX_NAME    13  /* maximum length of file name */

// checks if contained has flag -- OR-ing required -- returns 1 if flag is contained, 0 otherwise
#define chkFlag(contained,flag)		((contained | flag) == contained)

#define MAILFILTER_NOTIFICATION_NONE				0x0000
#define MAILFILTER_NOTIFICATION_ADMIN_INCOMING		0x0001
#define MAILFILTER_NOTIFICATION_ADMIN_OUTGOING		0x0002
#define MAILFILTER_NOTIFICATION_SENDER_INCOMING		0x0004
#define MAILFILTER_NOTIFICATION_SENDER_OUTGOING		0x0008
#define MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING	0x0010
#define MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING	0x0020
#define MAILFILTER_NOTIFICATION_ALL					0xFFFF

#define MAILFILTER_MATCHTYPE_MATCH					0x0000
#define MAILFILTER_MATCHTYPE_NOMATCH				0x0001

#define MAILFILTER_MATCHACTION_NONE					0x0000
#define MAILFILTER_MATCHACTION_DROP_MAIL			0x0001
#define MAILFILTER_MATCHACTION_MOVE_MAIL			0x0002
#define MAILFILTER_MATCHACTION_RESCAN				0x0004
#define MAILFILTER_MATCHACTION_PASS					0x0008
#define MAILFILTER_MATCHACTION_SCHEDULE				0x0010
#define MAILFILTER_MATCHACTION_NOSCHEDULE			0x0020
#define MAILFILTER_MATCHACTION_COPY					0x0040

#define MAILFILTER_MATCHFIELD_ALWAYS				 0
#define MAILFILTER_MATCHFIELD_ATTACHMENT			 1
#define MAILFILTER_MATCHFIELD_EMAIL					 2
#define MAILFILTER_MATCHFIELD_SUBJECT				 3
#define MAILFILTER_MATCHFIELD_SIZE					 4
#define MAILFILTER_MATCHFIELD_EMAIL_FROM			 5
#define MAILFILTER_MATCHFIELD_EMAIL_TO				 6
#define MAILFILTER_MATCHFIELD_BLACKLIST				 7
#define MAILFILTER_MATCHFIELD_IPUNRESOLVABLE		 8
#define MAILFILTER_MATCHFIELD_ARCHIVECONTENTNAME	 9
#define MAILFILTER_MATCHFIELD_ARCHIVECONTENTCOUNT	10
#define MAILFILTER_MATCHFIELD_EMAIL_TOANDCC			11
#define MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC		12


#define MAILFILTER_OLD_MATCHFIELD_SUBJECT			0x0004
#define MAILFILTER_OLD_MATCHFIELD_SIZE				0x0008
#define MAILFILTER_OLD_MATCHFIELD_EMAIL_FROM		0x0010
#define MAILFILTER_OLD_MATCHFIELD_EMAIL_TO			0x0020

#define MAILFILTER_MAILTYPE_INTERNAL				0
#define MAILFILTER_MAILTYPE_EXTERNAL				1

//#define MAILFILTER_CONFIGURATION_LENGTH				329

#define MAILFILTER_MAILSOURCE_SEND					0
#define MAILFILTER_MAILSOURCE_RECEIVE				1
#define MAILFILTER_MAILSOURCE_INCOMING				1
#define MAILFILTER_MAILSOURCE_OUTGOING				0

/*------------------------------------------------------------------------
**	MailFilter Specific things
*/
// --- These are *MAILFILTER* constants! They have to be the same in ALL MailFilter programs. ---
/*#undef MAX_BUF
#define MAX_BUF 4096
#undef MAX_PATH
#define MAX_PATH _MAX_SERVER+_MAX_VOLUME+_MAX_PATH+10
  */
#define MAILFILTER_CONFIGURATION_SIGNATURE			"MAILFILTER_R001_009"		//*** MODIFY THESE
#define MAILFILTER_CONFIGURATION_THISBUILD			9							//    TWO BUILD NUMBERS ***

// DON'T TOUCH THESE UNLESS YOU ***REALLY*** KNOW WHAT YOU ARE DOING
//#define MAILFILTER_CONFIGURATION_BUILDCHECK			MFC_ConfigBuild != MAILFILTER_CONFIGURATION_THISBUILD
#define MAILFILTER_CONFIGURATION_LENGTH				329
//#define MAILFILTER_CONFIGURATIONFILE_STRING			(MAILFILTER_CONFIGURATION_LENGTH+1)

#endif // !__MAILFILTER_CONFIGURATION_DEFINES_HEADER
