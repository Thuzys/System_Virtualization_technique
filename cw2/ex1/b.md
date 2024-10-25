### Exercise 1

**b) Suppose AMD wants to support virtual address spaces of 512 PB. What changes would need to be made to Figure 5.18? How many different "Page-Map Level-5" tables could then exist for a single process? Read sections 1.1.3 and 5.3.1 and indicate the ranges of canonical addresses for the proposed extension.
**

----

**Changes to be made to Figure 5.18**

<!-- Write your answer here, along with the reasoning behind it. -->
- Seria necessário adicionar mais um nível de Page-Map (PML6) para suportar o espaço de endereçamento virtual de 512 PB. O numero de entradas na tabela do novo nivel seria de 2^2 <=> 4 entradas, pois com 2 bits a mais já seria possivél endereçar 512 PB. Uma vez que 2^59 é igual a 512 PB.

**Maximum number of "Page-Map Level-5" tables per process**

<!-- Write your answer here, along with the reasoning behind it. -->
- O número máximo de Page-Map Level-5 tables por processo seria 4. Uma vez que o numero de entradas da tabela PLM6 é 4.

**Ranges of canonical addresses**

<!-- Write your answer here, along with the reasoning behind it. -->
- O range de endereços canónicos para o novo espaço de endereçamento virtual de 512 PB seria:
    - 0x0000_0000_0000_0000 a 0x7FFF_FFFF_FFFF_FFFF (bits 59 a 63 a 0)
    - 0xFC00_0000_0000_0000 a 0xFFFF_FFFF_FFFF_FFFF (bits 59 a 63 a 1)
