### Exercise 1

**C) While processing the Present (P) bit in Page-Translation-Table Entry Fields (see Section 5.4.1), one of the entries in Table 8-1 is particularly relevant. Which entry is that, and why?**

----

<!-- Write your answer here. -->
- A entrada relevante é a entrada PF (Page Fault) pois é a entrada que mapeia as exceções que ocorrem durante o acesso a uma determinada pagina. e.g. 
    - Se a página não estiver presente na memória, é gerada uma exceção de Page Fault.
    - Se ocorrer um acesso de escrita numa página somente de leitura, é gerada uma exceção de Page Fault.
    - Se ocorrer um acesso de user-mode numa página de kernel-mode, é gerada uma exceção de Page Fault.
