@echo off
if exist build\testapp.exe (
    call build\testapp.exe %*
) else (
    if exist build\Debug\testapp.exe (
        call build\Debug\testapp.exe %*
    ) else (
        if exist build\Release\testapp.exe (
            call build\Release\testapp.exe %*
        ) else (
            @echo No build executable found.
        )
    )
)
