//
// modified for mailfilter.
//

/***************************************************************************
 *
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized
 * or modified in any manner or compiled, linked or uploaded or downloaded
 * to or from any computer system without the prior written consent of
 * Novell, Inc.
 *
 *  $release$
 *  $modname: nwsnut.h$
 *  $version: 4.0.10$
 *  $date: Thu, Jun 9, 1994$
 *  $nokeywords$
 *
 ***************************************************************************/


#ifndef _NUT_INCLUDED_
#define _NUT_INCLUDED_


#ifndef LONG
#define LONG unsigned long
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef NULL
#define NULL 0
#endif



/****************************************************************************/
/* constants */

/* the constant CURRENT_NUT_VERSION is incremented when increased
    functionality is added. An NLM can check this value which is placed
    in the NUTInfo structure, version field, to determine if the NWSNUT
    NLM contains sufficient functionality to support its requirements */

#define CURRENT_NUT_VERSION    405

/* the constant NUT_REVISION_LEVEL is incremented when a major change
    in the behavior of NWSNUT is made. This value is not used by the calling
    NLM, but rather by NWSNUT itself to determine what is expected of it
    by the calling NLM */

#define NUT_REVISION_LEVEL     1

#define SAVE 1
#define NO_SAVE 0
#define NOHEADER 0
#define NOBORDER 0
#define NO_HELP_CONTEXT 0xffff
#define SINGLE 1
#define DOUBLE 2
#define CURSOR_OFF 0
#define CURSOR_ON 1
#define VIRTUAL 0
#define DIRECT 1

#define NUT_SEVERITY_INFORM 1
#define NUT_SEVERITY_WARNING 2
#define NUT_SEVERITY_FATAL 3


/* text size minimization styles */
#define SNORMAL            0
#define SMINWIDTH        1
#define SMINHEIGHT        2


/* palettes to set screen colors.
	background and foreground can be reversed with VREVERSE
*/

/* white and black */
#define BW_PALETTE	0

/* white and dark blue */
#define NORMAL_PALETTE     1

/* light blue and dark blue */
#define INIT_PALETTE     2

/* green and black */
#define HELP_PALETTE        3

/* red and black */
#define ERROR_PALETTE    4

/* pink and white */
#define WARNING_PALETTE    5

/* green and red */
#define OTHER_PALETTE    6



/* text and portal justification styles */
#define JRIGHT            0
#define JLEFT            1
#define JTOP            2
#define JBOTTOM            3
#define JCENTER            4
#define JTOPRIGHT        5
#define JTOPLEFT        6
#define JBOTTOMLEFT        7
#define JBOTTOMRIGHT    8

/* video constants */
#define V_UP    6
#define V_DOWN  7
#define LINE_OFFSET 160
#define EXPLODE_RATE 45
#define SCREEN_SPEED 0

/* video attributes */
#define VNORMAL        0
#define VINTENSE    1
#define VREVERSE    2
#define VBLINK        3
#define VIBLINK        4
#define VRBLINK        5


/* header types */
#define NO_HEADER        0
#define SMALL_HEADER   1
#define NORMAL_HEADER     2
#define LARGE_HEADER    3

/* keyboard constants */
#define KS_OFF            0
#define KS_ON            1
#define KS_INT            2

#define K_NORMAL        0

#define K_F1            1
#define K_F2            2
#define K_F3            3
#define K_F4            4
#define K_F5            5
#define K_F6            6
#define K_F7            7
#define K_F8            8
#define K_F9            9
#define K_F10            10

#define K_SF1            11
#define K_SF2            12
#define K_SF3            13
#define K_SF4            14
#define K_SF5            15
#define K_SF6            16
#define K_SF7            17
#define K_SF8            18
#define K_SF9            19
#define K_SF10            20

#define K_CF1            21
#define K_CF2            22
#define K_CF3            23
#define K_CF4            24
#define K_CF5            25
#define K_CF6            26
#define K_CF7            27
#define K_CF8            28
#define K_CF9            29
#define K_CF10            30

#define K_AF1            31
#define K_AF2            32
#define K_AF3            33
#define K_AF4            34
#define K_AF5            35
#define K_AF6            36
#define K_AF7            37
#define K_AF8            38
#define K_AF9            39
#define K_AF10            40

#define K_HELP            1
#define K_MODIFY        3
#define K_MARK            5
#define K_CANCEL        7
#define K_MODE            9
#define K_EXIT            40

#define K_ESCAPE        41        
#define K_BACK            42
#define K_INSERT        43
#define K_DELETE        44
#define K_SELECT        45
#define K_CYCLE            46
#define K_UP            47
#define K_DOWN            48
#define K_LEFT            49
#define K_RIGHT            50
#define K_SUP            51
#define K_SDOWN            52
#define K_SLEFT            53
#define K_SRIGHT        54
#define K_PUP            55
#define K_PDOWN            56
#define K_FRIGHT        57
#define K_FLEFT            58
#define K_DELETE_END        59

/*
For NWSUngetKey of function keys, use UGK_FUNCTION_KEY for the "type"
parameter and "K_F1" etc. for the "value" parameter
*/

#define UGK_NORMAL_KEY              0x00
#define UGK_FUNCTION_KEY				0x01

/*
For other special keys listed below, use UGK_NORMAL_KEY for the "value"
parameter, and the UGK_xxx for the "type" parameter.
*/

#define UGK_ENTER_KEY               0x02
#define UGK_ESCAPE_KEY              0x03
#define UGK_BACKSPACE_KEY           0x04
#define UGK_DELETE_KEY              0x05
#define UGK_INSERT_KEY              0x06
#define UGK_CURSOR_UP_KEY           0x07
#define UGK_CURSOR_DOWN_KEY         0x08
#define UGK_CURSOR_RIGHT_KEY        0x09
#define UGK_CURSOR_LEFT_KEY         0x0a
#define UGK_CURSOR_HOME_KEY         0x0b
#define UGK_CURSOR_END_KEY          0x0c
#define UGK_CURSOR_PUP_KEY          0x0d
#define UGK_CURSOR_PDOWN_KEY        0x0e

