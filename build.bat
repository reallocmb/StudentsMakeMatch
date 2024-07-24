pushd bin
cl /Zi /I../inc ../src/students_make_match.c /link /LIBPATH:../lib hpdf.lib Comdlg32.lib /SUBSYSTEM:WINDOWS
main.exe
popd
