Templates are the files with an extension `*.th` ("template header" files).

**`index-template.th`**
```html
<!DOCTYPE html>
<html>
<head>
    <!-- String mode -->

    <!-- .. starts with "$." -->
    <!-- .. #define $ (*ctx) -->
    <title>{{ $.user.name }}</title>
</head>
<body>
    <h1>User Profile</h1>
    
    <!-- .. is a shortcut for `sb_appendf(sb, "%s", __VA_ARGS__);`  -->
    <p>Name: {{ sb_appendf(sb, "%s", $.user.name); }}</p>

    <!-- Format mode -->

    <!-- .. starts with '"' -->
    <p>Age: {{ "%d", $.user.age }}</p>
    
    <!-- .. is a shortcut for: `sb_appendf(sb, __VA_ARGS__);` -->
    <p>Birth Year: {{ sb_appendf(sb, "%d", $.current_year - $.user.age); }}</p>

    <p>Score: {{ "%.0f", calculate_score($.user) * 100 }}</p>
    <p>Balance: {{ "%.2f", $.user.balance }}</p>
    <p>Discount: {{ "|%-10.1f\%|", $.user.discount * 100 }}</p>
    <p>User ID: {{ "#%08X", $.user.id }}</p>
    <p>Pointer: {{ "%p", $.user }}</p>
    
    <!-- Raw mode: any valid C code inside the body of a function -->
    <!-- .. translated as is, without modifications -->
    {{ if ($.user.is_admin) { }}
        <p class="admin">Administrator</p>
    {{ } }}
    
    <!-- Line elimination -->
    <!-- .. ends with "-}}" as opposed to "}}" -->
    <!-- .. gets rid of surrounding whitespaces and following newline -->
    <ul>
    {{ for (int i = 0; i < $.user.friend_count; i++) { -}}
        <li>{{ "%s", $.user.friends[i] }}</li>
    {{ } -}}
    </ul>
    
    <!-- Multiline statements is a feature of the Raw mode -->
    {{ 
    if ($.user.notes) {
        sb_appendf(sb, "<p>Notes: %s</p>", $.user.notes);
    }
    -}}

    <!-- Include translated templates using function composition -->
    {{ render_footer_template(ctx, sb); }}
</body>
</html>
```

They are getting translated into the plain C single header libraries, where
every character outside `{{ ... }}` blocks is copied as is.

**`index-template.h`**
```c
#ifndef INDEX_TEMPLATE_H
#define INDEX_TEMPLATE_H

// A consistent render function identifier allows us to define a macro that
// redefines this name to the value that suits current needs the most.
bool render_template(struct Context* ctx, struct StringBuilder* sb);

#enif  // INDEX_TEMPLATE_H

#ifdef IMPLEMENTATION

// Enables explicit context injection
#define $ (*ctx)

// Implicitly checks for en error during each IO operations
#define sb_appendf(...)                                                 \
    do {                                                                \
        /* Macros do not allow recursive calls. */                      \
        /* That is why an actual function is about to get called. */    \
        bool result = sb_appendf(__VA_ARGS__);                          \
        /* Interrupts template rendering as soon as an error occurs. */ \
        if (!result) return false;                                      \
    } while(0)

// User supposed to provide appropriate `struct Context` and
// `struct StringBuilder` definitions.
bool
render_template(struct Context* ctx, struct StringBuilder* sb)
{
    // Translation of `index-template.th` appears in here.
    sb_appendf(sb,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <!-- String mode -->\n"
        "\n"
        "    <!-- .. starts with "$." -->\n"
        "    <!-- .. #define $ (*ctx) -->\n"
        "    <title>"
    );
    sb_appendf(sb, "%s", $.user.name);
    sb_appendf(sb, "</title>\n");
    // .. and so one until entire template file is translated
}

#undef sb_appendf
#undef $

#endif  // IMPLEMENTATION
```

Translated headers can be used like this:

**`main.c`**
```c
#include "string-builder.h"

struct User {
    char const* name;
};
struct Context {
    struct User user;
};

#define IMPLEMENTATION
#  define render_template render_footer_template  // index-template.h needs `render_footer_template`
#    include "footer-template.h"
#  undef render_template                          // We aren't forced to redefine `render_template`
#  include "index-template.h"
#undef IMPLEMENTATION

struct String
IndexPage() {
    struct User user = {
        .name = "John Doe",
    };
    struct Context ctx = {
        .user = user,
    };
    struct StringBuilder sb = {0};
    if (render_template(&ctx, &sb)) {
        return sb.ascii;
    }
    return (struct String){0};
}
```
