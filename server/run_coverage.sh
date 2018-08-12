# This bash script is used to run coverage analysis on unit tests

# @ { Coverage command
COVERAGE_PROGRAM="coverage"
COVERAGE_MODE="run"
COVERAGE_SOURCE_DIRS="app,jetson_io/uart/microcontroller,tasks"
COVERAGE_UNITTEST="unittest discover -q"
COVERAGE_TEST_DIR="tests"
COVERAGE_COMMAND="$COVERAGE_PROGRAM $COVERAGE_MODE              \
                  --source=$COVERAGE_SOURCE_DIRS                \
                  -m $COVERAGE_UNITTEST -s $COVERAGE_TEST_DIR"
# @ }

# @ { Report command
COVERAGE_REPORT_MODE="report"
COVERAGE_REPORT_COMMAND="$COVERAGE_PROGRAM $COVERAGE_REPORT_MODE"
# @ }

# @ { Generate HTML command
COVERAGE_HTML_MODE="html"
COVERAGE_HTML_COMMAND="$COVERAGE_PROGRAM $COVERAGE_HTML_MODE"
# @ }

# @ { Run commands
eval "$COVERAGE_COMMAND"
eval "$COVERAGE_REPORT_COMMAND"
eval "$COVERAGE_HTML_COMMAND"
# @ }