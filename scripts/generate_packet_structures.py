import jinja2
import pandas as pd
from argparse import ArgumentParser


#===================================#
#          Helper Functions         #
#===================================#

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
    arg_parser.add_argument("--csv"         , required=True  , help="Path to CSV file to parse inputs from")
    arg_parser.add_argument("--c_template"  , required=True  , help="Path to C template")
    # arg_parser.add_argument("--js_template" , required=True  , help="Path to JavaScript template")
    # arg_parser.add_argument("--target_path" , required=False , help="Output path, if not specified, outputs to same folder as script")
    # arg_parser.add_argument("--target_name" , required=False , help="Output file name, if not specified, defaults to \'packet_structure.*\'")
    return arg_parser.parse_args()


def _parse_csv(csv_path):
    data = pd.read_csv(csv_path, sep=',')
    print(data)
    return data


def _convert_csv_to_dicts(data):
    """
    Interprets the CSV data and converts it into useable data structures
    @param data : A Pandas dataframe of the CSV file
    """
    # The CSV has different column types and each should be handled differently
    COLUMN_TYPES = [
        "Field",
        "Bytes",
        "Name",
        "Value",
    ]

    # Get column names and indices
    raw_columns = list(data)
    columns = {}
    for index, col in enumerate(raw_columns):
        if not col.startswith("Unnamed"):
            columns[index] = col
    print(columns)

    structures = {}
    for index, col in columns.items():
        structures[col] = {}
        sub_table = data.iloc[0:, index:index+2]
        print("Field2:", data.iloc[0:1, index:index+2])
        # for x, y in sub_table.items():
        #     print(x, y)


def _load_template(template_path, template_name):
    """
    """
    template_loader = jinja2.FileSystemLoader(searchpath=template_path)
    template_env    = jinja2.Environment(loader=template_loader, trim_blocks=True)
    template        = template_env.get_template(template_name)

    return template


def _render_template(template, template_render_args, target_path, target_name):
    """
    """
    rendered_template = template.render(template_render_args)

    with open(os.path.join(target_path, target_name), "w") as file:
        file.write(rendered_template)


def _autogenerate_code():
    """
    """
    # Load both templates
    js_template = _load_template()
    c_template  = _load_template()

    js_args = {

    }

    c_args = {

    }

    # Render both templates
    _render_template(js_template , js_args , target_path, target_name + ".js")
    _render_template(c_template  , c_args  , target_path, target_name + ".c")


def main():
    # Parse arguments
    args = _parse_args()

    # Transform the CSV into a pandas matrix frame
    data = _parse_csv(args.csv)

    _convert_csv_to_dicts(data)

    # Autogenerate code based on the data populated in the pandas frame
    # _autogenerate_code()


if __name__ == '__main__':
    main()

"""
python generate_packet_structures.py --csv structures.csv --c_template packet_structure_h.jinja
"""