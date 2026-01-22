
/* bg.h */

#ifndef BOOKGEN_H
#define BOOKGEN_H

/* ==================================================
 * BookGen
 * ==================================================
 * A minimal, single-header ANSI C library for generating
 * semantic, well-indented HTML documents programmatically.
 *
 * BookGen is very simple.
 * When opened and looked at, it's a collection of functions emitting HTML immediately on call.
 * It is designed around printf-style output:
 * the user calls BG_* functions, and the resulting HTML is written to an output stream,
 * by default stdout.
 *
 * BookGen relies on being passed strings that outlive
 * the HTML generation process in memory,
 * the focus being set on string constants.
 *
 * The library relies on the user having a fundamental understanding of HTML.
 * That includes:
 * - Understanding the common tags like html, head, body, img, br, table, etc.
 * - Understanding the common attributes like width, margin, padding, etc.
 * - Understanding the role of CSS stylesheets in an HTML document.
 *
 * My intention with the library is *generating* documents.
 * While you can totally write your essays and books from scratch in BookGen,
 * it's best to write them in a separate plain text file
 * and move to BookGen for a nice looking export.
 *
 * Enjoy!
 * ==================================================
 * Sections:
 * - Not public API
 *   - INTERNAL STATE AND CONSTANTS
 *   - INTERNAL HELPER FUNCTIONS
 * - Public API
 *   - FUNCTION DECLARATIONS
 *   - INITIALIZATION
 *   - PRIMITIVE FUNCTIONS
 *   - DOCUMENT STRUCTURE
 *   - METADATA
 *   - STYLING
 *   - HEADINGS
 *   - TABLE OF CONTENTS
 *   - PLAIN TEXT
 *   - CODE
 *   - LISTS
 *   - TABLES
 *   - IMAGES
 *   - BREAKING
 *   - PARAGRAPHS
 *   - MISC
 * ==================================================
 * Author (feel free to reach out):
 *   Petar Katić
 *   pkatic2003 (at) gmail.com
 *   https://bratpeki.github.io
 * ================================================== */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ==================================================
 * INTERNAL STATE AND CONSTANTS
 * ==================================================
 * Not part of the public API.
 *
 * Naming conventions:
 * - v_bg_* : internal state variables
 * - V_BG_* : internal configuration constants
 * ================================================== */

/*
 * Maximum number of headers in the document.
 * Used for tracking headers, which is necessary for ToC generation.
 */
#define V_BG_MAX_TOC 100

/*
 * Base64 table used in U_BG_TOBASE64.
 */
