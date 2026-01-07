
/* example.c */

#include "bg.h"

int main() {

	BG_TAG_A("html", "lang=\"en\"");

	BG_TAG("head");
		BG_TAG("title");
			BG_TXT("BookGen Example Document");
		BG_END("title");
		BG_STYLE("style.css");
		BG_DEFSTYLE();
	BG_END("head");

	BG_TAG("body");

		BG_H(1, "The first chapter header");

			BG_TAG_A("div", "class=\"preface\"");
				BG_H(2, "Author's Note");
				BG_TXT("This book was generated entirely using ANSI C functions.");
			BG_END("div");

		BG_PAGEBREAK();

		BG_H(1, "The second chapter header");

			BG_H(2, "Why ANSI C?");
			BG_TAG("p");
				BG_TXT("Honestly, simplicity!");
				BG_TXT(
					"I like knowing my software can be compiled on as many architectures as possible, "
					"so ANSI C <i>felt</i> like a natural pick."
				);
			BG_END("p");

			BG_H(2, "The indentation engine");

				BG_H(3, "The <code>v_bg_depth</code> variable");
				BG_TAG("p");
					BG_TXT("By tracking");
					BG_TAG("code");
						BG_TXT("v_bg_depth");
					BG_END("code");
					BG_TXT("we ensure the HTML source is neatly indented.");
				BG_END("p");

				BG_H(3, "The heading logic");

					BG_TAG("p");
						BG_TXT("Notice how the numbers below are generated automatically.");
					BG_END("p");

					BG_H(4, "Specific Case A");

						BG_TXT(
							"Naturally, since <code>BG_TXT</code> is just text, <u><b>you can inject HTML</b></u>! "
							"That means you can link stuff like <a href=\"https://www.google.com\">this</a>!"
							"<br>"
						);

					BG_H(4, "Specific Case B");

						BG_TXT("Of course, though, there's <code>BG_LINK</code>.");
						BG_LINK("https://www.google.com", "Here it is in action.");

		BG_PAGEBREAK();

		BG_TOC();

	BG_END("body");
	BG_END("html");

	return EXIT_SUCCESS;

}

