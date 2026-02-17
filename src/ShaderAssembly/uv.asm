getComponent(pc, s0, z)   # s0 = pc.x
load(s5, 1)               # s5 = 1
getComponent(pc, s1, s5)  # s1 = pc.y

getComponent(s, s3, z)    # s3 = s.x
reciprocal(s3, s2)        # s2 = 1 / s3
getComponent(s, s4, s5)   # s4 = s.y
reciprocal(s4, s3)        # s3 = 1 / s4

multiply(s0, s2, s0)      # s0 = s0 * s2
multiply(s1, s3, s1)      # s1 = s1 * s3

setComponent(c, s0, z)    # c.x = s0
setComponent(c, s1, s5)   # c.y = s1