@echo off
:: Syntax: color [background][text]
color 0F

set /p mainDevelopment="What is the name of the main development branch, typically master, main, or development: "

set /p branchName="Working branch name, ie if you are working in main, then type main: "
set /p workflowSelection="Type y for PR workflow, else push on main development is assumed: "

:commitEntry
cls
set /p commitMessage="What did you work on for this commit?: "

if "%commitMessage%"=="" (

color 04

echo Fatal, commit message requires input

goto commitEntry
)
color 0F
if /i "%mainDevelopment%"=="%branchName%" (call:singleBranchWorkflow)

if /i "%workflowSelection%"=="y" ( call:PrWorkflow) else (call:mainPushWorkflow)



exit

:PrWorkflow

color 0F
echo ==========================================
color 02
echo Pr workflow selected
echo ==========================================
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

exit 

:mainPushWorkflow
cls
color 0F
echo ==========================================
color 02
echo target push workflow selected
echo ==========================================
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

exit 

:singleBranchWorkflow
echo.
cls
color 04
echo Alert single branch workflow
echo Potential major issues if working in team
set /p confirm="Press n to stop execution of git commands: "
echo.

if  "%confirm%"=="n" (exit) else (

color 06

echo Override safety feature, sending upstream... standby...

git status

git add -A

git commit --all -m "%commitMessage%"

git push

echo Operation complete

pause

exit  )