static const char V_BG_BASE64_TABLE[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

/*
 * Tracks the current indentation depth for formatted HTML output.
 */
static size_t v_bg_depth;

/*
 * Chapter counters for heading levels h1–h6.
 */
static size_t v_bg_chapter[6] = { 0, 0, 0, 0, 0, 0 };

/*
 * Table of Contents generation state.
 *
 * Includes:
 * - v_bg_toc_titles  : pointers to section titles
 * - v_bg_toc_levels  : heading levels (1–6)
 * - v_bg_toc_numbers : formatted chapter numbers (e.g. "2.2.10.")
 * - v_bg_toc_count   : number of stored TOC entries
 *
 * Invariant:
 * - v_bg_toc_count < V_BG_MAX_TOC
 */
static const char* v_bg_toc_titles[V_BG_MAX_TOC];
static size_t v_bg_toc_levels[V_BG_MAX_TOC];
static char v_bg_toc_numbers[V_BG_MAX_TOC][32];
static size_t v_bg_toc_count = 0;

/*
 * The output stream.
 * If not set, initialized in BG_INIT().
 */
static FILE* v_bg_out = NULL;

/* ==================================================
 * INTERNAL HELPER FUNCTIONS
 * ==================================================
 * Not part of the public API.
 *
 * Naming conventions:
 * - U_BG_* : Helper functions ("U" implying "utility")
 * ================================================== */

/*
 * Emits indentation spaces based on the current output depth.
 */
static void U_BG_INDENT()
{
	size_t i;
	for (i = 0; i < v_bg_depth; i++)
		fprintf(v_bg_out, "  ");
}

/*
 * Emit the contents of a file straight to the output stream.
 *
 * This function does not buffer.
 * It streams the file directly to the output stream.
 * If the file cannot be opened, no output is produced.
 */
static void U_BG_READFILE(const char* path)
{
	FILE *f;
	int ch;

	f = fopen(path, "r");
	if (f == NULL) return;

	while ((ch = getc(f)) != EOF) {
		fputc(ch, v_bg_out);
	}

	fclose(f);
}

/*
 * Stream a file as Base64 directly to the output stream.
 *
 * This function does not buffer.
 * It streams the file directly to the output stream.
 * If the file cannot be opened, no output is produced.
 */
static void U_BG_TOBASE64(const char* path)
{
	FILE* f;
	size_t n;

	/*
	 * 64 = 2^6, so one Base64 character represents 6 bits.
	 * Conversion: 3 input bytes (3*8 = 24 bits) is 4 Base64 characters (4*6 = 24 bits)
	 */
	unsigned char in[3], out[4];

	f = fopen(path, "rb");
	if (f == NULL) return;

	while ((n = fread(in, 1, 3, f)) > 0) {

		/*
		 * First 6 bits of the first byte.
		 */
		out[0] = V_BG_BASE64_TABLE[ in[0] >> 2 ];

		/*
		 * Last 2 bits of the first byte (in[0] & 0x03), shifted to high bits
		 * and first 4 bits of the second byte (in[1] >> 4).
		 * If the second byte doesn't exist (n == 1), we treat it as 0.
		 */
		out[1] = V_BG_BASE64_TABLE[ ((in[0] & 0x03) << 4) | ((n > 1 ? in[1] : 0) >> 4) ];

		/*
		 * Last 4 bits of the second byte (in[1] & 0x0F), shifted to high bits
		 * and first 2 bits of the third byte (in[2] >> 6).
		 * If the second byte doesn't exist (n == 1), output '=' as padding.
		 * If the third byte doesn't exist (n == 2), treat it as 0 for calculation.
		 */
		out[2] = (n > 1)
			? V_BG_BASE64_TABLE[ ((in[1] & 0x0F) << 2) | ((n > 2 ? in[2] : 0) >> 6) ]
			: '=';

		/*
		 * Last 6 bits of the third byte.
		 * If the third byte doesn't exist (n < 3), output '=' as padding.
		 */
		out[3] = (n > 2)
			? V_BG_BASE64_TABLE[ in[2] & 0x3F ]
			: '=';

		fwrite(out, 1, 4, v_bg_out);

	}

	fclose(f);
}

/* ==================================================
 * FUNCTION DECLARATIONS
 * ==================================================
 * All public API function declarations,
 * generated with the listfunc rule in the Makefile.
 * ================================================== */

static void BG_INIT();
static void BG_INIT_FILE(FILE* where);
static void BG_TAG(const char* inside);
static void BG_TAG_A(const char* inside, const char* attrs);
static void BG_END(const char* inside);
static void BG_VOID(const char* inside);
static void BG_VOID_A(const char* inside, const char* attrs);
static void BG_HTML();
static void BG_HTML_A(const char* attrs);
static void BG_END_HTML();
static void BG_HEAD();
static void BG_END_HEAD();
static void BG_BODY();
static void BG_BODY_A(const char* attrs);
static void BG_END_BODY();
static void BG_DOCTITLE(const char* txt);
static void BG_STYLE(const char* path);
static void BG_STYLE_INLINE(const char* path);
static void BG_H(size_t level, const char* title);
static void BG_TOC();
static void BG_TXT(const char* txt);
static void BG_RAW(const char* txt);
static void BG_CODE_BLOCK(const char* txt);
static void BG_CODE_INLINE(const char* txt);
static void BG_LI(const char* txt);
static void BG_UL();
static void BG_UL_A(const char* attrs);
static void BG_END_UL();
static void BG_OL();
static void BG_OL_A(const char* attrs);
static void BG_END_OL();
static void BG_TABLE();
static void BG_TABLE_A(const char* attrs);
static void BG_END_TABLE();
static void BG_TABLEROW();
static void BG_TABLEROW_A(const char* attrs);
static void BG_END_TABLEROW();
static void BG_TH(const char* txt);
static void BG_TH_A(const char* txt, const char* attrs);
static void BG_TD(const char* txt);
static void BG_TD_A(const char* txt, const char* attrs);
static void BG_CAPTION(const char* txt);
static void BG_IMG(const char* path);
static void BG_IMG_A(const char* path, const char* attrs);
static void BG_IMG_INLINE(const char* mime, const char* path);
static void BG_IMG_INLINE_A(const char* mime, const char* path, const char* attrs);
static void BG_FIGCAP(const char* txt);
static void BG_LINEBREAK(size_t howmany);
static void BG_PAGEBREAK();
static void BG_P();
static void BG_P_A(const char* attrs);
static void BG_END_P();
static void BG_LINK(const char* url, const char* label);
static void BG_QUOTE(const char* quote, const char* author);

/* ==================================================
 * INITIALIZATION
 * ==================================================
 * Functions that initialize the library.
 * ================================================== */

/*
 * Initializes the library and directs the output to stdout.
 *
 * Actually, it sets full buffering and a buffer size of 1MB (1 << 20).
 * This reduces the number of write syscalls caused by many fprintf calls.
 */
static void BG_INIT()
{
	if (!v_bg_out) v_bg_out = stdout;
	setvbuf(v_bg_out, NULL, _IOFBF, 1 << 20);
}

/*
 * Initializes the library and directs the output to a custom stream.
 *
 * This function runs BG_INIT(), so the logic is the same,
 * just with a custom output stream rather than the default stdout.
 */
static void BG_INIT_FILE(FILE* where)
{
	/* This is added specifically to combat someone setting "where" to NULL */
	v_bg_out = where ? where : stdout;
	BG_INIT();
}

/* ==================================================
 * PRIMITIVE FUNCTIONS
 * ==================================================
 * Functions that emit HTML:
 * - Opening and closing tags
 * - Void tags
 * ================================================== */

/*
 * Emit an opening tag.
 *
 * This function also allows attributes to be embedded in the tag string
 * (e.g. BG_TAG("h1 color=\"red\"")), but BG_TAG_A is preferred.
 */
static void BG_TAG(const char* inside)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<%s>\n", inside);
	v_bg_depth++;
}

