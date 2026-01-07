
/* bg.h */

#ifndef BOOKGEN_H
#define BOOKGEN_H

/* ==================================================
 * The ANSI C single-header library for generating HTML.
 * I envisioned using printf and then piping the output to a file.
 * However, the library is intended to be modular and extensible.
 * ================================================== */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ==================================================
 * NOT FOR THE END USER.
 * ==================================================
 * Variables and macros.
 * Denoted with a "V".
 * small_snake_case for variables and SCREAMING_SNAKE_CASE for macros.
 * ================================================== */

#define V_BG_MAX_TOC 100

/*
 * Tracks the depth for a nice, formatted output.
 */
static size_t v_bg_depth;

/*
 * These count the chapter number at each heading level.
 */
static size_t v_bg_chapter[6] = { 0, 0, 0, 0, 0, 0 };

/*
 * These are used to generate the TOC.
 */
static const char* v_bg_toc_titles[V_BG_MAX_TOC]; /* Stores the pointers to chapter titles */
static size_t v_bg_toc_levels[V_BG_MAX_TOC]; /* Stores the depth, for the class name */
static char v_bg_toc_numbers[V_BG_MAX_TOC][32]; /* Stores the chapter numbers, like "2.2.10." */
static size_t v_bg_toc_count = 0; /* Used for tracking ToC entires. Mustn't exceed V_BG_MAX_TOC. */

/* ==================================================
 * NOT FOR THE END USER.
 * ==================================================
 * Util functions.
 * Denoted with a "U".
 * SMALL_SNAKE_CASE, like all the functions used by the user.
 * ================================================== */

static void U_BG_INDENT()
{
	size_t i;
	for (
		i = 0;
		i < v_bg_depth;
		i++
	) printf("  ");
}

/* ==================================================
 * Fundamental functions.
 * ================================================== */

/*
 * Open a tag.
 *
 * This also handles attributes, ex. BG_TAG("h1 color=\"red\""),
 * but it'd be better to use BG_TAG_A for that.
 */
static void BG_TAG(const char* inside)
{
	U_BG_INDENT();
	printf("<%s>\n", inside);
	v_bg_depth++;
}

/*
 * Open a tag and provide attributes.
 */
static void BG_TAG_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	printf("<%s %s>\n", inside, attrs);
	v_bg_depth++;
}

/*
 * Close a tag.
 */
static void BG_END(const char* inside)
{
	v_bg_depth--;
	U_BG_INDENT();
	printf("</%s>\n", inside);
}

/*
 * Print a void element (img, br, hr, etc).
 */
static void BG_VOID(const char* tag)
{
	U_BG_INDENT();
	printf("<%s>\n", tag);
}

/*
 * Print a void element (img, br, hr, etc) and provide attributes.
 */
static void BG_VOID_A(const char* tag, const char* attrs)
{
	U_BG_INDENT();
	printf("<%s %s>\n", tag, attrs);
}

/* ==================================================
 * Plaintext
 * ================================================== */

/*
 * Print text.
 * Indents the text and adds a newline.
 */
static void BG_TXT(const char* txt)
{
	U_BG_INDENT();
	printf("%s\n", txt);
}

/*
 * Print raw text.
 * Nothing is added.
 */
static void BG_RAW(const char* txt)
{
	printf("%s", txt);
}

/* ==================================================
 * Linking external files and styling.
 * ================================================== */

/*
 * Link a stylesheet.
 * Use this inside the HEAD tag.
 */
static void BG_STYLE(const char* path)
{
	U_BG_INDENT();
	printf("<link rel=\"stylesheet\" href=\"%s\">\n", path);
}

/*
 * Use the default styling.
 * Use this inside the HEAD tag.
 */
