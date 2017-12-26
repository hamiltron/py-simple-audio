@echo off

echo "deploy script"
echo "%APPVEYOR_REPO_TAG%"

IF "%APPVEYOR_REPO_TAG%"=="True" (
  ECHO "Tagged commit - attempting deploy"
)
