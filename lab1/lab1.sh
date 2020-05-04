#!/bin/bash -e

STATUS_CODE_SUCCESS=0
STATUS_CODE_ERROR=1

function returnSucceful() {
  printf "\033[1;37m\033[42m%s \033[0m" $1
  echo ""

  exit $STATUS_CODE_SUCCESS
}

function returnError() {
  printf "\033[1;37m\033[41m%s \033[0m" $1
  echo ""

  exit $STATUS_CODE_ERROR
}

function handle_SIGNALS() {
  rm -rf -- $mktempName
  error "User kill process."
}

#проверяем параметры
fileName="$1"
[ -z "$fileName" ] && returnError 'Invalid first argument! Expected: bash lab1.sh filName.fileType'
[ ! -e "$fileName" ] && returnError 'Can not find file !'
[ ! -r "$fileName" ] && returnError 'Can not read file !'

OUTPUT_REGEX="s/^[[:space:]]*\/\/[[:space:]]*Output[[:space:]]*\([^ ]*\)$/\1/p"

#поиск комментария с ключевым словом Output
outputName=$(sed -n -e "$OUTPUT_REGEX" "$fileName")
[ -z "$outputName" ] && returnError 'Output comment was not found !'

echo "Create temp folder..."
mktempName=$(mktemp -d -t temp.XXXXX) || returnError 'Failed to create temp folder !'

trap handle_SIGNALS HUP INT QUIT PIPE TERM

echo "Copy file to temp folder..."
cp "$fileName" $mktempName || { rm -rf -- "$mktempName"; returnError 'Failed to copy file !'; }

echo "Build src file..."
setupPath=$(pwd)
cd "$mktempName" && echo "Go to the temp directory..."
g++ -std=c++11 -o "$outputName" "$fileName" || { rm -rf -- "$mktempName"; returnError "Failed compiling src file !"; }

echo "Move executable file to setup path..."
cp "$outputName" "$setupPath" || { rm -rf -- "$mktempName"; returnError "Failed to move executable file !"; }

rm -rf -- "$mktempName"
returnSucceful "Successfully completed !"
