# WriteUp : Understand Leak-Primitive Vulnerability in PWN

> WriteUp create in date: 3/5/2026 - 18h:33m

# Index

- 1.What is leak primitive?

- 2.What does it actually do?

- 3.How to exploit it?

- 4.How to protect binary and fix it?

# 1.What is leak primitive?

- leak primitive is a concept, It is not a single vulnerability, it describes these vulnerabilities for leaking memory contents and bypassing many protections such as ASLR, stack canaries etc. the vulnerabilities for leaking memory contents like:

- **Format-string vulnerability** : because leak memory contents with shellcode like: %X, etc.

- **buffer overflow (BOF) vulnerability** : because overwrite the RBP, RIP register can leak info stack. But it will blocked if program have stack canaries

- Or can is like: 

- **Array overflow** : because when access out-of-bounds access from array, then it will access into stack. If program have `printf` then will print contents from stack

- easier to understand, leak primitive only is for these vulnerabilities leak memory contents

# 2.What does it actually do?

> for example: the ASLR protections

- when exploit program but ASLR random address, problem : attacker doesn't know origrinal target address in program

- The leak primitive solution: attacker uses leak primitive for leaking addressing in memory such to stack , when they know original one address from program, they can caculator offset and original other address.

# 3.How to exploit it?