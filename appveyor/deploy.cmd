@echo off

ECHO "deploy script..."

ECHO "%APPVEYOR_REPO_BRANCH%"
ECHO "%APPVEYOR_REPO_TAG%"

:: this is ugly - wish AV would have the deploy script switches that Travis does
IF "%APPVEYOR_REPO_TAG%"=="true" (
  ECHO "Tagged commit"
  twine upload --repository-url https://test.pypi.org/legacy/ dist/*
) ELSE (
  ECHO "No tag, skipping deploy"
)
