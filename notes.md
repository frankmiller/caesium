

##

- nodes have strong ownership of their direct dependents, unless its a
  loop.
- somehow figure a way to map input stream sends to output streams that
  potentially need to emit.
- internally, streams are similar to a `cell<std::optional<T>>`

  
## Design Decisions
- can only input via streams
  
# Pending design decisions

- Can we sample cells? You can get the same effect by listening to a
  stream that takes a snapshot of the cell.
- When do output stream callbacks fire?
  - on some background thread
  - in the callstack of the call to end transaction.
- if you sample cells in a transaction, its like what a snapshot would
  see, not what the values will be after the transaction ends.

Types

- origin<T>
  - send(T)
- stream concept
- cell concept

Algorithm

- begin transaction
- transform input stream sends into output streams that may fire in this
  transaction (dirty output streams).
- update all dirty output streams and sampled cells with depth first
  search.
- end transaction
  - 
-


## questions

- can you send on a stream and sample a cell in the same transaction?
- what happens if you send on stream a, sample cell b, and send on
  stream c?
