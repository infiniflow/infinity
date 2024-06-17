#!/bin/bash

function kill_process()
{
    kill -s SIGUSR1 $(pgrep $1)
}

STATUS=$(kill_process "infinity" )

if [[ ${STATUS} == "false" ]];then
    echo "Infinity is closed abnormally!"
else
    echo ${STATUS}
    echo "Infinity is closed successfully!"
fi
