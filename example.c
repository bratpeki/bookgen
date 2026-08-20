
/* example.c */

#include "bg.h"

#include <stdio.h>
#include <stdlib.h>

#define IMGLINK "https://raw.githubusercontent.com/bratpeki/bratpeki.github.io/refs/heads/main/img/xrtd.svg"

/*
 * We're generating examples in both the provided light and dark themes.
 * They consist of the exact same content, just different styles.
 * This function emits that content.
 */
void printExample(FILE* f, const char* theme) {

	char buffer[5000];

	BG_INIT_FILE(f);

	BG_HTML();

	BG_HEAD();
		BG_VOID_A("meta", "charset=\"UTF-8\"");
		BG_DOCTITLE("BookGen Example Document");

		/* We use both the base styling and the passed theme color */
		BG_STYLE("../styles/default-base.css");
		BG_STYLE_INLINE(theme);

		BG_STYLE_PRINT();
	BG_END_HEAD();

	BG_BODY_PRINT();

		BG_H(1, "The first chapter header");

			BG_H(2, "Author's Note");

				BG_TXT("This document was generated entirely using ANSI C functions.");

		BG_H(1, "The second chapter header");

			BG_H(2, "Why ANSI C?");

				BG_TXT("Honestly, simplicity!");
				BG_LINEBREAK(2);
				BG_TXT(
					"I like knowing my software can be compiled on as many architectures as possible, "
					"so ANSI C <i>felt</i> like a natural pick."
				);

			BG_H(2, "The indentation engine");

				BG_H(3, "The <code>v_bg_depth</code> variable");

					BG_TXT("By tracking");
					BG_CODE_INLINE("v_bg_depth");
					BG_TXT("we ensure the HTML source is neatly indented.");

				BG_H(3, "The heading logic");

					BG_TXT("Notice how the numbers below are generated automatically.");

					BG_H(4, "Specific Case A");

						BG_TXT(
							"Naturally, since <code>BG_TXT</code> is just text, <i><b>you can inject HTML</b></i>! "
							"That means you can link stuff like <a href=\"https://www.google.com\">this</a>!"
						);

					BG_H(4, "Specific Case B");

						BG_TXT("Of course, though, there's <code>BG_LINK</code>.");
						BG_LINK("https://www.google.com", "Here it is in action.");

						BG_QUOTE("I am quoting myself.", "Peki");

	BG_PAGEBREAK();

			BG_H(2, "Code blocks");

				BG_TXT(
					"For longer examples, use <code>BG_CODE_BLOCK</code>. "
					"Whitespace and newlines are preserved exactly as written."
				);

				BG_CODE_BLOCK(
					"#include &lt;stdio.h&gt;\n"
					"#include &lt;stdlib.h&gt;\n"
					"\n"
					"int main(void)\n"
					"{\n"
					"  printf(\"Hello from ANSI C!\\n\");\n"
					"  return EXIT_SUCCESS;\n"
					"}"
				);

			BG_H(2, "Working with lists");

				BG_UL();

					BG_LI("Item 1");
					BG_LI("Item 2");
					BG_LI("Item 3");

					BG_OL();

						BG_LI("Subitem 1");
						BG_LI("Subitem 2");
						BG_LI("Subitem 3");

					BG_END_OL();

					BG_LI("Item 4");

				BG_END_UL();

	BG_PAGEBREAK();

			BG_H(2, "Images!");

				BG_TAG("figure");
					BG_IMG_A(IMGLINK, "width=\"250px\"");
					BG_FIGCAP("My music logo (read about my music <a href=\"https://bratpeki.github.io/markup/music.html\">here</a>)!");
				BG_END("figure");

			BG_H(2, "A simple table");

				BG_TABLE();

					BG_CAPTION("Declarations in BookGen");

					BG_TABLEROW();
						BG_TH("Prefix");
						BG_TH("Example");
						BG_TH("Public?");
					BG_END_TABLEROW();

					BG_TABLEROW();
						BG_TD("<code>v_bg_*</code> and <code>V_BG_*</code>");
						BG_TD("<code>v_bg_depth</code>, <code>V_BG_BASE64_TABLE</code>");
						BG_TD("No. Used internally to track state (depth of HTML indentation, chapter numbers, and so on).");
					BG_END_TABLEROW();

					BG_TABLEROW();
						BG_TD("<code>U_BG_*</code>");
						BG_TD("<code>U_BG_READFILE</code>, <code>U_BG_TOBASE64</code>");
						BG_TD("No. Used internally as helper methods.");
					BG_END_TABLEROW();

					BG_TABLEROW();
						BG_TD("<code>BG_*</code>");
						BG_TD("<code>BG_TAG</code>, <code>BG_END</code>");
						BG_TD("Yes. Part of the public API.");
					BG_END_TABLEROW();

				BG_END_TABLE();

	BG_PAGEBREAK();

			BG_H(2, "External file contents");

				BG_TXT("Here's our <code>Makefile</code>, included just because:");
				BG_READFILE_B("./Makefile", buffer);
				BG_CODE_BLOCK(buffer);

	BG_PAGEBREAK();

	/* To include all header levels in the ToC, use either BG_TOC(0) or BG_TOC(6). */
	BG_TOC(3);

	BG_END_BODY_PRINT();

	BG_END_HTML();

}

int main() {

	FILE *outLight, *outDark;

	outLight = fopen("./out/example-light.html", "w");
	if (!outLight) return EXIT_FAILURE;
	printExample(outLight, "./styles/default-light.css");
	fclose(outLight);

	outDark = fopen("./out/example-dark.html", "w");
	if (!outDark) return EXIT_FAILURE;
	printExample(outDark, "./styles/default-dark.css");
	fclose(outDark);

	return EXIT_SUCCESS;

}

