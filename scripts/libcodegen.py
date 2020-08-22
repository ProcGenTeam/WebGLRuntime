
import subprocess
import os

"""
Much of the generated code is based on code internal to quickjs with minor changes.

Generated IDL files are header files that provide basic declarations and wrappers.

The normal function signature for quickjs is...
JSValue js_std_gc(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
"""

TYPES = {}


def declare_type(type_name, cpp_type, js_to_cpp=None, js_to_cpp_cleanup=None, cpp_to_js=None, is_numeric=False):
    global TYPES

    TYPES[type_name] = {"type_name": type_name,
                        "cpp_type": cpp_type,
                        "js_to_cpp": js_to_cpp,
                        "js_to_cpp_cleanup": js_to_cpp_cleanup,
                        "cpp_to_js": cpp_to_js,
                        "is_numeric": is_numeric}


def declare_numeric_type(type_name, cpp_type_name, js_to_cpp_name, cpp_to_js_name):
    declare_type(type_name,
                 cpp_type=cpp_type_name,
                 js_to_cpp=f"""{cpp_type_name} {{0}} = 0;
if ({js_to_cpp_name}(ctx, &{{0}}, {{1}}) != 0) {{{{
    {{2}}
}}}}""",
                 cpp_to_js=f"{cpp_to_js_name}(ctx, {{0}})",
                 is_numeric=True)


TODO = "TODO()"

# Number Handling
declare_numeric_type("int32", "int32_t", "JS_ToInt32", "JS_NewInt32")
declare_numeric_type("int64", "int64_t", "JS_ToInt64", "JS_NewInt64")
declare_numeric_type("uint32", "uint32_t", "JS_ToUint32", "JS_NewUint32")
declare_numeric_type("double", "double", "JS_ToFloat64", "JS_NewFloat64")

declare_type("boolean",
             cpp_type="bool",
             js_to_cpp="""bool {0};
int {0}_r = JS_ToBool(ctx, {1});
if ({0}_r == -1) {{
    {2}
}} else if ({0}_r == 0) {{
    {0} = false;
}} else if ({0}_r == 1) {{
    {0} = true;
}}
""",
             cpp_to_js="""JS_NewBool(ctx, {0})""")

# String Handling
declare_type("const char*",
             cpp_type="const char*",
             js_to_cpp="""const char* {0} = JS_ToCString(ctx, {1});
if ({0} == NULL) {{
    {2}
}}
""",
             js_to_cpp_cleanup="""JS_FreeCString(ctx, {0});""",
             cpp_to_js="""JS_NewString(ctx, {0})""")

# Complex Types
declare_type("Callback",
             cpp_type="Callback*",
             js_to_cpp="""if (!JS_IsFunction(ctx, {1})) {{
    {2}
}}
Callback* {0} = new Callback(ctx, {1});
""",
             js_to_cpp_cleanup="""delete {0};""",
             cpp_to_js=TODO)

declare_type("JSValue",
             cpp_type="JSValue",
             js_to_cpp="""JSValue {0} = {1};""",
             cpp_to_js="""{0}""")

declare_type("ArrayBuffer",
             cpp_type="ArrayBuffer",
             js_to_cpp="""size_t {0}_size = 0;
uint8_t* {0}_arr = JS_GetArrayBuffer(ctx, &{0}_size, {1});
if ({0}_arr == NULL) {{
    {2}
}}
ArrayBuffer {0}({0}_arr, {0}_size);
""",
             cpp_to_js=TODO)

declare_type("ArrayBufferView",
             cpp_type="ArrayBufferView",
             js_to_cpp="""size_t {0}_offset = 0;
size_t {0}_length = 0;
size_t {0}_bytes_per_element = 0;
JSValue {0}_arr = JS_GetTypedArrayBuffer(ctx, {1}, &{0}_offset, &{0}_length, &{0}_bytes_per_element);
if (JS_IsException({0}_arr)) {{
    {2}
}}
ArrayBufferView {0}(ctx, {0}_arr, {0}_offset, {0}_length, {0}_bytes_per_element);
""",
             cpp_to_js=TODO)

declare_type("void", cpp_type="void", cpp_to_js="JS_UNDEFINED")


def get_cpp_type(type_name):
    global TYPES

    if TYPES[type_name] != None:
        return TYPES[type_name]["cpp_type"]
    else:
        raise Exception("Unknown type: " + type_name)


