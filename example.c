
/* example.c */

#include "bg.h"

#define IMGLINK "https://raw.githubusercontent.com/bratpeki/bratpeki.github.io/refs/heads/main/img/xrtd.svg"

int main() {

	BG_ROOT("lang=\"en\"");

	BG_METADATA();
		BG_DOCTITLE("BookGen Example Document");
		BG_STYLE("styles/default-light.css");
	BG_END_METADATA();

	BG_BODY(NULL);

		BG_H(1, "The first chapter header");

			BG_H(2, "Author's Note");

				BG_TXT("This book was generated entirely using ANSI C functions.");

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

					BG_TXT("By tracking ");
					BG_CODE_INLINE("v_bg_depth");
					BG_TXT(" we ensure the HTML source is neatly indented.");

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

			BG_UL(NULL);

				BG_LI("Item 1");
				BG_LI("Item 2");
				BG_LI("Item 3");

				BG_OL(NULL);

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

			BG_TABLE(NULL);

				BG_CAPTION("Supported ANSI C compilers");

				BG_TABLEROW(NULL);
					BG_TH("Compiler");
					BG_TH("Standard");
					BG_TH("Notes");
				BG_END_TABLEROW();

				BG_TABLEROW(NULL);
					BG_TD("GCC");
					BG_TD("C89–C23");
					BG_TD("Most commonly used");
				BG_END_TABLEROW();

				BG_TABLEROW(NULL);
					BG_TD("Clang");
					BG_TD("C89–C23");
					BG_TD("Excellent diagnostics");
				BG_END_TABLEROW();

				BG_TABLEROW(NULL);
					BG_TD("MSVC");
					BG_TD("C89 (partial)");
					BG_TD("Non-standard extensions");
				BG_END_TABLEROW();

			BG_END_TABLE();

		BG_PAGEBREAK();

		BG_TOC();

	BG_END_BODY();

	BG_END_ROOT();

	return EXIT_SUCCESS;

}

