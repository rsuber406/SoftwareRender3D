@echo off



set /p branchName="Working branch name, ie if you are working in main, then type main."

echo %branchName%

git checkout %branchName%

git add -A

set /p commitMessage="What did you work on for this commit?"

git commit --all -m "%commitMessage%"

git push

echo Pushed to the git associated with project.

pause



