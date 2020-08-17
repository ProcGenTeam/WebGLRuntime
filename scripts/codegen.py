#!/usr/bin/env python3

from absl import app, logging, flags
import json
import os

from libcodegen import do_codegen

FLAGS = flags.FLAGS

flags.DEFINE_string("filename", None, "The IDL JSON file to convert.")
flags.DEFINE_string("outputdir", None,
                    "The Output Directroy to write the resulting files to.")
flags.DEFINE_string("outputname", None,
                    "The Output Filename to write the resulting header file to.")


def read_idl_file(filename):
    with open(filename, "r") as file_handle:
        return json.load(file_handle)


def main(args):
    filename = FLAGS.filename
    outputdir = FLAGS.outputdir
    outputname = FLAGS.outputname

    output_filename = os.path.join(outputdir, outputname)

    logging.info("Generating %s to output file %s", filename, output_filename)

    codegen_name, _ = os.path.splitext(os.path.basename(output_filename))

    idl_file = read_idl_file(filename)

    do_codegen(codegen_name, idl_file, output_filename)


if __name__ == "__main__":
    flags.mark_flag_as_required("filename")
    flags.mark_flag_as_required("outputdir")
    flags.mark_flag_as_required("outputname")

    app.run(main)
