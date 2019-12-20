
## possible names

Lithium 
Potassium
Caesium
Francium 


##

- nodes have strong ownership of their direct dependents, unless its a
  loop.



Algorithm

- begin transaction
- stream send and cell sample calls are recorded in respective queues.
- end transaction:
  - the time cell is updated and propagated to dependent cells
  - for each stream send
    - emit value in internal signal