/*
Added in version 403

A special key type to cause LISTs to refresh. K_REFRESH_KEY may be returned
from an action procedure passed to NWSList, or another thread that wishes
to cause a list to refresh may call NWSUngetKey with the UGK version of this,
and it too will cause the list to be redrawn.

Use "type" = UGK_SPECIAL_KEY, and "value" = UGK_REFRESH_KEY
*/

#define UGK_SPECIAL_KEY					3
#define UGK_REFRESH_KEY				 0x22222222
#define K_REFRESH_KEY             UGK_REFRESH_KEY
#define OLD_REFRESH_KEY				 222


/* available action keys for list */
#define M_ESCAPE     0x0001
#define M_INSERT     0x0002
#define M_DELETE     0x0004
#define M_MODIFY     0x0008
#define M_SELECT     0x0010
#define M_MDELETE    0x0020        /* marked delete */
#define M_CYCLE      0x0040
#define M_MMODIFY    0x0080        /* marked modify */
#define M_MSELECT    0x0100        /* marked select */
#define M_NO_SORT    0x0200        /* don't sort list */
#define M_REFRESH    0x0400			/* allow the list to be refreshed */

/* return values for EditString */
#define E_ESCAPE    1
#define E_SELECT    2
#define E_EMPTY        4
#define E_CHANGE    8

/* type values for EditString */
#define EF_ANY         0x0001L
#define EF_DECIMAL     0x0002L
#define EF_HEX         0x0004L
#define EF_NOSPACES    0x0008L
#define EF_UPPER       0x0010L
#define EF_DATE        0x0020L
#define EF_TIME        0x0040L
#define EF_FLOAT       0x0080L
#define EF_SET         0x0100L
#define EF_NOECHO      0x0200L
#define EF_FILENAME	  0x0400L

/* added in version 404 */

#define EF_MASK		  0x0800L

/* and in version 405 */

#define EF_NOCONFIRM_EXIT	0x1000L



/* scroll bar stuff for NWSEditTextWithScrollBars, and NWSViewTextWithScrollBars */

/* which scroll bars to show */
#define SHOW_VERTICAL_SCROLL_BAR		2
#define SHOW_HORIZONTAL_SCROLL_BAR	4

/* when to show the scroll bars. Use ONLY one of these */
#define CONSTANT_SCROLL_BARS			0x0200
#define TEXT_SENSITIVE_SCROLL_BARS	0x0400
#define CONSIDER_LOCKED_FIELDS		0x0800




/* character and key constants */

#define F_H1         NWSGetLineDrawCharacter(0)
/* Í */
#define F_H2         NWSGetLineDrawCharacter(1)
/* ³ */
#define F_V1         NWSGetLineDrawCharacter(2)
/* º */
#define F_V2         NWSGetLineDrawCharacter(3)
/* Ú */
#define F_UL1        NWSGetLineDrawCharacter(4)
/* ¿ */
#define F_UR1        NWSGetLineDrawCharacter(5)
/* À */
#define F_LL1        NWSGetLineDrawCharacter(6)
/* Ù */
#define F_LR1        NWSGetLineDrawCharacter(7)
/* É */
#define F_UL2        NWSGetLineDrawCharacter(8)
/* » */
#define F_UR2        NWSGetLineDrawCharacter(9)
/* È */
#define F_LL2        NWSGetLineDrawCharacter(10)
/* ¼ */
#define F_LR2        NWSGetLineDrawCharacter(11)
/* Á */
#define F_UT1        NWSGetLineDrawCharacter(12)
/* Â */
#define F_DT1        NWSGetLineDrawCharacter(13)
/* ´ */
#define F_LT1        NWSGetLineDrawCharacter(14)
/* Ã */
#define F_RT1        NWSGetLineDrawCharacter(15)
/* Ê */
#define F_UT2 			NWSGetLineDrawCharacter(24)
/* Ë */
#define F_DT2 			NWSGetLineDrawCharacter(25)
/* ¹ */
#define F_LT2	 		NWSGetLineDrawCharacter(26)
/* Ì */
#define F_RT2 			NWSGetLineDrawCharacter(27)
/* Å */
#define F_X1        	NWSGetLineDrawCharacter(36)
/* Î */
#define F_X2        	NWSGetLineDrawCharacter(39)
/*  */
#define F_UP        	NWSGetLineDrawCharacter(40)
/*  */
#define F_DOWN       NWSGetLineDrawCharacter(41)
/*  */
#define F_LEFT       NWSGetLineDrawCharacter(42)
/*  */
#define F_RIGHT     	NWSGetLineDrawCharacter(43)
/* ° */
#define F_BG1        NWSGetLineDrawCharacter(44)
/* ± */
#define F_BG2        NWSGetLineDrawCharacter(45)
/* ² */
#define F_BG3        NWSGetLineDrawCharacter(46)
/* Û */
#define F_BG4        NWSGetLineDrawCharacter(47)



/* form constants (control flags) */

#define F_NOVERIFY  0x00
#define F_VERIFY    0x10
#define F_FORCE     0x20

/* a flag to pass if no help is desired in the form */

#define F_NO_HELP        0xffffffff

/**** fieldFlags Type masks ****/

#define NORMAL_FIELD        0x00        /* normal editable field */
#define LOCKED_FIELD        0x01        /* non accessable */
#define SECURE_FIELD        0x02        /* non editable */
#define REQUIRED_FIELD      0x04        /* verify field on form exit */
#define HIDDEN_FIELD        0x09        /* hidden fields are also locked */
#define PROMPT_FIELD        0x11        /* prompt fields are also locked */
#define ULOCKED_FIELD     0x0100             /* field locked by user, not by NUT */

