if not sign("DyStack") then return end
local cstackops = require("cstackops")

local hold = nil
regop("hold", function(stack)
    if hold == nil then
        if stack:top() == 0 then
            print("nothing to hold")
            return false
        end
        hold = select(2, stack:pop())
    else
        return stack:push(hold)
    end
    return true
end)

regop("resize", function(stack)
    if stack:top() == 0 then
        print("not enough space to pop off new size")
        return false
    end
    local new_size = select(2, stack:pop())
    if new_size < stack:size() then
        print("warning: scaling stack down")
    end
    return cstackops._resize(stack, new_size)
end)
