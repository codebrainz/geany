/*
*
*   Copyright (c) 1998-2001, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to read.c
*/
#ifndef _READ_H
#define _READ_H

/*
*   INCLUDE FILES
*/
#include "general.h"    /* must always come first */

#include <stdio.h>
#include <ctype.h>

#include "mio.h"
#include "parse.h"
#include "vstring.h"

/*
*   MACROS
*/
#define getInputLineNumber()    File.input.lineNumber
#define getInputFileName()  vStringValue (File.input.name)
#define getInputFilePosition()  File.filePosition
#define getSourceFileTagPath()  vStringValue (File.source.tagPath)
#define getSourceLanguage() File.source.language
#define getSourceLanguageName() getLanguageName (File.source.language)
#define getSourceLineNumber()   File.source.lineNumber
#define isInputLanguage(lang)    (boolean)((lang) == File.input.language)
#define isInputHeaderFile()      File.input.isHeader

/*
*   DATA DECLARATIONS
*/

enum eCharacters {
	/*  White space characters.
	 */
	SPACE   = ' ',
	NEWLINE = '\n',
	CRETURN = '\r',
	FORMFEED    = '\f',
	TAB     = '\t',
	VTAB    = '\v',

	/*  Some hard to read characters.
	 */
	DOUBLE_QUOTE  = '"',
	SINGLE_QUOTE  = '\'',
	BACKSLASH     = '\\',

	STRING_SYMBOL = ('S' + 0x80),
	CHAR_SYMBOL   = ('C' + 0x80)
};

/*  Maintains the state of the current input file.
 */
typedef struct sInputFileInfo {
	vString *name;           /* name to report for input file */
	vString *tagPath;        /* path of input file relative to tag file */
	unsigned long lineNumber;/* line number in the input file */
	unsigned long lineNumberOrigin; /* The value set to `lineNumber'
					   when `resetInputFile' is called
					   on the input stream.
					   This is needed for nested stream. */
	boolean  isHeader;       /* is input file a header file? */
	langType language;       /* language of input file */
} inputFileInfo;

typedef struct sInputFile {
	vString *path;      /* path of input file (if any) */
	vString *line;      /* last line read from file */
	const unsigned char* currentLine;   /* current line being worked on */
	MIO     *mio;       /* MIO stream used for reading the file */
	MIOPos  filePosition;   /* file position of current line */
	unsigned int ungetchIdx;
	int     ungetchBuf[3];  /* characters that were ungotten */
	boolean eof;        /* have we reached the end of file? */
	boolean newLine;    /* will the next character begin a new line? */

	/*  Contains data pertaining to the original source file in which the tag
	 *  was defined. This may be different from the input file when #line
	 *  directives are processed (i.e. the input file is preprocessor output).
	 */
	inputFileInfo input; /* name, lineNumber */
	inputFileInfo source;
} inputFile;

/*
*   GLOBAL VARIABLES
*/
/* should not be modified externally */
extern inputFile File;

/*
*   FUNCTION PROTOTYPES
*/

/* InputFile: reading from fp in inputFile with updating fields in input fields */
extern kindOption *getInputLanguageFileKind (void);

extern void freeSourceFileResources (void);
extern boolean fileOpen (const char *const fileName, const langType language);
extern boolean fileEOF (void);
extern void fileClose (void);
extern int getcFromInputFile (void);
extern int getNthPrevCFromInputFile (unsigned int nth, int def);
extern int skipToCharacterInInputFile (int c);
extern void ungetcToInputFile (int c);
extern const unsigned char *readLineFromInputFile (void);
extern char *readLineRaw (vString *const vLine, MIO *const fp);
extern char *readSourceLine (vString *const vLine, MIOPos location, long *const pSeekValue);
extern boolean bufferOpen (unsigned char *buffer, size_t buffer_size,
                           const char *const fileName, const langType language );
#define bufferClose fileClose

#endif  /* _READ_H */

/* vi:set tabstop=4 shiftwidth=4: */
