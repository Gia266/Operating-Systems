#!/bin/bash
# ELEC377 - Operating System
# Lab 4 - Shell Scripting, ps.sh
# Program Description: Creating a shell from scratch

showRSS="no"
showComm="no"
showCommand="no"
showGroup="no"

while (($# > 0))
do 
    if [[ $1 == "-rss" ]]; then
        showRSS="yes"
    elif [[ $1 == "-comm" ]]; then
        showComm="yes"
    elif [[ $1 == "-command" ]]; then
        showCommand="yes"
    elif [[ $1 == "-group" ]]; then
        showGroup="yes"
    else
        echo "Error - Not a valid flag"
        exit
    fi
    shift
done

if [ $showComm == "yes" ] && [ $showCommand == "yes" ]; then
    echo "Error - Cannot use both -comm and -command flags"
    exit
fi

for p in /proc/[0-9]* ; do
    if [ -d $p ]; then
        pid=`grep '^Pid' $p/status | sed -e s/Pid://`
        gid=`grep '^Gid' $p/status | sed -e 's/^[^0-9]*\([0-9]*\).*/\1/'`
        uid=`grep '^Uid' $p/status | sed -e 's/^[^0-9]*\([0-9]*\).*/\1/'`
        shortComm=`grep '^Name' $p/status | sed -e s/Name://`
        rss=`grep '^VmRSS' $p/status | sed -e s/VmRSS://`
        cmd=`cat $p/cmdline | tr '\0' ' '`
        user=`grep -E "^.*:.*:$uid:" /etc/passwd | cut -d : -f 1`
        group=`grep -E "^.*:.*:$gid:" /etc/passwd | cut -d : -f 1`

        if [ -z "$cmd" ]; then
            cmd=$shortComm
        fi  

        if [ -z "$rss" ]; then
            rss="0 kB"
        fi

        printf "%6s\t%16s" $pid $user >> tmp/tmpPs$$.txt

        if [ $showComm == "yes" ]; then
            printf "\t%s " $shortComm >> tmp/tmpPs$$.txt
        fi

        if [ $showRSS == "yes" ]; then
            printf "\t%6d %2s " $rss >> tmp/tmpPs$$.txt
        fi

        if [ $showGroup == "yes" ]; then
            printf "\t%16s " "$group" >> tmp/tmpPs$$.txt
        fi

        if [ $showCommand == "yes" ]; then
            cmd=`echo $cmd | tr -s '[:blank:]'`
            printf "\t%s " "$cmd" >> tmp/tmpPs$$.txt
        fi

        printf "\n" >> tmp/tmpPs$$.txt
    
    fi

done


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
