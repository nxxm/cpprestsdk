nxxm-override-deps . \
-s Release/include/ \
-s Release/src \
-s Release/src/pch \
-s Release/src/utilities/ \
-x Release/src/http/client/http_client_winhttp.cpp  \
-x Release/src/http/client/http_client_winrt.cpp \
-x Release/src/http/listener/http_server_httpsys.cpp \
-x Release/src/pplx/pplxapple.cpp \
-x Release/src/pplx/pplxwin.cpp \
-x Release/src/streams/fileio_win32.cpp \
-x Release/src/streams/fileio_winrt.cpp \
-x Release/src/websockets  \
-x Release/tests \
-j8 -t gcc-7-cxx17 \
-v -o bingrequest $@
