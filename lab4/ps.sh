#!/bin/bash
# ELEC377 - Operating System
# Lab 4 - Shell Scripting, ps.sh
# Program Description: Creating a shell from scratch

#Setting variables to "no" so its not active 
showRSS="no"
showComm="no"
showCommand="no"
showGroup="no"

#while there are still arguments 
while (($# > 0))
do 
    #if the flag is -rss set the flag to active
    if [[ $1 == "-rss" ]]; then
        showRSS="yes"
    #if the flag is -comm set the flag to active
    elif [[ $1 == "-comm" ]]; then
        showComm="yes"
    #if the flag is -command set the flag to active
    elif [[ $1 == "-command" ]]; then
        showCommand="yes"
    #if the flag is -group set the flag to active
    elif [[ $1 == "-group" ]]; then
        showGroup="yes"
    else
    #anything other than the four flags will end with an error 
        echo "Error - Not a valid flag"
        exit
    fi
    #to stop it from running as an infinite loop
    shift
done

# if both comm and command are in the same command, signal an error
if [ $showComm == "yes" ] && [ $showCommand == "yes" ]; then
    echo "Error - Cannot use both -comm and -command flags"
    exit
fi

#look for filesystem /proc with directories that only composed of digits
for p in /proc/[0-9]* ; do

    #check if p is a directory
    if [ -d $p ]; then

        #use grep to search for status in Pid and removing Pid from output
        pid=`grep '^Pid' $p/status | sed -e s/Pid://`

        #use grep to search for status in Gid and isolate just the numerical GID value to gid
        gid=`grep '^Gid' $p/status | sed -e 's/^[^0-9]*\([0-9]*\).*/\1/'`

        #use grep to search for status in Uid and isolate just the numerical UID value to gid
        uid=`grep '^Uid' $p/status | sed -e 's/^[^0-9]*\([0-9]*\).*/\1/'`

        #use grep to search for status in Name and removing Name from output
        shortComm=`grep '^Name' $p/status | sed -e s/Name://`

        #use grep to search for status in VmRSS and removing VmRSS from output
        rss=`grep '^VmRSS' $p/status | sed -e s/VmRSS://`

        #extract the command line of a process from its cmdline file in proc and replaces the null characters with spaces for readability
        cmd=`cat $p/cmdline | tr '\0' ' '`

        #find the line /etc/passwd that contains the specific UID and extracts username 
        user=`grep -E "^.*:.*:$uid:" /etc/passwd | cut -d : -f 1`

        #find the line /etc/passwd that contains the specific UID and extracts group 
        group=`grep -E "^.*:.*:$gid:" /etc/passwd | cut -d : -f 1`

        #if the cmd has zero as it's length then set the vairable to the value found for comm flag
        if [ -z "$cmd" ]; then
            cmd=$shortComm
        fi  

        #if the length of rss is zero then set it to 0kB
        if [ -z "$rss" ]; then
            rss="0 kB"
        fi

        #print out PID and USER to temp file
        printf "%6s\t%16s" $pid $user >> tmp/tmpPs$$.txt

        if [ $showComm == "yes" ]; then
            #print out short comm to temp file
            printf "\t%s " $shortComm >> tmp/tmpPs$$.txt
        fi

        if [ $showRSS == "yes" ]; then
            #print out RSS to temp file
            printf "\t%6d %2s " $rss >> tmp/tmpPs$$.txt
        fi

        if [ $showGroup == "yes" ]; then
            #print out group to temp file
            printf "\t%16s " "$group" >> tmp/tmpPs$$.txt
        fi

        if [ $showCommand == "yes" ]; then
            #ignore all the blank outputs and print cmd to temp file
            cmd=`echo $cmd | tr -s '[:blank:]'`
            printf "\t%s " "$cmd" >> tmp/tmpPs$$.txt
        fi

        printf "\n" >> tmp/tmpPs$$.txt
    
    fi

done

#Print out the headers depending on the argument's flag
printf "   UID\t            USER"   
if [[ $showRSS == "yes" ]] ; then
    printf "\t%s" 'RSS'
fi

if [[ $showComm == "yes" ]] ; then
    printf "\t\t%s " 'Comm'
fi

if [[ $showCommand == "yes" ]] ; then
    printf "\t\t%s " 'Command'
fi

if [[ $showGroup == "yes" ]] ; then
    printf "\t\t%s " 'Group'
fi

printf "\n"

# sort and print the file
sort -n tmp/tmpPs$$.txt

#cat < tmp/tmpPs$$.txt

# delete file
rm tmp/tmpPs$$.txt