/* MASKED_FIELD added in version 402 */

#define MASKED_FIELD      0x200          /* display '*' for text  */
                                        

#define FORM_DESELECT       0x20        /* flag to cause form deselection
                                         * before action & verify routines
                                         * are called */
#define NO_FORM_DESELECT    0x00        /* In case old flag was used */
#define DEFAULT_FORMAT      0x00        /* normal field controlled justify */
#define RIGHT_FORMAT        0x40        /* right justification format */
#define LEFT_FORMAT         0x80        /* left justification format */
#define CENTER_FORMAT       0xC0        /* centering format */


#define MAXPORTALS 50
#define MAXLISTS   20
#define SAVELISTS  20
#define MAXACTIONS 60
#define MAXFUNCTIONS MAXACTIONS
#define MAXHELP    30

#define NO_MESSAGE                      0xffff
#define DYNAMIC_MESSAGE_ONE       0xfffe
#define DYNAMIC_MESSAGE_TWO       0xfffd
#define DYNAMIC_MESSAGE_THREE     0xfffc
#define DYNAMIC_MESSAGE_FOUR         0xfffb
#define DYNAMIC_MESSAGE_FIVE         0xfffa
#define DYNAMIC_MESSAGE_SIX          0xfff9
#define DYNAMIC_MESSAGE_SEVEN        0xfff8
#define DYNAMIC_MESSAGE_EIGHT        0xfff7
#define DYNAMIC_MESSAGE_NINE         0xfff6
#define DYNAMIC_MESSAGE_TEN          0xfff5
#define DYNAMIC_MESSAGE_ELEVEN    0xfff4
#define DYNAMIC_MESSAGE_TWELVE    0xfff3
#define DYNAMIC_MESSAGE_THIRTEEN    0xfff2
#define DYNAMIC_MESSAGE_FOURTEEN 0xfff1
#define SYSTEM_MESSAGE               0x8000


#define IS_DYNAMIC_MESSAGE(a) (a > 0xfff0 && a < 0xffff)


struct NUTInfo_;


typedef struct PCB_
{
    LONG frameLine;
    LONG frameColumn;
    LONG frameHeight;
    LONG frameWidth;
    LONG virtualHeight;        /* height of virtual screen */
    LONG virtualWidth;        /* width of virtual screen */
    LONG cursorState;
    LONG borderType;
    LONG borderAttribute;
    WORD saveFlag;
	 WORD secondarySaveFlag;
    LONG directFlag;
    LONG headerAttribute;
    LONG portalLine;        /* top-most line of portal */
    LONG portalColumn;        /* left-most column of portal */
    LONG portalHeight;
    LONG portalWidth;
    LONG virtualLine;        /* position of portal over virtual portal */
    LONG virtualColumn;        /* position of portal over virtual portal */
    LONG cursorLine;
    LONG cursorColumn;
    LONG firstUpdateFlag;
    BYTE *headerText;
    BYTE *headerText2;
    BYTE *virtualScreen;
    BYTE *saveScreen;

    /* the following fields should never be referenced by any application */

    void *screenID;
    struct    NUTInfo_ *nutInfo;
    LONG sequenceNumber;
    LONG reserved1;
    LONG mtflags;
    LONG borderPalette;
    LONG showScrollBars;
    LONG lastLine;
    LONG longestLineLen;
    LONG verticalScroll;
    LONG horizontalScroll;
    LONG oldVertical;
    LONG oldHorizontal;
    void (*deHighlightFunction)(struct NUTInfo_ *, struct PCB_ *);
    void (*reHighlightFunction)(struct NUTInfo_ *, struct PCB_ *);
    void (*reportPortalUpdate)(struct PCB_ *, struct NUTInfo_ *, LONG updateType);
} PCB;

typedef struct HS_
{
    LONG nextScreen;
    LONG previousScreen;
    LONG frameLine;            /* Topmost line of frame on physical screen */
    LONG frameColumn;        /* Leftmost column of frame on physical screen */
    LONG frameHeight;        /* Height of frame on physical screen */
    LONG frameWidth;        /* Width of frame on physical screen */
    LONG virtualHeight;        /* Height of virtual screen */
    LONG virtualWidth;        /* Width of virtual screen */
    LONG cursorState;        /* Is the cursor on or off (1 or 0) */
    LONG borderType;        /* Type of border to use */
    LONG borderAttribute;    /* Attribute to use with border */
    LONG saveFlag;            /* TRUE = save old screen */
    LONG directFlag;        /* TRUE = no virtual screen */
    LONG headerAttribute;    /* Attribute of header text */
    BYTE *headerText;                /* Pointer to header text */
    BYTE *text;            /* Pointer to the help text, always assumed to */
                        /* be non NULL (when empty points to a '/0' BYTE */
} HELP_SCREEN;

typedef struct LIST_STRUCT
{
    struct LIST_STRUCT *prev;
    struct LIST_STRUCT *next;
    void               *otherInfo;
    LONG               marked;
    WORD               flags;
    WORD               maxSkew;
    void               (*entryProcedure)(struct LIST_STRUCT *listElement,
                                   LONG displayLine, void *NUTInfoStructure);
    LONG               extra;
    BYTE               text[1];
} LIST;

typedef struct LP_
{
    void *head;
    void *tail;
    int (*sortProc)();
    void (*freeProcedure)(void *memoryPointer);
} LISTPTR;

