@echo off


set /p mainDevelopment = "What is the name of the main development branch, typically master, main, or development: "

set /p branchName="Working branch name, ie if you are working in main, then type main: "
set /p commitMessage="What did you work on for this commit?: "
set /p workflowSelection="Type y for PR workflow, else push on main development is assumed: "

if /i "%workflowSelection%"=="y" ( call:PrWorkflow) 
if "%workflowSelection%" == "s" (call:singleBranchWorkflow)
call:mainPushWorkflow

exit /b

:PrWorkflow

if /i "%mainDevelopment%" == "%branchName%" (call:singleBranchWorkflow)
git status

git add -A

git commit --all -m "%commitMessage%"

git checkout %mainDevelopment%

git status

git pull

git checkout %branchName%

git merge %mainDevelopment%

git push

echo Pushed to the git associated with project. Please handle the necessary pull request on github to make your changes into the development branch

pause

exit /b

:mainPushWorkflow
if /i "%mainDevelopment%" == "%branchName%" (call:singleBranchWorkflow)
echo target push workflow selected
git status

git add -A

git commit --all -m "%commitMessage%"

git push

git checkout %mainDevelopment%

git pull

git merge %branchName%

git push

git checkout %branchName%

git merge %mainDevelopment%

git push

echo Pulled target development branch, merged your working branch
echo pushed target development branch
echo checked out to working branch
echo synchronized working branch with target.
echo pushed working to working remote to synchronize

pause

exit /b

:singleBranchWorkflow
echo.
echo Alert single branch workflow
echo Potential major issues if working in team
set /p confirm = "Press n to stop execution of git commands: "
echo.

if  "%confirm%" == "n" (exit /b) else (

git status

git add -A

git commit --all -m "%commitMessage%"

git push

echo Operation complete

pause

exit /b )



