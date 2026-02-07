# Getting started

Let's start with showcase of most of the features.

**`filename.th`**
```html
<!DOCTYPE html>
<html>
<head>
    <!-- Example of explicit "context injection" -->
    <title>{{ $user->name }}</title>
</head>
<body>
    <h1>User Profile</h1>
    
    <!-- String mode -->
    <p>Name: {{ ctx->user->name }}</p>

    <!-- Format mode -->
    <p>Age: {{ "%d", ctx->user->age }}</p>
    
    <!-- .. supports any valid C expressions -->
    <p>Birth Year: {{ "%d", $current_year - $user->age }}</p>
    
    <!-- .. is fully compliant with C printf format -->
    <p>Balance: {{ "%.2f", $user->balance }}</p>
    <p>Discount: {{ "|%-10.1f\%|", $user->discount * 100 }}</p>
    <p>User ID: {{ "#%08X", $user->id }}</p>
    <p>Pointer: {{ "%p", $user }}</p>

    <!-- .. supports complex expression -->
    <p>Score: {{ "%.0f", calculate_score(ctx->user) * 100 }}</p>
    
    <!-- Raw mode: any valid C code inside the body of a function -->
    {{ if ($user->is_admin) { }}
        <p class="admin">Administrator</p>
    {{ } }}
    
    <!-- Line elimination -->
    <!-- .. gets rid of surrounding whitespace and following newline -->
    <ul>
    {{ for (int i = 0; i < $user->friend_count; i++) { -}}
        <li>{{ "%s", $user->friends[i] }}</li>
    {{ } -}}
    </ul>
    
    
    <!-- Multiline statements -->
    {{ 
    if ($user->notes) {
        sb_appendf(sb, "<p>Notes: %s</p>", $user->notes);
    }
    -}}

    <!-- Include translated templates using function composition -->
    {{ render_footer_template(ctx, sb); }}
</body>
</html>
```

gets translated into

**`filename.h`**
```c
struct String;
struct Context;
struct StringBuilder;

#define $ ctx->

struct String
render_template(struct Context* ctx, struct StringBuilder* sb);

#ifdef IMPLEMENTATION
struct String
render_template(struct Context* ctx, struct StringBuilder* sb)
{
    sb_appendf(sb, "%s", "example\n");
    return sb->ascii;
}
#endif  // IMPLEMENTATION
```

that later can be used as

**`main.c`**
```c
#include "string-builder.h"

struct User {
    char const* name;
};
struct Context {
    struct User* user;
};

#define IMPLEMENTATION
#define render_template render_index_page
#include "filename.h"

struct String
IndexPage() {
    struct User user = {
        .name = "John Doe",
    };
    struct Context ctx = {
        .user = &user,
    };
    struct StringBuilder sb = {0};
    return render_index_page(&ctx, &sb);
}
```

# Format statement modes

Everything inside `{{` and `}}` is called format statement. There are following
rules of how those statements are translated, rest appears in as a raw byte
stream in the output.

## Rule 1: The "Leading Quote" Rule (Format Mode)

If the first non-whitespace character is a double quote ("), the block is
interpreted as Format Mode.

Example:
Input: `{{ "%d", ctx->user->age }}`
Output: `sb_appendf(sb, "%d", ctx->user->age);`

## Rule 2: The "Simple Path" Rule (String Mode)

If the content consists entirely of a single C-style "path" (alphanumerics,
underscores, ->, or .) and contains no spaces, commas, or semicolons, it is
interpreted as String Mode.

Transformation: Wrap in `sb_appendf(sb, "%s", ctx->[content]);`
Example:
Input: `{{ user->name }}`
Output: `sb_appendf(sb, "%s", ctx->user->name);`

## Rule 3: The "Statement/Logic" Rule (Raw Mode)

If the content does not meet Rule 1 or Rule 2, or if it contains specific C
triggers, it is interpreted as Raw Mode.

## Rule 4: line elimination

If line contains single statement that ends with `-}}` this line should not
output any whitespaces including newline surrounding it.
