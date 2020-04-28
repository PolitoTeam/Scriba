# docker
echo "Cleaning..."
docker stop server 2> /dev/null
docker stop db 2> /dev/null
docker network rm shared_editor 2> /dev/null