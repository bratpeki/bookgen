
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
 * the user calls BG_* functions, and the resulting HTML is written directly to stdout.
 * However, the library is intended to be so simple that
 * you can easily send the resulting HTML code to a file or elsewhere.
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
 * Enjoy!
 * ==================================================
 * Sections:
 * - Not public API
 *   - INTERNAL STATE AND CONSTANTS
 *   - INTERNAL HELPER FUNCTIONS
 * - Public API
 *   - FUNCTION DECLARATIONS
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
		printf("  ");
}

/* ==================================================
 * FUNCTION DECLARATIONS
 * ==================================================
 * All public API function declarations,
 * generated with the listfunc rule in the Makefile.
 * ================================================== */

static void BG_TAG(const char* inside);
static void BG_TAG_A(const char* inside, const char* attrs);
static void BG_END(const char* inside);
static void BG_VOID(const char* inside);
static void BG_VOID_A(const char* inside, const char* attrs);
static void BG_ROOT(const char* attrs);
static void BG_END_ROOT();
static void BG_METADATA();
static void BG_END_METADATA();
static void BG_BODY(const char* attrs);
static void BG_END_BODY();
static void BG_DOCTITLE(const char* txt);
static void BG_STYLE(const char* path);
static void BG_DEFSTYLE();
static void BG_H(size_t level, const char* title);
static void BG_TOC();
static void BG_TXT(const char* txt);
static void BG_RAW(const char* txt);
static void BG_CODE_BLOCK(const char* txt);
static void BG_CODE_INLINE(const char* txt);
static void BG_LI(const char* txt);
static void BG_UL(const char* attrs);
static void BG_END_UL();
static void BG_OL(const char* attrs);
static void BG_END_OL();
static void BG_TABLE(const char* attrs);
static void BG_END_TABLE();
static void BG_TABLEROW(const char* attrs);
static void BG_END_TABLEROW();
static void BG_TH(const char* txt);
static void BG_TH_A(const char* txt, const char* attrs);
static void BG_TD(const char* txt);
static void BG_TD_A(const char* txt, const char* attrs);
static void BG_CAPTION(const char* txt);
static void BG_IMG(const char* path);
static void BG_IMG_A(const char* path, const char* attrs);
static void BG_FIGCAP(const char* txt);
static void BG_LINEBREAK(size_t howmany);
static void BG_PAGEBREAK();
static void BG_LINK(const char* url, const char* label);
static void BG_QUOTE(const char* quote, const char* author);

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
	printf("<%s>\n", inside);
	v_bg_depth++;
}

/*
 * Emit an opening tag with attributes.
 */
static void BG_TAG_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	printf("<%s %s>\n", inside, attrs);
	v_bg_depth++;
}

/*
 * Emit a closing tag.
 */
static void BG_END(const char* inside)
{
	v_bg_depth--;
	U_BG_INDENT();
	printf("</%s>\n", inside);
}

/*
 * Emit a void tag (e.g. img, br, hr, etc).
 */
static void BG_VOID(const char* inside)
{
	U_BG_INDENT();
	printf("<%s>\n", inside);
}

/*
 * Emit a void tag with attributes.
 */
static void BG_VOID_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	printf("<%s %s>\n", inside, attrs);
}

/* ==================================================
 * DOCUMENT STRUCTURE
 * ==================================================
 * Functions that emit elements that define the document structure.
 * Those include html, head and body.
 * ================================================== */

/*
 * Emit the html (document root) opening tag.
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_ROOT(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("html");
	else
		BG_TAG_A("html", attrs);
}

/*
 * Emit the html (document root) closing tag.
 */
static void BG_END_ROOT()
{
	BG_END("html");
}

/*
 * Emit the head (document metadata) opening tag.
 */
static void BG_METADATA()
{
	BG_TAG("head");
}

/*
 * Emit the head (document metadata) closing tag.
 */
static void BG_END_METADATA()
{
	BG_END("head");
}

/*
 * Emit the body opening tag.
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_BODY(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("body");
	else
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
	printf("<link rel=\"stylesheet\" href=\"%s\">\n", path);
}

/*
 * Emit an inline stylesheet with the default theme.
 * Use this inside the HEAD tag.
 * Uses light mode by default.
 *
 * To use the dark mode, import the library like this:
 * #define V_BG_DARK_MODE
 * #include "bg.h"
 */
