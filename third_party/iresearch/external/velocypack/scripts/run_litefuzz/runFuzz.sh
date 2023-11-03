#!/bin/bash

usage() {
  echo "Usage: $0 [ -e path-to-executable ] [ -f path-to-litefuzz-dir ] [ -n number-of-iterations (default: 30000) ] [ -o output-dir (default: \"./crashes\") ] [ -i input-files ]" 1>&2
}

exit_abnormal() {
  usage
  exit 1
}

while getopts ":e:f:n:o:i:" options; do
  case "${options}" in
  e)
    EXECUTABLE_PATH=${OPTARG}
    ;;
  f)
    TESTER_PATH=${OPTARG/\/\/\//}
    ;;
  n)
    ITERATIONS=${OPTARG}
    ;;
  o)
    OUTPUT_DIR=${OPTARG}
    ;;
  i)
    sed -e 's/^"//' -e 's/"$//' <<<"$OPTARG"
    INPUT_FILES+=("$OPTARG")
    ;;
  :)
    echo "Error: -${OPTARG} without argument"
    exit_abnormal
    ;;
  *)
    exit_abnormal
    ;;
  esac
done

if [[ -z "${EXECUTABLE_PATH}" || ! -f "${EXECUTABLE_PATH}" ]]; then
  echo "Error: must provide a valid executable path"
  exit_abnormal
fi

if [[ -z "${TESTER_PATH}" || ! -f "${TESTER_PATH}/litefuzz.py" ]]; then
  echo "Error: must provide valid path of litefuzz"
  exit_abnormal
fi

if [[ -z "${OUTPUT_DIR}" ]]; then
  OUTPUT_DIR="./crashes"
fi
eval "	rm -rf ${OUTPUT_DIR} ; mkdir ${OUTPUT_DIR}"

LIST_OF_INPUT_FILES=()
input_files_dir_all=""
if [[ -z "${INPUT_FILES}" ]]; then
  echo "Error: must provide input files"
  exit_abnormal
else
  for file in ${INPUT_FILES[@]}; do
    if [[ "$file" =~ .*\/\*\..* ]]; then
      files=$(ls ${file/\"/})
      for file2 in "$files{@}"; do
        LIST_OF_INPUT_FILES+=("$file2")
      done
    elif [[ "$file" =~ .*\/\*.* ]]; then
      file=${file/\"/}
      files=$(ls ${file/\/\*/})
      for file2 in "$files{@}"; do
        LIST_OF_INPUT_FILES+=("$file2")
      done
    elif [[ -d $file ]]; then
      files=$(ls ${file})
      for file2 in $files{@}; do
        input_file="$file/$file2"
        LIST_OF_INPUT_FILES+=("${input_file/\/\//\/}")
      done
    else
      LIST_OF_INPUT_FILES+=("$file")
    fi
  done
fi

IS_NUMBER='^[0-9]+$'

if [[ -z ${ITERATIONS} ]]; then
  ITERATIONS=30000
elif ! [[ $ITERATIONS =~ $IS_NUMBER ]]; then
  echo "Error: Iterations argument must be a number."
  exit_abnormal
fi

count=0
for input_file in ${LIST_OF_INPUT_FILES[@]}; do
  if ! [[ -f $input_file ]]; then
    echo "Error: $input_file is not a valid file"
  else
    ((count = count + 1))
    echo "----------------------------------------------------------------------------------------------------------------------------"
    echo "Running test ${input_file} with ${ITERATIONS} iterations..."
    eval "python3 ${TESTER_PATH}/litefuzz.py -l -c \"${EXECUTABLE_PATH} FUZZ\" -n ${ITERATIONS} -i \"${input_file}\" -o \"${OUTPUT_DIR}/crashes${count}\""
    echo "----------------------------------------------------------------------------------------------------------------------------"
  fi
done