def get_js_to_cpp_for_type(type_name, variable_name, argument_value):
    global TYPES

    error_callback = "return JS_EXCEPTION;"

    if TYPES[type_name] != None:
        format_string = TYPES[type_name]["js_to_cpp"]

        if format_string != None:
            return format_string.format(
                variable_name, argument_value, error_callback)
        else:
            return None
    else:
        raise Exception("Unknown type: " + type_name)


def get_js_to_cpp_cleanup_for_type(type_name, variable_name):
    global TYPES

    if TYPES[type_name] != None:
        format_string = TYPES[type_name]["js_to_cpp_cleanup"]

        if format_string != None:
            return format_string.format(variable_name)
        else:
            return None
    else:
        raise Exception("Unknown type: " + type_name)


def get_cpp_to_js_for_type(type_name, variable_name):
    global TYPES

    if TYPES[type_name] != None:
        format_string = TYPES[type_name]["cpp_to_js"]

        if format_string != None:
            return format_string.format(variable_name)
        else:
            return None
    else:
        raise Exception("Unknown type: " + type_name)


def emit_constant(decl):
    name = decl["name"]
    constant_type = decl["constantType"]
    constant_value = decl["constantValue"]

    declarations = ""
    signatures = ""

    function_list_entry = None

    if constant_type == "uint32":
        function_list_entry = f"JS_PROP_INT32_DEF(\"{name}\", {constant_value}, JS_PROP_CONFIGURABLE)"
    else:
        raise Exception(f"constants of type {constant_type} not implemented")

    return (declarations, signatures, function_list_entry, None)


def emit_function(decl):
    name = decl["name"]
    return_type = decl.get("returnType", None)
    constructor = decl.get("constructor", None)
    prototype = decl.get("prototype", None)
    pass_context = decl.get("passContext", False)

    if return_type == None and constructor == None:
        raise Exception("No return type specified")

    function_name = f"js_{name}_wrap"

    user_function_name = f"js_{name}"

    wrapper_function_signature = f"static JSValue {function_name}(JSContext *ctx," \
        "JSValueConst this_val, int argc, JSValueConst *argv)"

    # TODO(joshua): Check argument length and argument types.
    wrapper_prechecks = ""

    argument_names = []

    argument_count = len(decl["arguments"])

    wrapper_argument_conversion = ""

    user_arguments = []

    cleanup_fragments = ""

    if pass_context:
        argument_names += ["ctx"]
        user_arguments += [f"JSContext* ctx"]

    if prototype != None:
        prototype_cpp_type = get_cpp_type(prototype)

        class_id = decl["class_id"]

        wrapper_argument_conversion += f"{prototype_cpp_type} _this = ({prototype_cpp_type}) JS_GetOpaque(this_val, {class_id});\n"

        argument_names += ["_this"]

        user_arguments += [f"{prototype_cpp_type} _this"]

    argument_number = 0

    for arg in decl["arguments"]:
        argument_name = arg["name"]
        argument_type = arg["argumentType"]

        argument_cpp_type = get_cpp_type(argument_type)

        argument_value = f"argv[{argument_number}]"

        argument_conversion = get_js_to_cpp_for_type(
            argument_type, argument_name, argument_value)

        if argument_conversion == None:
            raise Exception("Could not convert " + argument_type)

        wrapper_argument_conversion += f"{argument_conversion}\n"

        argument_names += [argument_name]

        user_arguments += [f"{argument_cpp_type} {argument_name}"]

        argument_cleanup = get_js_to_cpp_cleanup_for_type(
            argument_type, argument_name)

        if argument_cleanup != None:
            cleanup_fragments += argument_cleanup + "\n"

        argument_number += 1

    cpp_return_type = constructor if constructor != None else get_cpp_type(
        return_type)

    arguments_joined = ", ".join(argument_names)

    user_arguments_joined = ", ".join(user_arguments)

    user_call = f"{user_function_name}({arguments_joined})"

    full_user_call = f"{cpp_return_type} user_ret = {user_call};"

    if cpp_return_type == "void":
        full_user_call = f"{user_call};"

    wrapper_return_conversion = ""

    if constructor == None:
        wrapper_return_conversion = get_cpp_to_js_for_type(
            return_type, "user_ret")

        wrapper_return_conversion = f"JSValue _r_value = {wrapper_return_conversion};"
    else:
        class_id = decl["class_id"]

        wrapper_return_conversion = f"""
JSValue proto;
if (JS_IsUndefined(this_val)) {{
    proto = JS_GetClassProto(ctx, {class_id});
}} else {{
    proto = JS_GetPropertyStr(ctx, this_val, "prototype");
    if (JS_IsException(proto)) {{
        return JS_EXCEPTION;
    }}
}}
JSValue _r_value = JS_NewObjectProtoClass(ctx, proto, {class_id});
JS_FreeValue(ctx, proto);
JS_SetOpaque(_r_value, user_ret);
        """

    wrapper_function = f"""{wrapper_function_signature} {{
        {wrapper_prechecks}

        {wrapper_argument_conversion}

        {full_user_call}

        {wrapper_return_conversion}

        {cleanup_fragments}

        return _r_value;
    }}"""

    user_signature = f"{cpp_return_type} {user_function_name}({user_arguments_joined});"

    function_list_entry = f"JS_CFUNC_DEF(\"{name}\", {argument_count}, {function_name} )"

    return (wrapper_function, user_signature, function_list_entry, None)


