### Exercise 1

**D) Use an AI tool designed for assisting with programming and technical inquiries to help
you read the code snippet between lines 378 and 397 of the following Linux kernel source code file:
https://github.com/torvalds/linux/blob/7234e2ea0edd00bfb6bb2159e55878c19885ce68/arch/x86/kernel/head_64.S#L378
Find information in Section 5.4.1 of the AMD64 manual to explain what is going on between lines 387 and 390.**

----

Estas linhas manupulam os bits de controlo de processo para permitir system calls e caso seja suportado pelo CPU, habilitar os recursos NX (No-Execute) para melhorar a segurança do sistema.