/*
 * Emit an opening tag, with attributes.
 */
static void BG_TAG_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<%s %s>\n", inside, attrs);
	v_bg_depth++;
}

/*
 * Emit a closing tag.
 */
static void BG_END(const char* inside)
{
	v_bg_depth--;
	U_BG_INDENT();
	fprintf(v_bg_out, "</%s>\n", inside);
}

/*
 * Emit a void tag (e.g. img, br, hr, etc).
 */
static void BG_VOID(const char* inside)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<%s>\n", inside);
}

/*
 * Emit a void tag, with attributes.
 */
static void BG_VOID_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<%s %s>\n", inside, attrs);
}

/* ==================================================
 * DOCUMENT STRUCTURE
 * ==================================================
 * Functions that emit elements that define the document structure.
 * Those include html, head and body.
 * ================================================== */

/*
 * Emit the html (document root) opening tag.
 */
static void BG_HTML()
{
	BG_TAG("html");
}

/*
 * Emit the html (document root) opening tag, with attributes.
 */
static void BG_HTML_A(const char* attrs)
{
	BG_TAG_A("html", attrs);
}

/*
 * Emit the html (document root) closing tag.
 */
static void BG_END_HTML()
{
	BG_END("html");
}

/*
 * Emit the head (document metadata) opening tag.
 */
static void BG_HEAD()
{
	BG_TAG("head");
}

/*
 * Emit the head (document metadata) closing tag.
 */
