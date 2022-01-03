x86_64-w64-mingw32-g++ dsktool.cpp -o bin/Windows64/dsktool.exe
bzr whoami "Amaury Carvalho <amauryspires@gmail.com>"
bzr launchpad-login amaurycarvalho
#bzr init
bzr add *
today=$(date +"%x %r %Z")
bzr commit -m "Commit on $today by $USER"
bzr push lp:~amaurycarvalho/dsktool/trunk
