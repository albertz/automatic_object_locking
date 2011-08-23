Some discussion about multithreading in Python came up [here](http://news.ycombinator.com/item?id=2916291).

The author of the linked post discusses different solutions of the locking problem, e.g. the GIL (as in CPython and PyPy) and having explicit locks inside builtin functions (as in Jython).

I stated [here](http://news.ycombinator.com/item?id=2916580) that I think it should also be possible to do the per-object/per-builtin-function in an automatic way.

And this is a small draft demonstrating one way it could be implemented.

-- Albert Zeyer, <http://www.az2000.de>
