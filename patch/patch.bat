@echo off
title Patch

copy "hd533_launcher.exe" In.exe
copy "bin\hd533_launcher.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\HD Launcher.exe"


copy "heroes3_sod.exe" In.exe
copy "bin\heroes3_pow.bin" .
copy "bin\heroes3_pow_icon.bin" .
copy "extra_all\*.bin" .
copy "extra_sod\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow.exe"

copy "heroes3_sod.exe" In.exe
copy "bin\heroes3_pow.bin" .
copy "bin\heroes3_pow_icon.bin" .
copy "bin\heroes3_hd.bin" .
copy "extra_all\*.bin" .
copy "extra_sod\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow HD.exe"

copy "heroes3_sod.exe" In.exe
copy "bin\heroes3_sod.bin" .
copy "extra_all\*.bin" .
copy "extra_sod\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_sod.exe"

copy "heroes3_sod.exe" In.exe
copy "bin\heroes3_sod.bin" .
copy "bin\heroes3_hd.bin" .
copy "extra_all\*.bin" .
copy "extra_sod\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_sod HD.exe"

copy "heroes3_hota161.exe" In.exe
copy "bin\heroes3_hota.bin" .
copy "extra_all\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_hota.exe"

copy "heroes3_hota161.exe" In.exe
copy "bin\heroes3_hota.bin" .
copy "bin\heroes3_hd.bin" .
copy "extra_all\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_hota HD.exe"

copy "heroes3_era392.exe" In.exe
copy "bin\heroes3_era.bin" .
copy "extra_all\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_era.exe"

copy "heroes3_era392.exe" In.exe
copy "bin\heroes3_era.bin" .
copy "bin\heroes3_hd_wog.bin" .
copy "extra_all\*.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pow_era HD.exe"


copy "h3maped_sod.exe" In.exe
copy "bin\h3maped_sod.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pmaped_sod.exe"

copy "h3maped_hota161.exe" In.exe
copy "bin\h3maped_hota.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pmaped_hota.exe"

copy "h3maped_era392.exe" In.exe
copy "bin\h3maped_era.bin" .
ExeBuilder.exe
del *.bin
del In.exe
move Out.exe "..\h3pmaped_era.exe"

echo OK
set /P %a=
