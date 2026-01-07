
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
static const char* v_bg_toc_titles[V_BG_MAX_TOC]; /* Stores the pointers to const chapter titles */
static size_t v_bg_toc_levels[V_BG_MAX_TOC]; /* Stores the depth, for the class name */
static char v_bg_toc_numbers[V_BG_MAX_TOC][13]; /* Stores the chapter numbers, like "1.2.3.4.5." */
static size_t v_bg_toc_count = 0;

/* ==================================================
 * NOT FOR THE END USER.
 * ==================================================
 * Util functions.
 * Denoted with a "U".
 * SMALL_SNAKE_CASE, like all the functions used by the user.
 * ================================================== */

void U_BG_INDENT()
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
void BG_TAG(const char* inside)
{
	U_BG_INDENT();
	printf("<%s>\n", inside);
	v_bg_depth++;
}

/*
 * Open a tag and provide attributes.
 */
void BG_TAG_A(const char* inside, const char* attrs)
{
	U_BG_INDENT();
	printf("<%s %s>\n", inside, attrs);
	v_bg_depth++;
}

/*
 * Close a tag.
 */
void BG_END(const char* inside)
{
	v_bg_depth--;
	U_BG_INDENT();
	printf("</%s>\n", inside);
}

/* ==================================================
 * Plaintext
 * ================================================== */

/*
 * Print text.
 */
void BG_TXT(const char* txt)
{
	U_BG_INDENT();
	printf("%s\n", txt);
}

/* ==================================================
 * Linking external files and styling.
 * ================================================== */

/*
 * Link a stylesheet.
 * Use this inside the HEAD tag.
 */
void BG_STYLE(const char* path)
{
	U_BG_INDENT();
	printf("<link rel=\"stylesheet\" href=\"%s\">\n", path);
}

/*
 * Use the default styling.
 * Use this inside the HEAD tag.
 */
void BG_DEFSTYLE()
{
	BG_TAG("style");

	/* Fonts */
	BG_TXT("* { font-family: serif; }");
	BG_TXT("code { font-family: monospace; }");

	/* TOC */
	BG_TXT("li.toc-L1 { padding-left: 10px; }");
	BG_TXT("li.toc-L2 { padding-left: 20px; }");
	BG_TXT("li.toc-L3 { padding-left: 30px; }");
	BG_TXT("li.toc-L4 { padding-left: 40px; }");
	BG_TXT("li.toc-L5 { padding-left: 50px; }");
	BG_TXT("li.toc-L6 { padding-left: 60px; }");

	BG_END("style");
}

/* ==================================================
 * Headings and TOC
 * ================================================== */

/*
 * Write the chapter title, along with the chapter numbering.
 */
void BG_H(size_t level, const char* title)
{
	size_t i;
	char chapterNumBuf[13]; /* 6 digits, 6 dots, null terminator */
	char chapterTmpBuf[3]; /* 1 digit, 1 dot, null terminator */

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

void BG_TOC()
{
	size_t i;
	BG_TAG("div class=\"toc\"");
	/* Header for the TOC itself */
	BG_H(1, "Table of Contents");
	BG_TAG("ul");

	for (i = 0; i < v_bg_toc_count; i++) {
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
 * Common items.
 * ================================================== */

/*
 * Forces a page break when printing.
 */
void BG_PAGEBREAK() {
	U_BG_INDENT();
	printf("<div style=\"break-after: page;\"></div>\n");
}

/*
 * Hyperlink.
 */
void BG_LINK(const char* url, const char* label) {
	U_BG_INDENT();
	printf("<a href=\"%s\">%s</a>\n", url, label);
}

#endif

