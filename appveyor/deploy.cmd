@echo off

echo
IF "%APPVEYOR_REPO_TAG%"=="True" (
  ECHO "Tagged commit - attempting deploy"
)
