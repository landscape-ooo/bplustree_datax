#! /bin/bash

prefix=/usr/local/webserver/fdfs2qq_mongo/
fdfs2qq_BIN=$prefix/bin/cli_consume.exe
fdfs2qq_PID=/usr/local/webserver/fdfs2qq_mongo/etc/cli_consume_run.pid

 
 
case "$1" in
 
	start)
		echo -n "Starting fdfs2qq "
		if [ ! -e "$fdfs2qq_PID" ] ; then
			mkdir -p -- "$(dirname -- "$fdfs2qq_PID")"
			touch -- "$fdfs2qq_PID"
		fi
 
		nohup nice $fdfs2qq_BIN &> /dev/null 2>/tmp/cli_consume.error& echo $! > $fdfs2qq_PID
 
		if [ "$?" != 0 ] ; then
			echo " failed"
			exit 1
		fi
 
	;;
    stop)
		echo -n "Shutting down fdfs2qq "
 
		if [ ! -r $fdfs2qq_PID ] ; then
			echo "warning, no pid file found - fdfs2qq_mongo_exe is not running ?"
			exit 1
		fi
 
		kill -SIGTERM `cat $fdfs2qq_PID`
	;;
 
	quit)
		echo -n "Gracefully shutting down fdfs2qq "
 
		if [ ! -r $fdfs2qq_PID ] ; then
			echo "warning, no pid file found - fdfs2qq_mongo_exe is not running ?"
			exit 1
		fi
 
		kill -SIGQUIT `cat $fdfs2qq_PID`
 
	;;
 
	restart)
		$0 stop
		$0 start
	;;

	*)
		echo "Usage: $0 {start|stop|quit|restart}"
		exit 1
	;;
 
esac

printf "\n"