def emit_class(decl):
    name = decl["name"]
    cpp_name = decl["cppName"]
    cpp_name_ptr = cpp_name + "*"

    class_id_name = f"js_{name}_class_id"

    class_id = f"static JSClassID {class_id_name};"

    declarations = ""

    signatures = ""

    # Add Constructor
    constructor_decl = {
        "type": "function",
        "name": f"{name}_ctor",
        "constructor": cpp_name_ptr,
        "class_id": class_id_name,
        "arguments": decl["constructorArguments"]}

    ctor_wrapper_function, ctor_user_signature, _, _ = emit_function(
        constructor_decl)

    ctor_wrapper_name = f"js_{name}_ctor_wrap"

    ctor_argument_count = len(decl["constructorArguments"])

    # Add finalizer
    finalizer_wrapper_name = f"js_{name}_finalizer_wrap"

    finalizer_name = f"js_{name}_finalizer"

    finalizer_signature = f"void {finalizer_name}({cpp_name_ptr} val);"

    finalizer_wrapper = f"""static void {finalizer_wrapper_name}(JSRuntime *rt, JSValue val)
{{
    {cpp_name} *value = ({cpp_name_ptr}) JS_GetOpaque(val, {class_id_name});
    if (value) {{
        {finalizer_name}(value);
    }}
}}"""

    # Add new function
    new_function_name = f"js_{name}_new"

    new_function_wrapper = f"""JSValue {new_function_name}(JSContext* ctx, {cpp_name_ptr} user_value) {{
    JSValue proto = JS_GetClassProto(ctx, {class_id_name});
    JSValue value = JS_NewObjectProtoClass(ctx, proto, {class_id_name});
    JS_FreeValue(ctx, proto);
    JS_SetOpaque(value, user_value);

    return value;
}}"""

    new_function_signature = f"JSValue {new_function_name}(JSContext* ctx, {cpp_name_ptr} user_value);"

    # Add fromValue function
    fromValue_function_name = f"js_{name}_fromValue"

    fromValue_function_wrapper = f"""{cpp_name_ptr} {fromValue_function_name}(JSContext* ctx, JSValue value) {{
    if (JS_IsUndefined(value) || JS_IsNull(value)) {{
        return nullptr;
    }}

    {cpp_name} *cpp_value = ({cpp_name_ptr}) JS_GetOpaque(value, {class_id_name});

    return cpp_value;
}}"""

    # Add prototype methods
    prototype_name = f"$$PROTO_{name}"

    declare_type(prototype_name, f"{cpp_name_ptr}",
                 f"{prototype_name}_TODO();", f"{prototype_name}_TODO()")

    prototype_signatures = ""

    prototype_declarations = ""

    prototype_function_list_entries = []

    for prototype_decl in decl["prototype"]:
        if prototype_decl["type"] == "function":
            prototype_decl_name = prototype_decl["name"]

            prototype_decl["name"] = f"{name}_{prototype_decl_name}"
            prototype_decl["prototype"] = prototype_name
            prototype_decl["class_id"] = class_id_name

            prototype_argument_count = len(prototype_decl["arguments"])
            prototype_function_name = f"js_{name}_{prototype_decl_name}_wrap"

            pt_wrapper_function, pt_user_signature, pt_function_list_entry, _ = emit_function(
                prototype_decl)

            prototype_signatures += pt_user_signature + "\n"
            prototype_declarations += pt_wrapper_function + "\n"
            prototype_function_list_entries += [
                f"JS_CFUNC_DEF(\"{prototype_decl_name}\", {prototype_argument_count}, {prototype_function_name} )"]
        elif prototype_decl["type"] == "constant":
            pt_wrapper_function, pt_user_signature, pt_function_list_entry, _ = emit_constant(
                prototype_decl)
            prototype_function_list_entries += [pt_function_list_entry]
        elif prototype_decl["type"] == "property":
            prototype_decl_name = prototype_decl["name"]

            prototype_decl["name"] = f"{name}_{prototype_decl_name}"
            prototype_decl["prototype"] = prototype_name
            prototype_decl["class_id"] = class_id_name

            pt_wrapper_function, pt_user_signature, pt_function_list_entry, _ = emit_property(
                prototype_decl)

            prototype_signatures += pt_user_signature + "\n"
            prototype_declarations += pt_wrapper_function + "\n"
            prototype_function_list_entries += [
                f"""JS_CGETSET_DEF("{prototype_decl_name}", js_{name}_{prototype_decl_name}_get_wrap, js_{name}_{prototype_decl_name}_set_wrap)"""]
        else:
            raise Exception(
                "Declaration type " + prototype_decl["type"] + " not supported for prototypes.")

    # Add Class Declaration

    class_decl_name = f"js_{name}_class"

    class_decl = f"""static JSClassDef js_{name}_class = {{
    "{name}",
    .finalizer = {finalizer_wrapper_name},
}};"""

    # Add Prototype funcion list

    prototype_function_list_entries_joined = ",\n".join(
        prototype_function_list_entries)

    prototype_function_list_name = f"js_{name}_prototype_funcs"

    prototype_function_list = f"""static const JSCFunctionListEntry {prototype_function_list_name}[] = {{
        {prototype_function_list_entries_joined}
    }};"""

    # Add Initialization function

    initialization_function_name = f"js_{name}_init"

    initialization_function = f"""void {initialization_function_name}(JSContext* ctx, JSValue global) {{
        JS_NewClassID(&{class_id_name});
        JS_NewClass(JS_GetRuntime(ctx), {class_id_name}, &{class_decl_name});

        JSValue proto = JS_NewObject(ctx);
        JS_SetPropertyFunctionList(ctx, proto, {prototype_function_list_name}, countof({prototype_function_list_name}));
        
        JSValue obj = JS_NewCFunction2(ctx, {ctor_wrapper_name}, "{name}", {ctor_argument_count},
                               JS_CFUNC_constructor, 0);
        JS_SetConstructor(ctx, obj, proto);
        
        JS_SetClassProto(ctx, {class_id_name}, proto);

        JS_SetPropertyStr(ctx, global, "{name}", obj);
    }}"""

    declarations += f"""
{class_id}

{finalizer_wrapper}

{class_decl}

{ctor_wrapper_function}

{new_function_wrapper}

{fromValue_function_wrapper}

{prototype_declarations}

{prototype_function_list}

{initialization_function}
"""

    signatures += f"""
struct {cpp_name};

{ctor_user_signature}

{new_function_signature}

{prototype_signatures}

{finalizer_signature}
"""

    initialization_call = f"{initialization_function_name}(ctx, global);"

    declare_type(name,
                 cpp_type=f"{cpp_name}*",
                 js_to_cpp=f"{cpp_name}* {{0}} = js_{name}_fromValue(ctx, {{1}});",
                 cpp_to_js=f"js_{name}_new(ctx, {{0}})")

    return (declarations, signatures, None, initialization_call)