static void BG_DEFSTYLE()
{
	BG_TAG("style");

	BG_TXT("body {");
	v_bg_depth++;
	BG_TXT("max-width: 800px;"); /* Keeps the book centered and readable */
	BG_TXT("margin: 40px auto;"); /* Centers the content on the screen */
	BG_TXT("padding: 0 20px;"); /* Prevents text touching mobile edges */
	BG_TXT("color: #333;"); /* Soften the black for less eye strain */
	BG_TXT("font-family: serif;");
	v_bg_depth--;
	BG_TXT("}");

	BG_TXT("h1, h2, h3 { color: #111; }");
	BG_TXT("h1 { border-bottom: 2px solid #eee; padding-bottom: 10px; }");

	BG_TXT("code { background: #f4f4f4; padding: 2px 5px; border-radius: 3px; font-family: monospace; }");
	BG_TXT("pre { background: #f4f4f4; padding: 15px; overflow-x: auto; border-left: 4px solid #ccc; }");

	BG_TXT(".toc ul { list-style: none; padding-left: 0; }");
	BG_TXT(".toc a { text-decoration: none; color: #007acc; }");
	BG_TXT(".toc a:hover { text-decoration: underline; }");

	BG_TXT("li.toc-L1 { font-weight: bold; margin-top: 10px; }");
	BG_TXT("li.toc-L2 { padding-left: 20px; font-weight: normal; font-size: 0.95em; }");
	BG_TXT("li.toc-L3 { padding-left: 40px; font-size: 0.9em; color: #666; }");

	BG_TXT("li.toc-L4 { padding-left: 40px; font-size: 0.9em; color: #666; }");
	BG_TXT("li.toc-L5 { padding-left: 50px; font-size: 0.9em; color: #666; }");
	BG_TXT("li.toc-L6 { padding-left: 60px; font-size: 0.9em; color: #666; }");

	BG_TXT("table { border-collapse: collapse; width: 100%; margin: 20px 0; }");
	BG_TXT("th, td { border: 1px solid #ddd; padding: 8px 10px; }");
	BG_TXT("th { background: #f7f7f7; font-weight: bold; text-align: left; }");
	BG_TXT("caption { caption-side: bottom; font-size: 0.9em; color: #666; margin-top: 8px; }");

	BG_TXT("@media print { body { max-width: 100%; margin: 0; } .toc { border: none; } }");

	BG_END("style");
}

/* ==================================================
 * Headings and TOC
 * ================================================== */

/*
 * Write the chapter title, along with the chapter numbering.
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
	 * Constructing the chapter number, ex. 1.2.3.4.
	 * The number is stored in chapterBuf
	 */

	chapterNumBuf[0] = '\0';

	for (i = 0; i < level; i++) {
		sprintf(chapterTmpBuf, "%lu.", v_bg_chapter[i]);
		strcat(chapterNumBuf, chapterTmpBuf);
	}

	/* Printing */

	U_BG_INDENT();
	printf("<h%lu>%s %s </h%lu>\n", level, chapterNumBuf, title, level);

	/* Record for TOC */

	/* If this assert is not satisfied, increase V_BG_MAX_TOC */
	assert(v_bg_toc_count < V_BG_MAX_TOC);

	/* Storing the chapter number */

	v_bg_toc_titles[v_bg_toc_count] = title;
	v_bg_toc_levels[v_bg_toc_count] = level;
	strcpy(v_bg_toc_numbers[v_bg_toc_count], chapterNumBuf);
	v_bg_toc_count++;
}

/*
 * Print the Table of Contents.
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
			"<li class=\"toc-L%lu\">%s %s</li>\n",
			v_bg_toc_levels[i],
			v_bg_toc_numbers[i],
			v_bg_toc_titles[i]
		);
	}

	BG_END("ul");
	BG_END("div");
}

/* ==================================================
 * Tables.
 * ================================================== */

/*
 * Print one table header cell.
 */
static void BG_TH(const char* txt)
{
	BG_TAG("th");
	BG_TXT(txt);
	BG_END("th");
}

/*
 * Print one table data cell.
 */
static void BG_TD(const char* txt)
{
	BG_TAG("td");
	BG_TXT(txt);
	BG_END("td");
}

/*
 * Print the table's caption.
 */
static void BG_CAPTION(const char* txt)
{
	BG_TAG("caption");
	BG_TXT(txt);
	BG_END("caption");
}

/*
 * Print one table header cell, with attributes (colspan, align, etc).
 */
static void BG_TH_A(const char* attrs, const char* txt)
{
	BG_TAG_A("th", attrs);
	BG_TXT(txt);
	BG_END("th");
}

/*
 * Print one table data cell, with attributes (colspan, align, etc).
 */
static void BG_TD_A(const char* attrs, const char* txt)
{
	BG_TAG_A("td", attrs);
	BG_TXT(txt);
	BG_END("td");
}

/* ==================================================
 * Common items.
 * ================================================== */

/*
 * Forces a page break when printing.
 */
static void BG_PAGEBREAK()
{
	U_BG_INDENT();
	printf("<div style=\"break-after: page;\"></div>\n");
}

/*
 * Make a hyperlink.
 */
static void BG_LINK(const char* url, const char* label)
{
	U_BG_INDENT();
	printf("<a href=\"%s\">%s</a>\n", url, label);
}

/*
 * Make a variable number of line breaks.
 */
static void BG_BR(size_t howmany)
{
	size_t i;
	for (i = 0; i < howmany; i++) {
		BG_VOID("br");
	}
}

#endif