typedef struct MI_
{
    BYTE *dynamicMessageOne;
    BYTE *dynamicMessageTwo;
    BYTE *dynamicMessageThree;
    BYTE *dynamicMessageFour;
    BYTE *dynamicMessageFive;
    BYTE *dynamicMessageSix;
    BYTE *dynamicMessageSeven;
    BYTE *dynamicMessageEight;
    BYTE *dynamicMessageNine;
    BYTE *dynamicMessageTen;
    BYTE *dynamicMessageEleven;
    BYTE *dynamicMessageTwelve;
    BYTE *dynamicMessageThirteen;
    BYTE *dynamicMessageFourteen;
    LONG messageCount;
    BYTE **programMesgTable;
} MessageInfo;

typedef struct INT_
{
    void (*interruptProc)(void *handle);
    LONG key;
} INTERRUPT;

typedef struct MP_
{
    int (*listAction)(int option, void *parameter);
    void *parameter;
} MENU_PARAMETER;


/* environment structure */
typedef struct NUTInfo_
{
    PCB         *portal[MAXPORTALS];
    LONG         currentPortal;
    LONG         headerHeight;
    LONG         waitFlag;
    LISTPTR     listStack[MAXLISTS];
    LISTPTR     saveStack[SAVELISTS];
    LONG         nextAvailList;
    LIST         *head, *tail;
    int         (*defaultCompareFunction)(LIST *el1, LIST *el2);
    void        (*freeProcedure)(void *memoryPointer);
    void         (*interruptTable[MAXFUNCTIONS])();
    LONG         functionKeyStatus[MAXACTIONS];
    MessageInfo messages;
    LONG         helpContextStack[MAXHELP];
    LONG        currentPreHelpMessage;
    int          freeHelpSlot;
    LONG         redisplayFormFlag;
    LONG        preHelpPortal;
    short        helpActive;         
    short        errorDisplayActive;
    LONG        helpPortal;
    LONG         waitPortal;
    LONG         errorPortal;
    void         *resourceTag;
    void         *screenID;
    BYTE        *helpScreens;
    int         helpOffset;
    LONG        helpHelp;
    void        *allocChain;
    LONG        version;
    LONG        reserved[10];
    LONG         moduleHandle;
    void        *customData;
    void        (*customDataRelease)(void *theData, struct NUTInfo_ *thisStructure);
    LONG        displayErrorLabel;
    BYTE        *markBuffer;
    LONG        markBufferLength;
    BYTE        *editBuffer;
    LONG        editBufferLength;

    /* always leave the following fields at the end of the struct.  They
        should never be referenced directly by an application */
    LONG        staticFlag;
    LONG        processID;
    LONG        mtflags;        
    LONG        saveCurrentPortal;
    LONG        palette;
    void        *nutDataHandle;
    struct NUTInfo_    *next;
    struct NUTInfo_    *prev;
    void         (*listSortFunction)(LIST *head, LIST *tail, struct NUTInfo_ *thisStructure);
	 LONG         compatibilityLevel;
} NUTInfo;


typedef struct MFC_
{
    LONG headernum;                    /* menu header message number */
    LONG centerLine;                    /* menu center line */
    LONG centerColumn;                /* menu center column */
    LONG maxoptlen;                    /* len of longest menu option */
    int (* action)(int option, void *parameter); /* menu action routine */
    LONG arg1;
    LONG arg2;
    LONG arg3;
    LONG arg4;
    LONG arg5;
    LONG arg6;
    LISTPTR menuhead;                    /* list head for menu list */
    NUTInfo    *nutInfo;
} MFCONTROL;

typedef struct fielddef
{
    LIST *element;                      /* list element that owns the field */
    LONG fieldFlags;                    /* Control flags */
    LONG fieldLine;                     /* Line where field is located */
    LONG fieldColumn;                   /* Column where field is located */
    LONG fieldWidth;                    /* Maximum width of field */
    LONG fieldAttribute;                /* Display attribute for field */
    int fieldActivateKeys;              /* Keys that will activate the field */

    void (* fieldFormat)(struct fielddef *field, BYTE *text, LONG buffLen);
                                        /* Routine called when field selected */
    LONG (* fieldControl)(struct fielddef *field, int selectKey,
            int *fieldChanged, NUTInfo *handle);
                                        /* Routine to verify Input */
    int (* fieldVerify)(struct fielddef *field, BYTE *data, NUTInfo *handle);
    void (* fieldRelease)(struct fielddef *);     /* Data & Xtra field release routine */

    BYTE *fieldData;                    /* Pointer to data */
    BYTE *fieldXtra;                    /* Additional control info */
    int fieldHelp;                      /* help context for this field */
    struct fielddef *fieldAbove;        /* Pointer to field above */
    struct fielddef *fieldBelow;        /* Pointer to field below */
    struct fielddef *fieldLeft;         /* Pointer to field to left */
    struct fielddef *fieldRight;        /* Pointer to field to right */
    struct fielddef *fieldPrev;         /* Pointer to previous field */
    struct fielddef *fieldNext;         /* Pointer to next field */

            /* if this value is set, this routine will be called upon
                entry to each field */

    void (*fieldEntry)(struct fielddef *intoField, void *fieldData,
					NUTInfo *handle);

            /* this allows the user to have any sort of custom data that
                he wants attached to the field. */

    void *customData;

            /* and this lets him release it. Note that these parameters
                match NWSFree which allows the use of NWSAlloc for
                this data (a further guarantee that the memory will be freed */

    void (*customDataRelease)(void *fieldCustomData, NUTInfo *handle);

            /* handle to keep track of who owns the field */

    NUTInfo    *nutInfo;
} FIELD;


/* Structures used for DisplayErrorCondition() */
typedef struct PCERR_
{
    int ccodeReturned;
    int errorMessageNumber;
} PROCERROR;


typedef struct NA_
{
    void     *address;
    void     *next;
} NUT_ALLOC;


