@echo off


set /p mainDevelopment = "What is the name of the main development branch, typically master, main, or development: "

set /p branchName="Working branch name, ie if you are working in main, then type main: "
set /p commitMessage="What did you work on for this commit?: "

git add -A

git commit --all -m "%commitMessage%"

git checkout %mainDevelopment%

git pull

git checkout %branchName%

git merge %mainDevelopment%

git push

echo Pushed to the git associated with project. Please handle the necessary pull request on github to make your changes into the development branch

pause



