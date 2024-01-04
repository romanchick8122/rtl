SCRIPT_DIR=$(dirname "$0")
SRC_DIR="${SCRIPT_DIR}/../include"
DST_DIR="${SCRIPT_DIR}/source"
SRC_FILES=$(find ${SRC_DIR} -name \*.hpp)
HYDE_ARGS="$1 --namespace-blacklist=__detail --hyde-src-root=\"${SRC_DIR}\" --hyde-yaml-dir=\"${DST_DIR}\""
CXX_ARGS="-x c++ -std=c++20 -I ${SRC_DIR}"
for SRC_FILE in ${SRC_FILES}; do
    SRC_PROCESS_COMMAND="hyde ${HYDE_ARGS} \"${SRC_FILE}\" -- ${CXX_ARGS}"
    echo ${SRC_PROCESS_COMMAND}
    eval ${SRC_PROCESS_COMMAND}
done