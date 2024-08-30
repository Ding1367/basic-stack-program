# basic-stack-program
I made this randomly just to see how good I am and then when I realized I could be good I decided to add **Lua support** so this might actually be useful, since you can do stuff like, build interpreters on top of it, since a C stack that holds 8 bytes for each element is still somehow better than a Lua table.

# The docs
You don't need much documentation besides being able to type **--nom** or **--nomenu** to silence the options, at least for the actual executable. You need to know some stuff to actually make your cool little .lua file for the executable.

# Building... maybe?
I don't know about Windows since I don't use it to develop, but if you have your lua headers and library and stuff and do some linking it could work. I'm the developer which is the embodiment of "it works on my machine."

You want to build it as an executable by linking to your Lua includes and headers. That's it, no -ObjC no nothing, that's all I want from you when building :3
If you want to build it as a library, all you have to do is remove main and compile as static or dynamic library. You can also remove whatever else you don't want or also just not use it.

## Operations
For operations, you have some options. The main one is **regop** which takes in a name which is at most 63 characters for C reasons and a function which takes one argument (the stack userdata) and returns a boolean representing success or error, one annoying thing about it is if you return false, it will print a message.

However, this is why I put my code on GitHub, so that you can do whatever with my code like add custom options, make the stack dynamically grow (or shrink) and other things like not printing a message.

There's also remop which, if the operation exists, removes the operation from the table. And also getop, which just gets the operation or nil like default tables if not present.

## Signing
You can sign your code which is useful if you want to check if the file is loaded already or add the extension to a list which can make sure no other extensions by the same name can run, I don't know why you'd want that but sure!!!! You can use sign(name) and use getsigns() to get the table itself which is NOT protected so just be careful when modifying it, I won't warn you :3

## The stack userdata
Ok so the main thing in this whole program, it's the stack userdata. Now unfortunately I had to proxy the stack because I didn't want to push a light userdata onto the stack because then all light userdatas will act like a stack which is BAD. This means the userdata metatable "stack" expects the first argument to be a pointer to a pointer to a *ding_stack_t* so you'll need to implement your own. Luckily you don't need to implement your own stack operations because I made some for you. Oh also if you don't need to use the Lua heap, or you can't use it, you can use *stkp_push_stack_to_lua* which takes a *lua_State\** and the stack pointer itself.

Now let's actually get into what you can do in Lua.
You have :push(value) which pushes the value and if the stack didn't tip over it returns true, and if not returns false. If :pop() is trying to grab onto thin air it will return false if not it will return true and the value it popped. Same with :peek(), if it's trying to get nothing it will return false, if it got the top element it will return true and the element.

### Doing stuff yourself
You can get the stack size by calling :size() and to manage the top of the stack yourself, you can use :top(new\_top?), what this does is if new\_top is present it will set the top to new\_top if it doesn't exceed the size, which you guessed it, will return true if it doesn't fail, and if it does it will return false.

To override a value on the stack, and act like you're managing an array rather than a stack, you can :top(index) and then push the new value, and to just access it you can do the same but peek instead.

That is the whole Lua API because you don't really need anything else. Also I did not sandbox Lua, so you can do whatever regular Lua can do, which is why if you get any .lua extensions/files you can load, MAKE SURE THE CODE IS SECURE!!!! Also that means require is available so also check the native code if you can yourself.
