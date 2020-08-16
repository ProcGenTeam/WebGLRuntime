#!/usr/bin/env python3

from absl import app, logging, flags
import json
import os
import subprocess

FLAGS = flags.FLAGS

flags.DEFINE_string("filename", None, "The IDL JSON file to convert.")
flags.DEFINE_string("outputdir", None,
                    "The Output Directroy to write the resulting files to.")
flags.DEFINE_string("outputname", None,
                    "The Output Filename to write the resulting header file to.")

"""
Generated IDL files are header files that provide basic declarations and wrappers.

The normal function signature for quickjs is...
JSValue js_std_gc(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
"""


def read_idl_file(filename):
    with open(filename, "r") as file_handle:
        return json.load(file_handle)


TYPES = {}


def declare_type(type_name, cpp_type, js_to_cpp, cpp_to_js):
    global TYPES

    TYPES[type_name] = {"type_name": type_name, "cpp_type": cpp_type,
                        "js_to_cpp": js_to_cpp, "cpp_to_js": cpp_to_js}


TODO = "TODO()"

declare_type("int32",
             cpp_type="int32_t",
             js_to_cpp="""int32_t {0} = 0;
if (JS_ToInt32(ctx, &{0}, {1}) != 0) {{
    {2}
}}""",
             cpp_to_js="JS_NewInt32(ctx, {0})")


def get_cpp_type(type_name):
    global TYPES

    if TYPES[type_name] != None:
        return TYPES[type_name]["cpp_type"]
    else:
        raise Exception("Unknown type: " + type_name)


def get_js_to_cpp_for_type(type_name, variable_name, argument_number):
    global TYPES

    argument_value = f"argv[{argument_number}]"

    error_callback = "return JS_EXCEPTION;"

    if TYPES[type_name] != None:
        return TYPES[type_name]["js_to_cpp"].format(
            variable_name, argument_value, error_callback)
    else:
        raise Exception("Unknown type: " + type_name)


def get_cpp_to_js_for_type(type_name, variable_name):
    global TYPES

    if TYPES[type_name] != None:
        return TYPES[type_name]["cpp_to_js"].format(variable_name)
    else:
        raise Exception("Unknown type: " + type_name)


def emit_function(decl):
    name = decl["name"]
    return_type = decl["returnType"]

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

    argument_number = 0

    for arg in decl["arguments"]:
        argument_name = arg["name"]
        argument_type = arg["argumentType"]

        argument_cpp_type = get_cpp_type(argument_type)

        argument_conversion = get_js_to_cpp_for_type(
            argument_type, argument_name, argument_number)

        wrapper_argument_conversion += f"{argument_conversion}\n"

        argument_names += [argument_name]

        user_arguments += [f"{argument_cpp_type} {argument_name}"]

        argument_number += 1

    cpp_return_type = get_cpp_type(return_type)

    arguments_joined = ", ".join(argument_names)

    user_arguments_joined = ", ".join(user_arguments)

    user_call = f"{user_function_name}({arguments_joined})"

    wrapper_return_conversion = get_cpp_to_js_for_type(return_type, "user_ret")

    wrapper_function = f"""{wrapper_function_signature} {{
        {wrapper_prechecks}

        {wrapper_argument_conversion}

        {cpp_return_type} user_ret = {user_call};

        JSValue value = {wrapper_return_conversion};

        return value;
    }}"""

    user_signature = f"{cpp_return_type} {user_function_name}({user_arguments_joined});"

    function_list_entry = f"JS_CFUNC_DEF(\"{name}\", {argument_count}, {function_name} )"

    return (wrapper_function, user_signature, function_list_entry)


def emit_declaration(decl):
    if decl["type"] == "function":
        return emit_function(decl)
    else:
        raise Exception("Declartion type " +
                        decl["type"] + " not Implemented.")


def do_codegen(filename, output_filename):
    codegen_name, _ = os.path.splitext(os.path.basename(output_filename))

    idl_file = read_idl_file(filename)

    # Check the version number
    assert idl_file["version"] == 1

    decls = ""

    user_signatures = ""

    function_list_entries = []

    for decl in idl_file["declarations"]:
        wrapper_function, user_signature, function_list_entry = emit_declaration(
            decl)

        decls += wrapper_function + "\n"

        user_signatures += user_signature + "\n"

        function_list_entries += [function_list_entry]

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
    
    JS_FreeValue(ctx, global);
}}
    """

    codegen_file = f"""#pragma once
// AUTOGENERATED by scripts/codegen.py

#include <quickjs.h>

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

{user_signatures}

{init_signature}

#ifdef {impl_define}
{decls}

{function_list}

{init_function}
#endif
"""

    with open(output_filename, "w") as file_handle:
        file_handle.write(codegen_file)

    # Run clang-format
    subprocess.check_call(
        ["clang-format", "-style=google", "-i", output_filename])


def main(args):
    filename = FLAGS.filename
    outputdir = FLAGS.outputdir
    outputname = FLAGS.outputname

    output_filename = os.path.join(outputdir, outputname)

    logging.info("Generating %s to output file %s", filename, output_filename)

    do_codegen(filename, output_filename)


if __name__ == "__main__":
    flags.mark_flag_as_required("filename")
    flags.mark_flag_as_required("outputdir")
    flags.mark_flag_as_required("outputname")

    app.run(main)
