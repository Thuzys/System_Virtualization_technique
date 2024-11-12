### Exercise 1

**a) Consider Figures 5-18 through 5-23. What changes would need to be made to these figures if the architectural limit for physical memory were to be increased to 64 PB?**

----
<!-- Write your answer here, along with the reasoning behind it. -->
- Sendo que a limitação atual é 4PB tendo 52 bits para endereçar a physical page, sendo que 12 são do offset e 40 são fornecidos pelo PTE. Para a limitação da arquitetura da memória física ser aumentada para 64 PB, o número de bits necessários para endereçar a physical-page base address seria de 44 bits, pois 2^44 * 2^12 = 64 PB <=> 2^56. Assim, o número de bits necessários para endereçar a physical page seria 56.
