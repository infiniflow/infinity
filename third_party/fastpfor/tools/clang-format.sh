STYLE=$(which clang-format)
if [ $? -ne 0 ]; then
	echo "clang-format not installed. Unable to check source file format policy." >&2
	exit 1
fi
RE=0
BASE=$(git rev-parse --show-toplevel)
if [ $? -ne 0 ]; then
  ALLFILES=$(find . -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.cc' -o -name '*.hh')
else
  ALLFILES=$(git ls-tree --full-tree --name-only  -r HEAD | grep -e ".*\.\(c\|h\|cc\|cpp\|hh\)\$")
fi
for FILE in $ALLFILES; do
  $STYLE $BASE/$FILE | cmp -s $BASE/$FILE -
  if [ $? -ne 0 ]; then
        echo "$BASE/$FILE does not respect the coding style. Formatting. " >&2
        $STYLE -i $BASE/$FILE
        RE=1
  fi
done

exit $RE
