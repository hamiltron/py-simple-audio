@echo off

ECHO "deploy script..."

IF "%APPVEYOR_REPO_TAG%"=="true" (
  ECHO "Tagged commit - attempting deploy"
) ELSE (
  ECHO "No tag, skipping deploy"
)