static void BG_END_HEAD()
{
	BG_END("head");
}

/*
 * Emit the body opening tag.
 */
static void BG_BODY()
{
	BG_TAG("body");
}

/*
 * Emit the body opening tag, with attributes.
 */
static void BG_BODY_A(const char* attrs)
{
	BG_TAG_A("body", attrs);
}

/*
 * Emit the body closing tag.
 */
static void BG_END_BODY()
{
	BG_END("body");
}

/* ==================================================
 * METADATA
 * ==================================================
 * Functions that emit elements intended for the head tag.
 * ================================================== */

/*
 * Emit the HTML document title.
 */
static void BG_DOCTITLE(const char* txt)
{
	BG_TAG("title");
	BG_TXT(txt);
	BG_END("title");
}

/* ==================================================
 * STYLING
 * ==================================================
 * Functions that emit CSS styling information.
 * ================================================== */

/*
 * Emit a stylesheet link element.
 * Use this inside the HEAD tag.
 */
static void BG_STYLE(const char* path)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<link rel=\"stylesheet\" href=\"%s\">\n", path);
}

/*
 * Emit a style element, containing the stylesheet in full.
 * Use this inside the HEAD tag.
 */
static void BG_STYLE_INLINE(const char* path)
{
	BG_TAG("style");
	U_BG_READFILE(path);
	BG_END("style");
}

/* ==================================================
 * HEADINGS
 * ==================================================
 * Functions that emit chapter headings.
 * ================================================== */

/*
 * Emit a header at the given level with the given title.
 *
 * Invariant:
 * 1 <= level <= 6
 */
static void BG_H(size_t level, const char* title)
{
	size_t i;
	char chapterNumBuf[32];
	char chapterTmpBuf[8];

	/* We allow h1 through h6 */

	assert(
		(level >= 1 && level <= 6) &&
		"The header levels are clamped between 1 and 6!"
	);

	if (level >= 2)
		assert(
			(v_bg_chapter[level-2] != 0) &&
			"Cannot jump header levels!"
		);

	/* Increment current level */

	v_bg_chapter[level - 1]++;

	/* Reset all sub-levels */

	for ( i = level; i < 6; i++ ) {
		v_bg_chapter[i] = 0;
	}

	/*
	 * Constructing the chapter number, e.g. 1.2.3.4.
	 * The number is stored in chapterBuf
	 */

	chapterNumBuf[0] = '\0';

	for (i = 0; i < level; i++) {
		sprintf(chapterTmpBuf, "%lu.", v_bg_chapter[i]);
		strcat(chapterNumBuf, chapterTmpBuf);
	}

	/* Printing */

	U_BG_INDENT();
	fprintf(v_bg_out,
		"<h%lu id=\"%s\">%s %s</h%lu>\n",
		level, chapterNumBuf,
		chapterNumBuf, title,
		level
	);

	/* Record for TOC */

	assert(
		(v_bg_toc_count < V_BG_MAX_TOC) &&
		"Too many headers defined! Increase V_BG_MAX_TOC."
	);

	/* Storing the chapter number */

	v_bg_toc_titles[v_bg_toc_count] = title;
	v_bg_toc_levels[v_bg_toc_count] = level;
	strcpy(v_bg_toc_numbers[v_bg_toc_count], chapterNumBuf);
	v_bg_toc_count++;
}

/* ==================================================
 * TABLE OF CONTENTS
 * ==================================================
 * Functions that handle the ToC.
 * ================================================== */

/*
 * Emit the ToC.
 * Use at the end of the document.
 */
static void BG_TOC()
{
	size_t i;
	BG_TAG_A("div", "class=\"toc\"");
	BG_H(1, "Table of Contents");
	BG_TAG("ul");

	/* The "-1" omits the TOC header from the TOC */
	for (i = 0; i < v_bg_toc_count - 1; i++) {
		U_BG_INDENT();
		/* The class name is added for styling (toc-L1, toc-L2, etc) */
		fprintf(v_bg_out,
			"<li class=\"toc-L%lu\"><a href=\"#%s\">%s %s</a></li>\n",
			v_bg_toc_levels[i],
			v_bg_toc_numbers[i],
			v_bg_toc_numbers[i], v_bg_toc_titles[i]
		);
	}

	BG_END("ul");
	BG_END("div");
}

