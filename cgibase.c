/*
 * Here are the routines of man2html that output a HREF string.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>              /* tolower() */
#include <string.h>             /* strlen() */
#include "defs.h"

typedef void (*link_print_func_t)(const char *section, const char *page);
typedef link_print_func_t (*link_lookup_func_t)(const char *what);

struct link_part {
    link_print_func_t print;
    char *string;
};

static struct link_part *link_parts_section_page;
static struct link_part *link_parts_section;
static struct link_part *link_parts_page;
static struct link_part *link_parts_home;

static const char *
html_char_escape(int ch) {
    switch (ch) {
    case '"': return "&quot;";
    case '&': return "&amp;";
    case '<': return "&lt;";
    case '>': return "&gt;";
    }
    return NULL;
}

static void
print_html_char(int ch) {
    const char *esc;

    if ((esc = html_char_escape(ch))) {
        printf("%s", esc);
    } else {
        printf("%c", ch);
    }
}

static void
print_html_string(const char *str) {
    for (; *str; str++) {
        print_html_char(*str);
    }
}

static void
link_print_section(const char *section, const char *page) {
    (void)page;
    print_html_char(section[0]);
}

static void
link_print_subsection(const char *section, const char *page) {
    (void)page;
    if (section[0]) {
        print_html_string(&section[1]);
    }
}

static void
link_print_page(const char *section, const char *page) {
    (void)section;
    print_html_string(page);
}

static link_print_func_t
link_lookup_none(const char *what) {
    (void)what;
    return NULL;
}

static link_print_func_t
link_lookup_section(const char *what) {
    if (!strcmp(what, "{section}")) return link_print_section;
    if (!strcmp(what, "{subsection}")) return link_print_subsection;
    return NULL;
}

static link_print_func_t
link_lookup_section_page(const char *what) {
    if (!strcmp(what, "{page}")) return link_print_page;
    return link_lookup_section(what);
}

static link_print_func_t
link_lookup_page(const char *what) {
    if (!strcmp(what, "{page}")) return link_print_page;
    return NULL;
}

static const char *
link_parse(const char *template, link_lookup_func_t lookup,
           struct link_part **out) {
    struct link_part *parts;
    struct link_part *part;
    const size_t maxparts = 15;
    const size_t parts_size = (maxparts + 1) * sizeof(*parts);
    const char *a;
    const char *b;
    char *str;
    size_t len;

    *out = NULL;
    part = parts = xmalloc(parts_size);
    memset(parts, 0, parts_size);
    for (a = template; *a; a = b) {
        b = a;
        if (*a == '{') {
            b++;
            while (*b != '}') {
                if (!isalnum(*b)) {
                    return "syntax error";
                }
                b++;
            }
            b++;
        } else if (*a == '}') {
            return "syntax error";
        } else {
            while (*b && (*b != '{') && (*b != '}')) {
                b++;
            }
        }
        len = b - a;
        str = xmalloc(len + 1);
        memcpy(str, a, len);
        str[len] = '\0';
        //
        if (part >= parts + maxparts) {
            return "too many link parts";
        }
        if (*str == '{') {
            if ((part->print = lookup(str)) == NULL) {
                return "unknown directive";
            }
            free(str);
        } else {
            part->string = str;
        }
        part++;
    }
    *out = parts;
    return NULL;
}

const char *
link_parse_section_page(const char *template) {
    return link_parse(template,
                      link_lookup_section_page,
                      &link_parts_section_page);
}

const char *
link_parse_section(const char *template) {
    return link_parse(template,
                      link_lookup_section,
                      &link_parts_section);
}

const char *
link_parse_page(const char *template) {
    return link_parse(template,
                      link_lookup_page,
                      &link_parts_page);
}

const char *
link_parse_home(const char *template) {
    return link_parse(template,
                      link_lookup_none,
                      &link_parts_home);
}

static void
print_link(struct link_part *parts, const char *section, const char *page) {
    struct link_part *part;

    if (!parts) {
        return;
    }
    for (part = parts; ; part++) {
        if (part->print) {
            part->print(section, page);
        } else if (part->string) {
            printf("%s", part->string);
        } else {
            break;
        }
    }
}

void
print_sig(void) {
}

void
include_file_html(char *g) {
    printf("<A HREF=\"file:/usr/include/%s\">%s</A>&gt;", g,g);
}

void
man_page_html(char *section, char *page) {
    struct link_part *parts;
    const char *text;

    if (section && page) {
        parts = link_parts_section_page;
        text = page;
    } else if (section) {
        parts = link_parts_section;
        text = section;
    } else if (page) {
        parts = link_parts_page;
        text = page;
    } else {
        parts = link_parts_home;
        text = "Return to Main Contents";
    }
    printf("<A HREF=\"");
    print_link(parts, section, page);
    printf("\">");
    print_html_string(text);
    printf("</A>");
}

void
ftp_html(char *f) {
    printf("<A HREF=\"ftp://%s\">%s</A>", f, f);
}

void
www_html(char *f) {
    printf("<A HREF=\"http://%s\">%s</A>", f, f);
}

void
mailto_html(char *g) {
    printf("<A HREF=\"mailto:%s\">%s</A>", g, g);
}

void
url_html(char *g) {
    printf("<A HREF=\"%s\">%s</A>", g, g);
}
