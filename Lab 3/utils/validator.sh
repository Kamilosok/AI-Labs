#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <exercise-number> <program> [program-args...]"
  exit 1
fi

exercise="$1"
program="$2"
shift 2
prog_args=( "$@" )

root_dir="$(dirname "$0")/.."
test_dir="$root_dir/tests"

tests=( "$test_dir"/in${exercise}_*.txt )

if [ ${#tests[@]} -eq 0 ] || [ ! -e "${tests[0]}" ]; then
  echo "No test files found for exercise $exercise in $test_dir"
  exit 1
fi

passed=0
total=0

for inpath in "${tests[@]}"; do

  total=$((total + 1))
  num=$(basename "$inpath" | sed -E "s/in${exercise}_([0-9]+)\.txt/\1/")
  outpath="$test_dir/out${exercise}_${num}.txt"

  infile="$root_dir/zad${exercise}_input.txt"
  outfile="$root_dir/zad${exercise}_output.txt"

  cp "$inpath" "$infile"

  "$program" "${prog_args[@]}"
  if diff -q -w -B "$outfile" "$outpath" >/dev/null; then
    echo -n "Test $num passed"
    passed=$((passed + 1))
  else
    echo "Test $num failed"
    echo "Expected:"
    cat "$outpath"
    echo "Got:"
    cat "$outfile"
  fi
  echo
done

echo "Summary: $passed / $total tests passed."

# Cleanup
rm -f "$root_dir/zad${exercise}_input.txt" "$root_dir/zad${exercise}_output.txt"
