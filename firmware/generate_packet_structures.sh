# This bash script generates packet structures for the firmware

SCRIPTS_DIR="scripts"
SCRIPT_NAME="generate_packet_structures.py"
CSV="structures.csv"
TEMPLATE_PATH="$SCRIPTS_DIR"
TEMPLATE_NAME="packet_structure"
TARGET_PATH="main/L2_app"
PYTHON="C:/Python35/python.exe"

# Command arguments
ARG_CSV_NAME="--csv $SCRIPTS_DIR/$CSV"
ARG_TEMPLATE_PATH="--template_path $TEMPLATE_PATH"
ARG_TEMPLATE_NAME="--template      $TEMPLATE_NAME"
ARGS_TARGET_PATH="--target_path    $TARGET_PATH"

# Create the command by adding all the command line arguments
COMMAND="$PYTHON $SCRIPTS_DIR/$SCRIPT_NAME $ARG_CSV_NAME $ARG_TEMPLATE_PATH $ARG_TEMPLATE_NAME $ARGS_TARGET_PATH"

echo "-------------------------------------------------------------"
echo "Generating packet structures..."
echo "Running $SCRIPT_NAME with these arguments:"
echo "    - $ARG_CSV_NAME"
echo "    - $ARG_TEMPLATE_PATH"
echo "    - $ARG_TEMPLATE_NAME"
echo "    - $ARGS_TARGET_PATH"
$COMMAND
echo "Script completed"
echo "-------------------------------------------------------------"