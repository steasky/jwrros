#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
    DESTDIR_ARG="--root=$DESTDIR"
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/book/ts1_ws/src/geometry-melodic-devel/tf_conversions"

# snsure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/book/ts1_ws/install/lib/python2.7/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/book/ts1_ws/install/lib/python2.7/dist-packages:/home/book/ts1_ws/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/book/ts1_ws/build" \
    "/usr/bin/python" \
    "/home/book/ts1_ws/src/geometry-melodic-devel/tf_conversions/setup.py" \
    build --build-base "/home/book/ts1_ws/build/geometry-melodic-devel/tf_conversions" \
    install \
    $DESTDIR_ARG \
    --install-layout=deb --prefix="/home/book/ts1_ws/install" --install-scripts="/home/book/ts1_ws/install/bin"