/* ==================================================
 * PLAIN TEXT
 * ==================================================
 * Functions that emit plain text content.
 * ================================================== */

/*
 * Emit formatted plain text.
 * Indents the text and adds a newline.
 */
static void BG_TXT(const char* txt)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "%s\n", txt);
}

/*
 * Emit raw plain text.
 * Nothing is added.
 */
static void BG_RAW(const char* txt)
{
	fprintf(v_bg_out, "%s", txt);
}

/* ==================================================
 * CODE
 * ==================================================
 * Function that emit code-realted items.
 * ================================================== */

/*
 * Emit a multiline code block.
 * Remember to use HTML escape codes for symbols like <, >, etc.
 */
static void BG_CODE_BLOCK(const char* txt)
{
	U_BG_INDENT();
	BG_RAW("<pre>");
	BG_RAW(txt);
	BG_RAW("</pre>\n");
}

/*
 * Emit an inline code block.
 * Remember to use HTML escape codes for symbols like <, >, etc.
 */
static void BG_CODE_INLINE(const char* txt)
{
	BG_TAG("code");
	BG_TXT(txt);
	BG_END("code");
}

/* ==================================================
 * LISTS
 * ==================================================
 * Functions that handle lists.
 * ================================================== */

/*
 * Emit a list item.
 */
static void BG_LI(const char* txt)
{
	BG_TAG("li");
	BG_TXT(txt);
	BG_END("li");
}

/*
 * Emit an opening unordered list tag (<ul>).
 */
static void BG_UL()
{
	BG_TAG("ul");
}

/*
 * Emit an opening unordered list tag (<ul>), with attributes.
 */
static void BG_UL_A(const char* attrs)
{
	BG_TAG_A("ul", attrs);
}

/*
 * Emit a closing unordered list tag (</ul>).
 */
static void BG_END_UL()
{
	BG_END("ul");
}

/*
 * Emit an opening ordered list tag (<ol>).
 */
static void BG_OL()
{
	BG_TAG("ol");
}

/*
 * Emit an opening ordered list tag (<ol>), with attributes.
 */
static void BG_OL_A(const char* attrs)
{
	BG_TAG_A("ol", attrs);
}

/*
 * Emit a closing ordered list tag (</ol>).
 */
static void BG_END_OL()
{
	BG_END("ol");
}

/* ==================================================
 * TABLES
 * ==================================================
 * Functions that handle tables.
 * ================================================== */

/*
 * Emit a table opening tag.
 */
static void BG_TABLE()
{
	BG_TAG("table");
}

/*
 * Emit a table opening tag, with attributes.
 */
static void BG_TABLE_A(const char* attrs)
{
	BG_TAG_A("table", attrs);
}

/*
 * Emit a table closing tag.
 */
static void BG_END_TABLE()
{
	BG_END("table");
}

/*
 * Emit a table row opening tag.
 */
static void BG_TABLEROW()
{
	BG_TAG("tr");
}

/*
 * Emit a table row opening tag, with attributes.
 */
static void BG_TABLEROW_A(const char* attrs)
{
	BG_TAG_A("tr", attrs);
}

/*
 * Emit a table row closing tag.
 */
static void BG_END_TABLEROW()
{
	BG_END("tr");
}

/*
 * Emit a table header cell.
 */
static void BG_TH(const char* txt)
{
	BG_TAG("th");
	BG_TXT(txt);
	BG_END("th");
}

/*
 * Emit a table header cell, with attributes (colspan, align, etc).
 */
static void BG_TH_A(const char* txt, const char* attrs)
{
	BG_TAG_A("th", attrs);
	BG_TXT(txt);
	BG_END("th");
}

/*
 * Emit a table data cell.
 */
