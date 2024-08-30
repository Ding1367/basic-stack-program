# What this does
If you haven't found out by just looking at the name, DyStack makes the stack dynamic. This adds a hold variable for when you need to resize your stack.

Know that this implementation isn't exactly what you call "perfect" in development so you still want to try to build your own DyStack.

# Resizing the stack
Sometimes you might need to resize the stack, but if you want to, theres a likely chance you ran out of space.

This is why there's a *hold* operation which, if holds a value right now, pushes it onto the stack and resets it, and if not pops the value and stores it for future use.

Now to resize your stack you can hold if you don't have enough space, and then use *resize* to pop off the top value (new size) and attempt to resize the stack.

Of course memory reallocation and any memory operation in general can fail, even free if the pointer is invalid or you freed it already, so it will attempt to do so. It will return false if it fails.

Do not reallocate the stack to make it smaller because if you resize it back to it's normal size there won't be anything because to actually initialize the new area of the memory it sets it to zero. So unless you're fine with that, do not reallocate the stack to be smaller.

# Building
Because this extension involves memory, it will use native modules so make sure to put the library in a place Lua can actually find it using the *LUA_CPATH* variable, or just modify your *LUA_CPATH* to find your library wherever you put it.

Like the actual stack program itself I don't know exactly how to build this on other systems besides Unix-like systems, just link to Lua and link to the stack program but compiled as a library, and also the includes too.
