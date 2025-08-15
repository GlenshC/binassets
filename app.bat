@echo off
if exist build\binassets_exe.exe (
    call build\binassets_exe.exe %*
) else (
    if exist build\Debug\binassets_exe.exe (
        call build\Debug\binassets_exe.exe %*
    ) else (
        if exist build\Release\binassets_exe.exe (
            call build\Release\binassets_exe.exe %*
        ) else (
            @echo No build executable found.
        )
    )
)