static void BG_TD(const char* txt)
{
	BG_TAG("td");
	BG_TXT(txt);
	BG_END("td");
}

/*
 * Emit a table data cell, with attributes (colspan, align, etc).
 */
static void BG_TD_A(const char* txt, const char* attrs)
{
	BG_TAG_A("td", attrs);
	BG_TXT(txt);
	BG_END("td");
}

/*
 * Emit the table caption.
 */
static void BG_CAPTION(const char* txt)
{
	BG_TAG("caption");
	BG_TXT(txt);
	BG_END("caption");
}

/* ==================================================
 * IMAGES
 * ==================================================
 * Functions for emitting image-related items.
 * That includes:
 * - Images
 * - Figures and figure captions
 * ================================================== */

/*
 * Emit an HTML image tag with a linked image.
 *
 * The path can be a local or web URL.
 */
static void BG_IMG(const char* path)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<img src=\"%s\">\n", path);
}

/*
 * Emit an HTML image tag with a linked image, with attributes.
 *
 * The path can be a local or web URL.
 */
static void BG_IMG_A(const char* path, const char* attrs)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<img src=\"%s\" %s>\n", path, attrs);
}

/*
 * Emit an HTML image tag with the image as Base64 data.
 *
 * The path can only be a local URL, not a web one, since it's read with fread.
 * Inlining images increases HTML size significantly.
 */
static void BG_IMG_INLINE(const char* mime, const char* path)
{
	fprintf(v_bg_out, "<img src=\"data:%s;base64,", mime);
	U_BG_TOBASE64(path);
	fprintf(v_bg_out, "\">\n");
}

/*
 * Emit an HTML image tag with the image as Base64 data, with attributes.
 *
 * The path can only be a local URL, not a web one, since it's read with fread.
 * Inlining images increases HTML size significantly.
 */
static void BG_IMG_INLINE_A(const char* mime, const char* path, const char* attrs)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<img %s src=\"data:%s;base64,", attrs, mime);
	U_BG_TOBASE64(path);
	fprintf(v_bg_out, "\">\n");
}

/*
 * Emit a figure caption.
 */
static void BG_FIGCAP(const char* txt)
{
	BG_TAG("figcaption");
	BG_TXT(txt);
	BG_END("figcaption");
}

/* ==================================================
 * BREAKING
 * ==================================================
 * Function that emit breaking items.
 * ================================================== */

/*
 * Emits a variable number of line breaks.
 */
static void BG_LINEBREAK(size_t howmany)
{
	size_t i;
	for (i = 0; i < howmany; i++) {
		BG_VOID("br");
	}
}

/*
 * Emits a page break.
 */
static void BG_PAGEBREAK()
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<div style=\"break-after: page;\"></div>\n");
}

/* ==================================================
 * PARAGRAPHS
 * ==================================================
 * Function that emit paragraph tags.
 * ================================================== */

/*
 * Emit a paragraph opening tag.
 */
static void BG_P()
{
	BG_TAG("p");
}

/*
 * Emit a paragraph opening tag, with attributes.
 */
static void BG_P_A(const char* attrs)
{
	BG_TAG_A("p", attrs);
}

/*
 * Emit a paragraph closing tag.
 */
static void BG_END_P()
{
	BG_END("p");
}

/* ==================================================
 * MISC
 * ==================================================
 * Function that emit various common items.
 * ================================================== */

/*
 * Emit a hyperlink.
 */
static void BG_LINK(const char* url, const char* label)
{
	U_BG_INDENT();
	fprintf(v_bg_out, "<a href=\"%s\">%s</a>\n", url, label);
}

/*
 * Emit a quote.
 */
static void BG_QUOTE(const char* quote, const char* author)
{
	BG_TAG("blockquote");

		BG_P();
			BG_TXT(quote);
		BG_END_P();

		if (author && *author)
		{
			BG_TAG("footer");
				U_BG_INDENT();
				fprintf(v_bg_out, "&mdash; %s\n", author);
			BG_END("footer");
		}

	BG_END("blockquote");
}

#endif

