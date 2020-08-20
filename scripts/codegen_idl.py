#!/usr/bin/env python3

# Based roughly: https://github.com/emscripten-core/emscripten/blob/master/tools/webidl_binder.py

import WebIDL
from absl import app, logging, flags
import os

from libcodegen import do_codegen

FLAGS = flags.FLAGS

flags.DEFINE_string("filename", None, "The WebIDL file to convert.")
flags.DEFINE_string("outputname", None,
                    "The Output Filename to write the resulting header file to.")

TYPE_ALIASES = {}


def add_type_alias(from_type, to_type):
    global TYPE_ALIASES

    TYPE_ALIASES[from_type] = to_type


def resolve_type(type_name):
    return TYPE_ALIASES[type_name]


# Basic Type Aliases
add_type_alias("void", "void")
add_type_alias("boolean", "boolean")

add_type_alias("byte", "int8")
add_type_alias("short", "int16")
add_type_alias("long", "int32")
add_type_alias("long long", "int64")
add_type_alias("octet", "octet")
add_type_alias("unsigned short", "uint16")
add_type_alias("unsigned long", "uint32")
add_type_alias("unrestricted float", "double")

add_type_alias("any", "JSValue")
add_type_alias("DOMString", "const char*")
add_type_alias("(ArrayBufferView or ArrayBuffer)", "ArrayBuffer")
add_type_alias("ArrayBufferView", "ArrayBufferView")
add_type_alias("Float32Array", "ArrayBufferView")


def read_file(filename):
    with open(filename, "r") as file_handle:
        return file_handle.read()


def get_member_idl(thing):
    if thing.isConst():
        id_name = thing.identifier.name
        id_type = resolve_type(thing.type.prettyName())
        id_value = thing.value.value

        return {
            "type": "constant",
            "name": id_name,
            "constantType": id_type,
            "constantValue": id_value
        }
    elif thing.isMethod():
        id_name = thing.identifier.name

        signatures = thing.signatures()

        if len(signatures) > 1:
            raise Exception("WebIDL methods with overloads are not supported")

        returnType, arguments = signatures[0]

        return_pretty_name = returnType.prettyName()

        if return_pretty_name.endswith("?"):
            return_pretty_name = return_pretty_name[:-1]

        id_returnType = resolve_type(return_pretty_name)
        id_arguments = []

        id_passContext = False

        if id_returnType == "JSValue":
            id_passContext = True

        for arg in arguments:
            arg_name = arg.identifier.name
            arg_optional = arg.optional
            arg_pretty_name = arg.type.prettyName()

            if arg_pretty_name.endswith("?"):
                arg_optional = True
                arg_pretty_name = arg_pretty_name[:-1]

            arg_type = resolve_type(arg_pretty_name)

            if arg_type == "JSValue":
                id_passContext = True

            id_arguments += [{
                "name": arg_name,
                "argumentType": arg_type,
                "argumentOptional": arg_optional
            }]

        return {
            "type": "function",
            "name": id_name,
            "returnType": id_returnType,
            "arguments": id_arguments,
            "passContext": id_passContext
        }
    else:
        print(thing)
        raise Exception("Interface Member not handled")


def get_interface_idl(interface):
    name = interface.getClassName()

    add_type_alias(name, name)

    cppName = name

    members = []

    for thing in interface.members:
        members += [get_member_idl(thing)]

    return {
        "type": "class",
        "name": name,
        "cppName": cppName,
        "constructorArguments": [],
        "prototype": members
    }


def do_idl_codegen(input_filename, output_filename):
    input_idl = read_file(input_filename)

    p = WebIDL.Parser()

    p.parse(input_idl)

    data = p.finish()

    decls = []

    for thing in data:
        if thing.isTypedef():
            name, inner_type = thing.identifier.name, thing.innerType
            add_type_alias(name, resolve_type(inner_type.prettyName()))
        elif thing.isInterface():
            interface_decl = get_interface_idl(thing)

            decls += [interface_decl]
        else:
            print(thing)
            raise Exception("Not Implemented")

    idl_file = {
        "version": 1,
        "declarations": decls
    }

    codegen_name, _ = os.path.splitext(os.path.basename(output_filename))

    do_codegen(codegen_name, idl_file, output_filename)


def main(args):
    filename = FLAGS.filename
    output_filename = FLAGS.outputname

    logging.info("Generating %s to output file %s", filename, output_filename)

    do_idl_codegen(filename, output_filename)


if __name__ == "__main__":
    flags.mark_flag_as_required("filename")
    flags.mark_flag_as_required("outputname")

    app.run(main)