def emit_property(decl):
    name = decl["name"]
    property_type = decl["propertyType"]
    prototype = decl.get("prototype", None)

    cpp_type = get_cpp_type(property_type)

    user_arguments = []
    argument_names = []

    prefix_fragment = ""

    if prototype != None:
        prototype_cpp_type = get_cpp_type(prototype)

        class_id = decl["class_id"]

        prefix_fragment += f"{prototype_cpp_type} _this = ({prototype_cpp_type}) JS_GetOpaque(this_val, {class_id});\n"

        argument_names += ["_this"]

        user_arguments += [f"{prototype_cpp_type} _this"]

    get_wrapper_name = f"js_{name}_get_wrap"
    set_wrapper_name = f"js_{name}_set_wrap"

    get_user_function_name = f"js_{name}_get"
    set_user_function_name = f"js_{name}_set"

    cpp_to_js_fragment = get_cpp_to_js_for_type(property_type, "user_value")

    get_argument_names_joined = ", ".join(argument_names)

    get_wrapper_function = f"""static JSValue {get_wrapper_name}(JSContext *ctx, JSValueConst this_val) {{
    {prefix_fragment}

    {cpp_type} user_value = {get_user_function_name}({get_argument_names_joined});

    JSValue value = {cpp_to_js_fragment};

    return value;
}}"""

    set_argument_names_joined = ", ".join(argument_names + ["user_value"])

    js_to_cpp_fragment = get_js_to_cpp_for_type(
        property_type, "user_value", "value")

    set_wrapper_function = f"""static JSValue {set_wrapper_name}(JSContext *ctx, JSValueConst this_val, JSValueConst value) {{
    {prefix_fragment}

    {js_to_cpp_fragment}

    {set_user_function_name}({set_argument_names_joined});

    return JS_UNDEFINED;
}}"""

    get_user_arguments_joined = ", ".join(user_arguments)
    set_user_arguments_joined = ", ".join(
        user_arguments + [f"{cpp_type} value"])

    get_user_signature = f"{cpp_type} {get_user_function_name}({get_user_arguments_joined});"
    set_user_signature = f"void {set_user_function_name}({set_user_arguments_joined});"

    declartions = f"""
{get_wrapper_function}

{set_wrapper_function}
"""

    signatures = f"""
{get_user_signature}

{set_user_signature}
"""

    function_list_entry = f"""JS_CGETSET_DEF("{name}", {get_wrapper_name}, {set_wrapper_name} )"""

    return (declartions, signatures, function_list_entry, None)


