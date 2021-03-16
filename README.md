# MIVCOTS_SOFTWARE
This is the software portion of the MIVCOTS project.
The dev team has not yet selected an specific license for the project, but if you would like to use or contribute to the project, feel free to contact us.

## Cloning
Use the command "git clone --recurse-submodules -j8 https://github.com/UABaja/MIVCOTS-Public.git" to also clone the submodules.

## To pull submodules into an already cloned repository, run:
#git submodule init
#git submodule update

## Installing wxWidgets:
1. Download and run the Windows installer for 3.0.4 (latest stable release)

   https://www.wxwidgets.org/downloads/
   
2. Navigate to the install directory (hopefully C:\wxWidgets-3.0.4) then \build\msw
3. Open wx_vc10.sln in Visual Studios 2017. Let it convert everything to v141.
4. Build the solution.
5. Set the WXWIN environment variable to the install directory if the installer didn't. 
   
   See https://wiki.wxwidgets.org/Adding_an_Environment_Variable_under_Windows
   
   Make sure to include a backslash on the end: "C:\wxWidgets-3.0.3\\"

## Other Requirements
1. Add another environemnt variable "C:\Users\[USER]\Documents\MIVCOTS_SOFTWARE\projects\mivcots\Resources\" with the variable name "MivcotsResources"
## Testing with com0com

Install com0com https://sourceforge.net/projects/com0com/

* After running com0com add a pair and then rename the ports to COM42 and COM43 and then hit apply

Install python 3 https://www.python.org/downloads/ (make sure to include it in the PATH)

Open a command prompt and use pip to install serial and pyserial

``` pip install serial```

```pip install pyserial```

Double click the test_data_source.py file to run the com testing

## Installing MariaDB

1. Go to https://downloads.mariadb.org/
2. Click Download on the latest STABLE version of MariaDB. As of 4/22/2018 it is version 10.2.14
3. Download the files for your operating system. I suggest downloading the 64 bit MSI Package.
4. When the MSI package is done downloading run the installer.
5. Click NEXT and then blindly accept the license except if your name is Kyle then read it.
6. Then will see the Custom Setup screen. Everything should be good as is but you can change where your database is by clicking the Browse button and navegating to the desired drive and folder of your choice. Then cick Next.
7. set the root password as "Telemetry" no quotes and then click next.
8. Default Instance Properties are good as is so click Next.
9. You can provide feedback if you want then click Next.
10. On the next screen click Install
11. When the installation is complete run the HeidiSQL program on your desktop and make sure there is a session and you can sign into it with the User:root and Password:Telemetry.
12. If that worked you are good to go and can run the MIVCOTS program.



