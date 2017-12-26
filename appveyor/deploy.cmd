@echo off

ECHO "deploy script..."

IF "%APPVEYOR_REPO_TAG%"=="true" (
  ECHO "Tagged commit - attempting deploy"
  twine upload --repository-url https://test.pypi.org/legacy/ dist/*
) ELSE (
  ECHO "No tag, skipping deploy"
)
