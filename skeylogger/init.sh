#!/bin/bash
sudo apt-get install expect -y

cd /root
mkdir .ssh
cd .ssh
basedir=`pwd`

set timeout 2
expect<<-END
spawn ssh-keygen -t rsa
expect "*id_rsa): " {send "${basedir}/id_rsa\n"}
expect {
"Overwrite (y/n)?"  {send "n\n"}
"Enter passphrase*" {send "\n"
expect "Enter same*"
send "\n"}
}

spawn scp "${basedir}/id_rsa.pub" sun@10.162.128.97:/home/sun/.ssh/authorized_keys
expect {
#""  {send "\n"}
"password: " {send "sysaaa\n"}
"*connecting (yes/no)?"  {send "yes\n"}
}
expect "*password:" {send "sysaaa\n"}
expect eof
exit
END
