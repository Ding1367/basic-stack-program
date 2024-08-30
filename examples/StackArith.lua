if not sign("StackArith") then
    return
end
regop("add", function(stk)
    if stk:top() < 2 then
        print("not enough arguments")
        return false
    end
    local a, b = select(2, stk:pop()), select(2, stk:pop())
    return stk:push(a + b)
end)
regop("sub", function(stk)
    if stk:top() < 2 then
        print("not enough arguments")
        return false
    end
    local b, a = select(2, stk:pop()), select(2, stk:pop())
    return stk:push(a - b)
end)