#ifdef __cplusplus
extern "C" {
#endif


extern long NWSInitializeNut(
    LONG utility,
    LONG version,
    LONG headerType,
    LONG compatibilityLevel,
    BYTE **messageTable,
    BYTE *helpScreens,
    int  screenID,            /* CLIB screen ID */
    LONG resourceTag,        /* OS ResourceTagStructure */
    NUTInfo **handle);

extern void NWSScreenSize(
    LONG *maxLines,
    LONG *maxColumns);

extern void NWSShowPortalLine(
    LONG line,
    LONG column,
    BYTE *text,
    LONG length,
    PCB *portal);

extern void NWSShowPortalLineAttribute(
    LONG line,
    LONG column,
    BYTE *text,
    LONG attribute,
    LONG length,
    PCB *portal);

extern void NWSScrollPortalZone(
    LONG line,
    LONG column,
    LONG height,
    LONG width,
    LONG attribute,
    LONG count,
    LONG direction,
    PCB *portal);

extern void NWSFillPortalZone(
    LONG line,
    LONG column,
    LONG height,
    LONG width,
    LONG fillCharacter,
    LONG fillAttribute,
    PCB *portal);

extern void NWSFillPortalZoneAttribute(
    LONG line,
    LONG column,
    LONG height,
    LONG width,
    LONG attribute,
    PCB *portal);

extern BYTE *NWSGetMessage(
    LONG message,
    MessageInfo *messages);

extern void NWSSetDynamicMessage(
    LONG message,
    BYTE *text,
    MessageInfo *messages);

extern LONG NWSCreatePortal(
    LONG line,
    LONG column,
    LONG frameHeight,
    LONG frameWidth,
    LONG virtualHeight,
    LONG virtualWidth,
    LONG saveFlag,
    BYTE *headerText,
    LONG headerAttribute,
    LONG borderType,
    LONG borderAttribute,
    LONG cursorFlag,
    LONG directFlag,
    NUTInfo *handle);

extern void NWSDestroyPortal(
    LONG portalNumber,
    NUTInfo *handle);
    
extern void NWSPositionPortalCursor(
    LONG line,
    LONG column,
    PCB  *portal);

extern void NWSEnablePortalCursor(
    PCB *portal);

extern void NWSDisablePortalCursor(
    PCB *portal);

extern void NWSDeselectPortal(
    NUTInfo *handle);

extern void NWSSelectPortal(
    LONG portalNumber,
    NUTInfo *handle);

extern int NWSComputePortalPosition(
    LONG centerLine,
    LONG centerColumn,
    LONG height,
    LONG width,
    LONG *line,
    LONG *column,
    NUTInfo *handle);

extern void NWSClearPortal(
    PCB *portal);

extern void NWSInitList(
    NUTInfo *handle,
    void (*freeRoutine)(void *memoryPointer));

extern LONG NWSPushList(
    NUTInfo *handle);

extern LONG NWSPopList(
    NUTInfo *handleNWS);

extern LONG NWSSaveList(
    LONG listIndex,
    NUTInfo *handle);

extern LONG NWSRestoreList(
    LONG listIndex,
    NUTInfo *handle);

extern void NWSDestroyList(
    NUTInfo *handle);

extern void NWSDestroyMenu(
    NUTInfo *handle);

extern void NWSDestroyForm(
    NUTInfo *handle);

extern LIST *NWSAppendToList(
    BYTE *text,
    void *otherInfo,
    NUTInfo *handle);

extern LIST *NWSDeleteFromList(
    LIST *el,
    NUTInfo *handle);

extern LIST *NWSInsertInList(
    BYTE *text,
    BYTE *otherInfo,
    LIST *atElement,
    NUTInfo *handle);

extern BYTE *NWSGetListElementText(
	LIST *element);

extern LIST *NWSGetListHead(
    NUTInfo *handle);

extern LIST *NWSGetListTail(
    NUTInfo *handle);

extern void NWSUnmarkList(
    NUTInfo *handle);

extern void NWSSetList(
    LISTPTR *listPtr,
    NUTInfo *handle);

extern void NWSGetList(
    LISTPTR *listPtr,
    NUTInfo *handle);

extern LONG NWSIsAnyMarked(
    NUTInfo *handle);

extern void NWSPushMarks(
    NUTInfo *handle);

extern void NWSPopMarks(
    NUTInfo *handle);

extern void NWSSortList(
    NUTInfo *handle);

extern void NWSInitMenu(
    NUTInfo *handle);

extern void NWSInitForm(
    NUTInfo *handle);

extern LONG NWSGetSortCharacter (
    LONG charIndex);

extern LONG NWSGetLineDrawCharacter (
    LONG charIndex);

extern LONG NWSStrcat(
    BYTE *string,
    BYTE *newStuff);

extern void NWSMemmove(
    void *dest,
    void *source,
    int len);

extern BYTE NWSToupper(
    BYTE ch);

extern int NWSIsdigit(
    BYTE ch);

extern int NWSIsxdigit(
    BYTE ch);

extern int NWSAsciiToInt(
    BYTE *data);

extern LONG NWSAsciiToLONG(
    BYTE *data);

extern int NWSAsciiHexToInt(
    BYTE *data);

extern void NWSWaitForEscape(
    NUTInfo *handle);

extern int NWSWaitForEscapeOrCancel(
    NUTInfo *handle);

extern void NWSGetKey(
    LONG *type,
    BYTE *value,
    NUTInfo *handle);

extern LONG NWSKeyStatus(
    NUTInfo *handle);

extern LONG NWSUngetKey(
    LONG type,
    LONG value,
    NUTInfo *handle);

extern void NWSEnableFunctionKey(
    LONG key,
    NUTInfo *handle);

extern void NWSDisableFunctionKey(
    LONG key,
    NUTInfo *handle);

extern void NWSDisableInterruptKey(
    LONG key,
    NUTInfo *handle);

extern void NWSEnableInterruptKey(
    LONG key,
    void (*interruptProc)(void *handle),
    NUTInfo *handle);

extern void NWSSaveFunctionKeyList(
    BYTE *keyList,
    NUTInfo *handle);

extern void NWSEnableFunctionKeyList(
    BYTE *keyList,
    NUTInfo *handle);

extern void NWSSaveInterruptList(
    INTERRUPT *interruptList,
    NUTInfo *handle);

extern void NWSEnableInterruptList(
    INTERRUPT *interruptList,
    NUTInfo *handle);

extern void NWSDisableAllInterruptKeys(
    NUTInfo *handle);

extern void NWSDisableAllFunctionKeys(
    NUTInfo *handle);

extern void NWSEnableAllFunctionKeys(
    NUTInfo *handle);

extern int NWSDisplayTextInPortal(
    LONG line,
    LONG indentLevel,
    BYTE *text,
    LONG attribute,
    PCB *portal);

extern LONG NWSDisplayInformation(
        LONG header,
        LONG pauseFlag,
        LONG centerLine,
        LONG centerColumn,
        LONG palette,
        LONG attribute,
        BYTE *displayText,
        NUTInfo *handle);

extern void NWSStartWait(
    LONG centerLine,
    LONG centerColumn,
    NUTInfo *handle);

extern void NWSEndWait(
    NUTInfo *handle);

extern LONG NWSAlert(
    LONG centerLine,
    LONG centerColumn,
    NUTInfo *handle,
    LONG message,
    ...);

extern LONG NWSAlertWithHelp(
    LONG centerLine,
    LONG centerColumn,
    NUTInfo *handle,
    LONG message,
    LONG helpContext, ...);

extern LONG NWSTrace(
    NUTInfo *handle,
    BYTE *message,
    ...);

extern void NWSDisplayErrorText(
    LONG message,
    LONG severity,
    NUTInfo *handle,
    ...);

extern void NWSDisplayErrorCondition(
    BYTE *procedureName,
    int errorCode,
    LONG    severity,
    PROCERROR *errorList,
    NUTInfo *handle, ...);

extern LIST *NWSAppendToMenu(
    LONG message,
    LONG option,
    NUTInfo *handle);

extern int NWSMenu(
    LONG header,
    LONG centerLine,
    LONG centerColumn,
    LIST *defaultElement,
    int  (*action)(int option, void *parameter),
   NUTInfo *handle,
    void *actionParameter);

extern int NWSConfirm(
    LONG header,
    LONG centerLine,
    LONG centerColumn,
    LONG defaultChoice,
    int (*action)(int option, void *parameter),
    NUTInfo *handle,
    void *actionParameter);

extern int NWSPushHelpContext(
    LONG helpContext,
    NUTInfo *handle);

extern int NWSPopHelpContext(
    NUTInfo *handle);

extern LONG NWSList(
 LONG    header,
 LONG    centerLine,
 LONG    centerColumn,
 LONG    height,
 LONG    width,
 LONG    validKeyFlags,
 LIST    **element,
 NUTInfo *handle,
 LONG    (*format)(LIST *element, LONG skew, BYTE *displayLine, LONG width),
 int     (*action)(LONG keyPressed, LIST **elementSelected,
                LONG *itemLineNumber, void *actionParameter),
 void    *actionParameter);

extern int NWSInsertInPortalList(
   LIST **currentElement,
   LONG *currentLine,
    int (*InsertProcedure)(BYTE *text, void **otherInfo, void *parameters),
    int (*FreeProcedure)(void *otherInfo),
    NUTInfo *handle,
    void *parameters);

extern int NWSModifyInPortalList(
    LIST **currentElement,
   LONG *currentLine,
    int (*ModifyProcedure)(BYTE *text, void *parameters),
    NUTInfo *handle,
    void *parameters);

extern int NWSDeleteFromPortalList(
    LIST **currentElement,
   LONG *currentLine,
    LIST *(*DeleteProcedure)(LIST *el, NUTInfo *handle, void *parameters),
    LONG deleteCurrentHeader,
    LONG deleteMarkedHeader,
    NUTInfo *handle,
    void *parameters);


extern int NWSEditString(
    LONG centerLine,
    LONG centerColumn,
    LONG editHeight,
    LONG editWidth,
    LONG header,
    LONG prompt,
    BYTE *buf,
    LONG maxLen,
    LONG type,
    NUTInfo *handle,
    int (*insertProc)(BYTE *buffer, LONG maxLen, void *parameters),
    int (*actionProc)(LONG action, BYTE *buffer, void *parameters),
    void *parameters);

extern FIELD *NWSAppendIntegerField(
    LONG line,
    LONG column,
    LONG fflag,
    int *data,
    int minimum,
    int maximum,
    LONG help,
    NUTInfo *handle);

extern FIELD *NWSAppendUnsignedIntegerField(
    LONG line,
    LONG column,
    LONG fflag,
    LONG *data,
    LONG minimum,
    LONG maximum,
    LONG help,
    NUTInfo *handle);

extern FIELD *NWSAppendHexField(
    LONG line,
    LONG column,
    LONG fflag,
    int *data,
    int minimum,
    int maximum,
    LONG help,
    NUTInfo *handle);

extern void NWSDisplayPreHelp(
    LONG line,
    LONG column,
    LONG message,
    NUTInfo *handle);

extern void NWSRemovePreHelp(
    NUTInfo *handle);

extern LONG NWSGetADisk(
    BYTE *volName,
    BYTE *prompt,
    NUTInfo *handle);

extern void NWSInitListPtr(
    LISTPTR *listPtr);


extern int NWSEditForm(
    LONG headernum,
    LONG line,
    LONG col,
    LONG portalHeight,
    LONG portalWidth,
    LONG virtualHeight,
    LONG virtualWidth,
    LONG ESCverify,
    LONG forceverify,
    LONG confirmMessage,
    NUTInfo *handle);

extern int NWSEditPortalFormField(
    LONG header,
    LONG cline,
    LONG ccol,
    LONG formHeight,
    LONG formWidth,
    LONG controlFlags,
    LONG formHelp,
    LONG confirmMessage,
    FIELD *startField,
    NUTInfo *handle);

extern int NWSEditPortalForm(
    LONG header,
    LONG centerLine,
    LONG centerColumn,
    LONG formHeight,
    LONG formWidth,
    LONG controlFlags,
    LONG formHelp,
    LONG confirmMessage,
    NUTInfo *handle);

extern FIELD *NWSAppendToForm(
    LONG fline,
    LONG fcol,
    LONG fwidth,
    LONG fattr,
    void (* fFormat)(struct fielddef *field, BYTE *text, LONG buffLen),
    LONG (* fControl)(struct fielddef *field, int selectKey,
            int *fieldChanged, NUTInfo *handle),
    int (* fVerify)(struct fielddef *field, BYTE *data, NUTInfo *handle),
    void (* fRelease)(struct fielddef *field),     /* Data & Xtra field release routine */
    BYTE *fData,
    BYTE *fXtra,
    LONG fflags,
    LONG fActivateKeys,
    LONG fhelp,
    NUTInfo *handle);

extern FIELD *NWSAppendPromptField(
    LONG line,
    LONG column,
    LONG promptnum,
    NUTInfo *handle);

extern FIELD *NWSAppendCommentField(
    LONG line,
    LONG column,
    BYTE *prompt,
    NUTInfo *handle);

extern FIELD *NWSAppendStringField(
    LONG line,
    LONG column,
    LONG width,
    LONG fflag,
    BYTE *data,
    BYTE *cset,
    LONG help,
    NUTInfo *handle);

extern FIELD *NWSAppendBoolField(
    LONG line,
    LONG column,
    LONG fflag,
    BYTE *data,
    LONG help,
    NUTInfo *handle);

extern FIELD *NWSAppendGenericBoolField(
    LONG line,
    LONG column,
    LONG fflag,
    BYTE *data,
    LONG help,
    BYTE *yesString,
    BYTE *noString,
    NUTInfo *handle);

extern FIELD *NWSAppendHotSpotField(
    LONG line,
    LONG column,
    LONG fflag,
    BYTE *displayString,
    LONG (* SpotAction)(FIELD *fp, int selectKey, int *changedField,
            NUTInfo *handle),
    NUTInfo *handle);

extern MFCONTROL *NWSInitMenuField(
    LONG headermsg,
    LONG cLine,
    LONG cCol,
    int (* action)(int option, void *parameter),
    NUTInfo *nutInfo, ...);

extern int NWSAppendToMenuField(
    MFCONTROL *m,
    LONG optiontext,
    int option,
    NUTInfo *nutInfo);

extern FIELD *NWSAppendMenuField(
    LONG line,
    LONG column,
    LONG fflag,
    int *data,
    MFCONTROL *m,
    LONG help,
    NUTInfo *nutInfo);


extern int NWSEditText(
    LONG centerLine,
    LONG centerColumn,
    LONG height,
    LONG width,
    LONG headerNumber,
    BYTE *textBuffer,
    LONG maxBufferLength,    /* length of document */
    LONG confirmMessage,
    LONG forceConfirm,
    NUTInfo *handle);

extern int NWSViewText(
    LONG centerLine,
    LONG centerColumn,
    LONG height,
    LONG width,
    LONG headerNumber,
    BYTE *textBuffer,
    LONG maxBufferLength,
    NUTInfo *handle);

extern void NWSDisplayHelpScreen(
    LONG offset,
    NUTInfo *handle);

extern void *NWSAlloc(
    LONG numberOfBytes,
    NUTInfo *handle);

extern void NWSFree(
    void *address,
    NUTInfo *handle);

extern int NWSDisplayTextJustifiedInPortal(
    LONG justify,
    LONG line,
    LONG column,
    LONG textWidth,
    BYTE *text,
    LONG attribute,
    PCB *portal);

extern int NWSDisplayInformationInPortal(
    LONG header,
    LONG portalJustifyLine,
    LONG portalJustifyColumn,
    LONG portalJustifyType,
    LONG portalPalette,
    LONG portalBorderType,
    LONG portalMaxWidth,
    LONG portalMaxHeight,
    LONG portalMinWidth,
    LONG portalMinHeight,
    LONG textLRJustifyType,
    LONG textLRIndent,
    LONG textTBJustifyType,
    LONG textTBIndent,
    LONG textAttribute,
    LONG textMinimizeStyle,
    BYTE *text,
    NUTInfo *handle);


extern void NWSRestoreNut(
    NUTInfo *handle);

extern void NWSDrawPortalBorder(
    PCB *portal);

extern void NWSUpdatePortal(
    PCB *portal);

extern void NWSSetFieldFunctionPtr(
    FIELD *fp, 
    void (*Format) (FIELD *, BYTE *text, LONG),
    LONG (*Control) (FIELD *, int, int *, NUTInfo *),
    int (*Verify) (FIELD *, BYTE *, NUTInfo *),
    void (*Release) (FIELD *),
    void (*Entry) (FIELD *, void *, NUTInfo *),
    void (*customDataRelease) (void *, NUTInfo *));


extern void NWSGetFieldFunctionPtr (FIELD *fp, 
    void (**Format) (FIELD *, BYTE *text, LONG),
    LONG (**Control) (FIELD *, int, int *, NUTInfo *),
    int (**Verify) (FIELD *, BYTE *, NUTInfo *),
    void (**Release) (FIELD *),
    void (**Entry) (FIELD *, void *, NUTInfo *),
    void (**customDataRelease) (void *, NUTInfo *));

extern void NWSSetDefaultCompare(
    NUTInfo *handle,
    int (*defaultCompareFunction)(LIST *el1, LIST *el2));

extern void NWSGetDefaultCompare (
    NUTInfo *handle,
    int (**defaultCompareFunction)(LIST *el1, LIST *el2));

/* added in version 402 */

extern void NWSSetListSortFunction(
    NUTInfo *handle,
    void (*listSortFunction)(LIST *head, LIST *tail, NUTInfo *handle));

/* added in version 402 */

extern void NWSGetListSortFunction(
    NUTInfo *handle,
    void (**listSortFunction)(LIST *head, LIST *tail, NUTInfo *handle));

extern void NWSSetScreenPalette(
    LONG newPalette,
    NUTInfo *handle);

extern LONG NWSGetScreenPalette(
    NUTInfo *handle);

extern void NWSGetPCB(
    PCB **pPcb,
    LONG portalNumber,
    NUTInfo *handle);

extern void NWSSetListNotifyProcedure(
    LIST *el, 
    void (*entryProcedure)(LIST *element,
        LONG displayLine, NUTInfo *handle));

extern void NWSGetListNotifyProcedure(
    LIST *el, 
    void (**entryProcedure)(LIST *element,
        LONG displayLine, NUTInfo *handle));

extern void NWSSetHandleCustomData(
    NUTInfo *handle,
    void *customData,
    void (*customDataRelease)(void *theData, NUTInfo *handle));
    
extern void NWSGetHandleCustomData(
    NUTInfo *handle,
    void **customData,
    void (**customDataRelease)(void *theData, NUTInfo *handle));


extern void NWSSetErrorLabelDisplayFlag(
    LONG flag,
    NUTInfo *handle);

extern void NWSSetHelpHelp(
    LONG helpIndex,
    NUTInfo *handle);


extern LONG NWSPromptForPassword (
    LONG passwordHeader,
    LONG line,
    LONG column,
    LONG maxPasswordLen,        /* max length of passwordString, including NULL */
    BYTE *passwordString,
    LONG verifyEntry,
    NUTInfo *handle);

extern FIELD *NWSAppendPasswordField(
    LONG line,
    LONG column,
    LONG width,
    LONG fflag,                        /* field flags */
    BYTE *data,                        /* ptr to field text */
    LONG maxDataLen,                /* including null */
    LONG help,                        /* help for field */
    LONG verifyEntry,                /* force password verification */
    LONG passwordPortalHeader,
    LONG maskCharacter,            /* fill character for field */
    NUTInfo *handle);


extern FIELD *NWSAppendScrollableStringField(
	LONG line,
	LONG column,
	LONG width,     /* field display width in form */
	LONG fflag,		/* field flags (NORMAL_FIELD, etc.) */
	BYTE *data,		/* ptr to field text */
	LONG maxLen,   /* max len of data, allowing for null terminator */
	BYTE *cset,		/* valid characters, if using EF_SET */
	LONG editFlags,		/* NWSEditString flags (EF_UPPER etc.) */
	LONG help,
	NUTInfo *handle);

void NWSSetScrollableFieldInsertProc (
	FIELD *fp,
	int (*insertProc) (BYTE *string, LONG maxLen, void *parameters));

/* Returns 0 for success, -1 if none selected */

LONG NWSGetCurrentPortal(
	NUTInfo *nutInfo,	
	LONG *portalNumber,		/* if not NULL, returns portal number */
	PCB **portal);				/* if not NULL, returns PCB pointer */


extern int NWSWaitForKeyAndValue(
	NUTInfo *handle,
	LONG nKeys,
	LONG keyType[], 
	LONG keyValue[]);

extern void NWSShowLineAttribute(
	LONG line,
	LONG column,
	BYTE *text,
	LONG attribute,
	LONG length,
	struct ScreenStruct *screenID);

extern void NWSShowLine(
	LONG line,
	LONG column,
	BYTE *text,
	LONG length,
	struct ScreenStruct *screenID);


extern void NWSScrollZone(
	LONG line,
	LONG column,
	LONG height,
	LONG width,
	LONG attribute,
	LONG count,
	LONG direction,
	struct ScreenStruct *screenID);

extern void NWSSaveZone(
	LONG line,
	LONG column,
	LONG height,
	LONG width,
	BYTE *buffer,
	struct ScreenStruct *screenID);

extern void NWSRestoreZone(
	LONG line,
	LONG column,
	LONG height,
	LONG width,
	BYTE *buffer,
	struct ScreenStruct *screenID);

extern void NWSRestoreDisplay(
	struct ScreenStruct *screenID);

extern void NWSPositionCursor(
	LONG line,
	LONG column,
	struct ScreenStruct *screenID);

extern void NWSGetNUTVersion(
	LONG *majorVersion,
	LONG *minorVersion,
	LONG *revision);

extern void NWSSetFormRepaintFlag(
	LONG value,
	NUTInfo *handle );

extern void NWSSetFormNoWrap(
	NUTInfo *handle);

extern int NWSViewTextWithScrollBars(
	LONG centerLine,
	LONG centerColumn,
	LONG height,
	LONG width,
	LONG headerNumber,
	BYTE *textBuffer,
	LONG maxBufferLength,
	LONG scrollBarFlag,
	NUTInfo *handle);

extern int NWSEditTextWithScrollBars(
	LONG centerLine,
	LONG centerColumn,
	LONG height,
	LONG width,
	LONG headerNumber,
	BYTE *textBuffer,
	LONG maxBufferLength,	/* length of document */
	LONG confirmMessage,
	LONG forceConfirm,
	LONG scrollBarFlag,
	NUTInfo *handle);


#ifdef __cplusplus
}
#endif

#endif
