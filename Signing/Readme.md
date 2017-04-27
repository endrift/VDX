# How to production-sign for Windows 7 to 10

1. Run `SIGN4PORTAL.CMD`
2. Upload the resulting `*.cab` files to the Microsoft portal
3. Download signed files from portal
4. Extract contents and move to folder containing the files
5. Append EV-Signature to binaries with `signtool sign /v /ac "C:\Program Files (x86)\Windows Kits\10\CrossCertificates\GlobalSign Root CA.crt" /n "Wohlfeil.IT e.U." /tr http://sha256timestamp.ws.symantec.com/sha256/timestamp /fd sha256 /td sha256 /as *.sys *.dll`
6. Overwrite catalog files with `Inf2Cat.exe /driver:"." /os:8_X64,8_X86,Server8_X64,Server2008R2_X64,7_X64,7_X86,Server2008_X64,Server2008_X86,Vista_X64,Vista_X86,Server2003_X64,Server2003_X86,XP_X64,XP_X86,2000,10_X86,10_X64,Server10_X64`
7. Sign new catalog files with `signtool sign /v /ac "C:\Program Files (x86)\Windows Kits\10\CrossCertificates\GlobalSign Root CA.crt" /n "Wohlfeil.IT e.U." /tr http://sha256timestamp.ws.symantec.com/sha256/timestamp /fd sha256 /td sha256 *.cat`


