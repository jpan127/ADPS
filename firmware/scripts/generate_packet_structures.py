import jinja2
import pandas as pd
import math
import os
from enum import IntEnum, Enum, unique
from pprint import PrettyPrinter
from argparse import ArgumentParser


"""
Quick Start:
    python generate_packet_structures.py --csv structures.csv --c_template packet_structure_h.jinja --template_path ./
"""

#=============================================#
#                  Constants                  #
#=============================================#

PP = PrettyPrinter(indent = 4)

#=============================================#
#                   Classes                   #
#=============================================#

@unique
class _FieldType(Enum):
    """
    Enumeration of a field value type
    """
    Bytes = "Bytes"
    Value = "Value"

    @classmethod
    def exist(self, enum):
        return enum == "Bytes" or enum == "Value"

@unique
class _StructureType(Enum):
    """
    Enumeration of types of structures
    """
    Command    = "Command Structure"
    Diagnostic = "Diagnostic Structure"

    @classmethod
    def exist(self, enum):
        return enum == "Command Structure" or enum == "Diagnostic Structure"

@unique
class _EnumerationType(Enum):
    """
    Enumeration of types of C enums
    """
    Type   = "Packet Types"
    Opcode = "Packet Opcodes"

    @classmethod
    def exist(self, enum):
        return enum == "Packet Types" or enum == "Packet Opcodes"

    @classmethod
    def value_to_key(self, value):
        if value == "Packet Types":
            return _EnumerationType.Type
        elif value == "Packet Opcodes":
            return _EnumerationType.Opcode
        else:
            return None

class _Structure(object):
    """
    Represents a C struct structure
    """
    def __init__(self, vars):
        self.vars = vars

    def get_values(self):
        return self.vars

class _Enumeration(object):
    """
    Represents a C enum structure
    """
    def __init__(self, enums, type):
        self.enums = {}
        prefix = ""
        if type == _EnumerationType.Type:
            prefix = "packet_type_"
        else:
            prefix = "packet_opcode_"
        for name, value in enums.items():
            self.enums[prefix + name] = value

    def get_values(self):
        return self.enums

#=============================================#
#               Helper Functions              #
#=============================================#

def _standardize_to_snake_case(string):
    """
    Converts any string in any format to snake case
    @param string : Input string
    @returns      : String in snake case
    """
    snake = ""
    for char in string:
        if char == " ":
            snake += "_"
        else:
            snake += char.lower()
    return snake


def _parse_args():
    """
    Parses all the command line arguments
    """
    arg_parser = ArgumentParser()
    arg_parser.add_argument("--csv"            , required=True  , help="Path to CSV file to parse inputs from")
    arg_parser.add_argument("--template_path"  , required=True  , help="Path to template")
    arg_parser.add_argument("--c_template"     , required=True  , help="Name of C template")
    # arg_parser.add_argument("--js_template" , required=True  , help="Path to JavaScript template")
    # arg_parser.add_argument("--target_path" , required=False , help="Output path, if not specified, outputs to same folder as script")
    arg_parser.add_argument("--target_name" , required=False , help="Output file name, if not specified, defaults to \'packet_structure.*\'")
    return arg_parser.parse_args()


def _parse_csv(csv_path):
    """
    Creates a Panda dataframe object from a CSV file
    @param csv_path : Path to CSV file
    @returns        : A Panda dataframe object
    """
    data = pd.read_csv(csv_path, sep=',')
    return data


def _convert_csv_to_dicts(data):
    """
    Interprets the CSV data and converts it into useable data structures
    @param data : A Pandas dataframe of the CSV file
    @returns    : A populated structure of all information needed to fill the templates with
    """

    # Get column names and indices
    raw_columns = list(data)
    columns = {}
    for index, col in enumerate(raw_columns):
        if not col.startswith("Unnamed"):
            columns[index] = col

    # Create a dictionary for each structure
    structures = {}
    matrix = data.as_matrix()
    for index, col in columns.items():

        # Parse the fields
        fields = [ matrix[0][index], matrix[0][index+1] ]
        
        values = {}
        for row in matrix[1:]:
            # Ignore not-a-number grids
            if type(row[index]) == float and math.isnan(row[index]):
                continue
            else:
                name = _standardize_to_snake_case(row[index])
                values[name] = row[index+1]

        # Sanity check that values are appropriate
        if fields[0] != "Name":
            raise ValueError("{} is not a valid field for {}".format(fields[0], col))
        if not _FieldType.exist(fields[1]):
            raise ValueError("{} is not a valid field for {}".format(fields[1], col))

        # Determine which structure to create
        if _StructureType.exist(col):
            structures[col] = _Structure(values)
        elif _EnumerationType.exist(col):
            structures[col] = _Enumeration(values, _EnumerationType.value_to_key(col))
        else:  
            raise ValueError("{} is not a valid structure or enumeration type".format(col))

        # PP.pprint(structures[col].get_values())

    return structures


def _load_template(template_path, template_name):
    """
    Finds a Jinja template and loads it into an object
    @param template_path : Path to templates
    @param template_name : Name of templates
    """
    template_loader = jinja2.FileSystemLoader(searchpath=template_path)
    template_env    = jinja2.Environment(loader=template_loader, trim_blocks=True)
    template        = template_env.get_template(template_name)

    return template


def _render_template(template, render_args, target_path, target_name):
    """
    Renders a template with the provided substitution argument
    @param template    : Template object
    @param render_args : Arguments to substitute into the template
    @param target_path : Path of output file
    @param target_name : Name of output file
    """
    rendered_template = template.render(render_args)

    with open(os.path.join(target_path, target_name), "w") as file:
        file.write(rendered_template)


def _autogenerate_code(template_path, template_name, target_path, target_name, structures):
    """
    Loads and renders the template with the data from [structures]
    @param template_path : Path to templates
    @param template_name : Name of templates
    @param target_path   : Output path of rendered template
    @param target_name   : Name of output file
    @param structures    : Data values to populate template with
    """
    c_template  = _load_template(template_path, template_name)

    c_args = {
        "MAX_PACKET_SIZE"      : "128",
        "PACKET_TYPES"         : structures[_EnumerationType.Type.value].get_values(),
        "PACKET_OPCODES"       : structures[_EnumerationType.Opcode.value].get_values(),
        "COMMAND_STRUCTURE"    : structures[_StructureType.Command.value].get_values(),
        "DIAGNOSTIC_STRUCTURE" : structures[_StructureType.Diagnostic.value].get_values(),
    }

    # Render both templates
    _render_template(c_template  , c_args  , target_path, target_name + ".h")


def main():
    """
    Main function
    """
    # Parse arguments
    args = _parse_args()

    target_name = args.target_name if args.target_name is not None else "packet_structure"

    # Transform the CSV into a pandas matrix frame
    data = _parse_csv(args.csv)

    structures = _convert_csv_to_dicts(data)

    # Autogenerate code based on the data populated in the pandas frame
    _autogenerate_code(args.template_path, args.c_template, "./", target_name, structures)


if __name__ == '__main__':
    main()
