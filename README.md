# Raisin Malware


## Options 

* root              Grants you root shell access.
* hpid $PID         Hides the specified PID.
* unpid $PID        Unhides the specified PID.
* hfile $FILENAME   Hides the specified FILENAME globally. Must be a filename without any path
* unfile $FILENAME  Unhides the specified FILENAME.
* hide u             Hides the rootkit LKM.
* unhide            Unhides the rootkit LKM.
* protect           Protects the rootkit from rmmod.
* unprotect         Disables the rmmod protection.
* reverse $IP       Sends start command to begin reverse shell at IP address, port is 443

## NOTES

* For hfile and unfile the path to the file must be from the persecptive of 
the application where it was started.
* In order to avoid errors with c strings add the '\0' to the end of file name