def emit_declaration(decl):
    if decl["type"] == "function":
        return emit_function(decl)
    elif decl["type"] == "class":
        return emit_class(decl)
    elif decl["type"] == "property":
        return emit_property(decl)
    elif decl["type"] == "constant":
        return emit_constant(decl)
    else:
        raise Exception("Declartion type " +
                        decl["type"] + " not Implemented.")


def do_codegen(codegen_name, idl_file, output_filename):
    # Check the version number
    assert idl_file["version"] == 1

    decls = ""

    user_signatures = ""

    initialization_calls = ""

    function_list_entries = []

    for decl in idl_file["declarations"]:
        wrapper_function, user_signature, function_list_entry, initialization_call = emit_declaration(
            decl)

        decls += wrapper_function + "\n"

        user_signatures += user_signature + "\n"

        if function_list_entry != None:
            function_list_entries += [function_list_entry]

        if initialization_call != None:
            initialization_calls += initialization_call + "\n"

    impl_define = f"JSCODEGEN_{codegen_name}_IMPLEMENTATION"

    init_signature = f"void codegen_{codegen_name}_init(JSContext* context);"

    function_list_entries_joined = ",\n".join(function_list_entries)

    function_list_name = f"js_{codegen_name}_funcs"

    function_list = f"""static const JSCFunctionListEntry {function_list_name}[] = {{
        {function_list_entries_joined}
    }};"""

    init_function = f"""
void codegen_{codegen_name}_init(JSContext* ctx) {{
    JSValue global = JS_GetGlobalObject(ctx);

    JS_SetPropertyFunctionList(ctx, global, {function_list_name},
                               countof({function_list_name}));

    {initialization_calls}
    
    JS_FreeValue(ctx, global);
}}
    """

    codegen_file = f"""#pragma once
// AUTOGENERATED by scripts/codegen.py

# include <quickjs.h>

# ifndef countof
# define countof(x) (sizeof(x) / sizeof((x)[0]))
# endif

{user_signatures}

{init_signature}

# ifdef {impl_define}
{decls}

{function_list}

{init_function}
# endif
"""

    with open(output_filename, "w") as file_handle:
        file_handle.write(codegen_file)

    # Run clang-format
    subprocess.check_call(
        ["clang-format", "-style=google", "-i", output_filename])