static void BG_DEFSTYLE()
{
	#ifndef V_BG_DARK_MODE

		/* Text */
		#define V_BG_COLOR_TEXT_PRIMARY   "#333333"
		#define V_BG_COLOR_TEXT_SECONDARY "#666666"
		#define V_BG_COLOR_TEXT_MUTED     "#888888"

		/* Backgrounds / surfaces */
		#define V_BG_COLOR_BG_PAGE        "#ffffff"
		#define V_BG_COLOR_BG_SUBTLE      "#eeeeee"
		#define V_BG_COLOR_BG_SURFACE     "#f5f5f5"

		/* Borders / separators */
		#define V_BG_COLOR_BORDER_PRIMARY "#cccccc"
		#define V_BG_COLOR_BORDER_ACCENT  "#bbbbbb"

	#else

		#define V_BG_COLOR_TEXT_PRIMARY   "#e6e6e6"
		#define V_BG_COLOR_TEXT_SECONDARY "#b3b3b3"
		#define V_BG_COLOR_TEXT_MUTED     "#9a9a9a"

		#define V_BG_COLOR_BG_PAGE        "#121212"
		#define V_BG_COLOR_BG_SURFACE     "#1e1e1e"
		#define V_BG_COLOR_BG_SUBTLE      "#242424"

		#define V_BG_COLOR_BORDER_PRIMARY "#3a3a3a"
		#define V_BG_COLOR_BORDER_ACCENT  "#4a4a4a"

	#endif

	BG_TAG("style");

	BG_TXT("body {");
	v_bg_depth++;
		BG_TXT("max-width: 800px;");
		BG_TXT("margin: 40px auto;");
		BG_TXT("padding: 0 20px;");
		BG_TXT("color: " V_BG_COLOR_TEXT_PRIMARY ";");
		BG_TXT("background: " V_BG_COLOR_BG_PAGE ";");
		BG_TXT("font-family: serif;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("h1 { border-bottom: 2px solid " V_BG_COLOR_BORDER_PRIMARY "; padding-bottom: 10px; }");

	BG_TXT("code {");
	v_bg_depth++;
		BG_TXT("background: " V_BG_COLOR_BG_SURFACE ";");
		BG_TXT("padding: 2px;");
		BG_TXT("font-family: monospace;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("pre {");
	v_bg_depth++;
		BG_TXT("background: " V_BG_COLOR_BG_SURFACE ";");
		BG_TXT("padding: 15px;");
		BG_TXT("overflow-x: auto;");
		BG_TXT("border-left: 4px solid " V_BG_COLOR_BORDER_ACCENT ";");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("a { text-decoration: underline; color: inherit; }");

	BG_TXT(".toc ul { list-style: none; padding-left: 0; }");
	BG_TXT(".toc a { text-decoration: none; }");

	BG_TXT("li.toc-L1 { font-weight: bold; margin-top: 10px; color: " V_BG_COLOR_TEXT_PRIMARY "; }");
	BG_TXT("li.toc-L2 { padding-left: 20px; font-size: 0.95em; color: " V_BG_COLOR_TEXT_PRIMARY "; }");
	BG_TXT("li.toc-L3 { padding-left: 40px; font-size: 0.9em; color: " V_BG_COLOR_TEXT_SECONDARY "; }");
	BG_TXT("li.toc-L4 { padding-left: 40px; font-size: 0.9em; color: " V_BG_COLOR_TEXT_SECONDARY "; }");
	BG_TXT("li.toc-L5 { padding-left: 50px; font-size: 0.9em; color: " V_BG_COLOR_TEXT_MUTED "; }");
	BG_TXT("li.toc-L6 { padding-left: 60px; font-size: 0.9em; color: " V_BG_COLOR_TEXT_MUTED "; }");

	BG_TXT("table { border-collapse: collapse; width: 100%; margin: 20px 0; }");
	BG_TXT("th, td { border: 1px solid " V_BG_COLOR_BORDER_PRIMARY "; padding: 8px 10px; }");
	BG_TXT("th { background: " V_BG_COLOR_BG_SUBTLE "; font-weight: bold; text-align: left; }");
	BG_TXT("caption { caption-side: bottom; font-size: 0.9em; color: " V_BG_COLOR_TEXT_MUTED "; margin-top: 8px; }");

	BG_TXT("@media print { body { max-width: 100%; margin: 0; } }");

	BG_TXT("blockquote {");
	v_bg_depth++;
		BG_TXT("margin: 1.5em 0;");
		BG_TXT("padding: 0.75em 1.5em;");
		BG_TXT("border-left: 4px solid " V_BG_COLOR_BORDER_ACCENT ";");
		BG_TXT("background: " V_BG_COLOR_BG_SURFACE ";");
		BG_TXT("color: " V_BG_COLOR_TEXT_SECONDARY ";");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("blockquote p {");
	v_bg_depth++;
		BG_TXT("margin: 0;");
		BG_TXT("font-style: italic;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("blockquote footer {");
	v_bg_depth++;
		BG_TXT("margin-top: 0.5em;");
		BG_TXT("font-size: 0.9em;");
		BG_TXT("color: " V_BG_COLOR_TEXT_MUTED ";");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("figcaption {");
	v_bg_depth++;
		BG_TXT("margin-top: 0.5em;");
		BG_TXT("font-size: 0.9em;");
		BG_TXT("color: " V_BG_COLOR_TEXT_MUTED ";");
		BG_TXT("text-align: center;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("figure {");
	v_bg_depth++;
		BG_TXT("margin: 1.5em auto;");
		BG_TXT("text-align: center;");
		BG_TXT("width: fit-content;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("figure img {");
	v_bg_depth++;
		BG_TXT("display: block;");
		BG_TXT("margin: 0 auto;");
	v_bg_depth--;
	BG_TXT("}");

	BG_END("style");

	#undef V_BG_COLOR_TEXT_PRIMARY
	#undef V_BG_COLOR_TEXT_SECONDARY
	#undef V_BG_COLOR_TEXT_MUTED

	#undef V_BG_COLOR_BG_PAGE
	#undef V_BG_COLOR_BG_SUBTLE
	#undef V_BG_COLOR_BG_SURFACE

	#undef V_BG_COLOR_BORDER_PRIMARY
	#undef V_BG_COLOR_BORDER_ACCENT
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

	assert(level >= 1 && level <= 6);

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
	printf(
		"<h%lu id=\"%s\">%s %s</h%lu>\n",
		level, chapterNumBuf,
		chapterNumBuf, title,
		level
	);

	/* Record for TOC */

	/* If this assert is not satisfied, increase V_BG_MAX_TOC */
	assert(v_bg_toc_count < V_BG_MAX_TOC);

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
		printf(
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
	printf("%s\n", txt);
}

/*
 * Emit raw plain text.
 * Nothing is added.
 */
static void BG_RAW(const char* txt)
{
	printf("%s", txt);
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
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_UL(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("ul");
	else
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
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_OL(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("ol");
	else
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
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_TABLE(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("table");
	else
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
 * Attributes are optional; pass NULL or "" if none.
 */
static void BG_TABLEROW(const char* attrs)
{
	if (attrs == NULL || strlen(attrs) == 0)
		BG_TAG("tr");
	else
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
 * Emit a table header cell with attributes (colspan, align, etc).
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
 * Emit a table data cell with attributes (colspan, align, etc).
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
 * Emit an HTML image tag.
 */
static void BG_IMG(const char* path)
{
	U_BG_INDENT();
	printf("<img src=\"%s\">\n", path);
}

/*
 * Emit an HTML image tag with attributes.
 */
static void BG_IMG_A(const char* path, const char* attrs)
{
	U_BG_INDENT();
	printf("<img src=\"%s\" %s>\n", path, attrs);
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
	printf("<div style=\"break-after: page;\"></div>\n");
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
	printf("<a href=\"%s\">%s</a>\n", url, label);
}

/*
 * Emit a quote.
 */
static void BG_QUOTE(const char* quote, const char* author)
{
	BG_TAG("blockquote");

		BG_TAG("p");
			BG_TXT(quote);
		BG_END("p");

		if (author && *author)
		{
			BG_TAG("footer");
				U_BG_INDENT();
				printf("— %s\n", author);
			BG_END("footer");
		}

	BG_END("blockquote");
}

#endif

