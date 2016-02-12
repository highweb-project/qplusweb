clear

echo "===================================="
echo " Web Service Library Release System "
echo "===================================="

packagename=
buildfolder="armbuild"
buildprocess="arm"

datetime=`date +%Y%m%d_%H%M%S`
packagename="WebEngine-""$buildprocess""-""$datetime"".tar.gz"
WebEngine="WebEngine-$buildprocess"

echo "=================================================="
echo "                  Release info"
echo "=================================================="
echo "build process : $buildprocess"
echo "build folder : $buildfolder"
echo "package name : $packagename"
echo "=================================================="
echo "execution(y/n)?"
read execute

result=

function CheckResult()
{
	if [ $1 -eq 1 ]
	then
		echo "Fail to $2"
		echo "cancel release"
		exit
	else
		echo "=============================================="
		echo "Success : $2"
		echo ""
	fi
}

if [ $execute = "y" ]
then

sudo rm -r $WebEngine

sudo mkdir -p $WebEngine
CheckResult $? "make directory : $WebEngine"

sudo mv $buildfolder/.libs $WebEngine/
CheckResult $? "move $buildfolder/.libs to $WebEngine/"

sudo mv $buildfolder/Programs $WebEngine/
CheckResult $? "move $buildfolder/Programs to $WebEngine/"

sudo mv $buildfolder/*.la $WebEngine/
CheckResult $? "move $buildfolder/*.la to $Webengine/"

sudo mv $buildfolder/*.gir $WebEngine/
CheckResult $? "move $buildfolder/*.gir to $WebEngine/"

sudo cp $buildfolder/seedlauncher.sh $WebEngine/

sudo tar cvzf $packagename $WebEngine/
CheckResult $? "$WebEngine packaging... $packagename"

sudo mv $WebEngine/* $buildfolder/
sudo mv $WebEngine/.libs $buildfolder/

sudo rm -r $WebEngine

echo "Checking....."
if [ -e $packagename -a -s $packagename ]
then
echo "Success to Package!!"
exit 

else

echo "Fail to package"

fi

elif [ $execute = "n" ]
then

echo "cancel release and packaging."

fi
