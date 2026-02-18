# Compute uv, store it in v0
getComponent(pc, s0, z)		# s0 = pc.x
load(s5, 1)					# s5 = 1.0
getComponent(pc, s1, s5)	# s1 = pc.y
setComponent(v0, s0, z)		# v0.x = s0
load(s2, 1)					# s2 = 1.0
setComponent(v0, s1, s2)	# v0.y = s1
load(s2, 2)					# s2 = 2.0
setComponent(v0, z, s2)		# v0.z = z
load(s2, 3)					# s2 = 3.0
setComponent(v0, z, s2)		# v0.w = z

getComponent(s, s0, z)		# s0 = s.x
load(s5, 1)					# s5 = 1.0
getComponent(s, s1, s5)		# s1 = s.y
reciprocal(s0, s0)			# s0 = 1.0 / s0
reciprocal(s1, s1)			# s1 = 1.0 / s1

getComponent(v0, s2, z)		# s2 = v0.x
load(s5, 1)					# s5 = 1.0
getComponent(v0, s3, s5)	# s3 = v0.y
multiply(s2, s0, s0)		# s0 = s2 * s0
multiply(s3, s1, s1)		# s1 = s3 * s1
setComponent(v0, s0, z)		# v0.x = s0
load(s5, 1)					# s5 = 1.0
setComponent(v0, s1, s5)	# v0.y = s1

# Adjust the uv to center around the origin
# Multiply by 2.0
getComponent(v0, s0, z)		# s0 = v0.x
load(s5, 1)					# s5 = 1.0
getComponent(v0, s1, s5)	# s1 = v0.y
load(s2, 2)					# s2 = 2.0
multiply(s0, s2, s3)		# s3 = s0 * s2
multiply(s1, s2, s4)		# s4 = s1 * s2
setComponent(v0, s3, z)		# v0.x = s3
load(s5, 1)					# s5 = 1.0
setComponent(v0, s4, s5)	# v0.y = s4
	
## Subtract 1.0
getComponent(v0, s0, z)		# s0 = v0.x
load(s5, 1)					# s5 = 1.0
getComponent(v0, s1, s5)	# s1 = v0.y
load(s2, 1)					# s2 = 1.0
negate(s2, s2)				# s2 = -s2
add(s0, s2, s0)				# s0 = s0 + s2
add(s1, s2, s1)				# s1 = s1 + s2
setComponent(v0, s0, z)		# v0.x = s0
load(s5, 1)					# s5 = 1.0
setComponent(v0, s1, s5)	# v0.y = s1

# Adjust for aspect ratio
load(s5, 1)					# s5 = 1.0
getComponent(s, s0, s5)		# s0 = s.y
reciprocal(s0, s0)			# s0 = 1.0 / s0
getComponent(s, s1, z)		# s1 = s.x
multiply(s1, s0, s0)		# s0 = s1 * s0
getComponent(v0, s1, z)		# s1 = v0.x
multiply(s1, s0, s0)		# s0 = s1 * s0
setComponent(v0, s0, z)		# v0.x = s0

# Initialize the ray direction, reuse the uv as the ray direction
load(s5, 2)					# s5 = 2.0
load(s4, 1)					# s4 = 1.0
negate(s4, s4)				# s4 = -s4
setComponent(v0, s4, s5)	# v0.z = s4
load(s5, 3)					# s5 = 3.0
setComponent(v0, z, s5)		# v0.w = 0.0
	
# Define sphere, s0 is the z coordinate
load(s0, 2)					# s0 = 2.0
negate(s0, s0)				# s0 = -s0

# Compute a (from quadratic formula), stored in s1
getComponent(v0, s1, z)		# s1 = v0.x
multiply(s1, s1, s1)		# s1 = s1 * s1
load(s5, 1)					# s5 = 1.0
getComponent(v0, s2, s5)	# s2 = v0.y
multiply(s2, s2, s2)		# s2 = s2 * s2
load(s5, 2)					# s5 = 2.0
getComponent(v0, s3, s5)	# s3 = v0.z
multiply(s3, s3, s3)		# s3 = s3 * s3
add(s1, s2, s1)				# s1 = s1 + s2
add(s1, s3, s1)				# s1 = s1 + s3
	
# Compute b (from quadratic formula), stored in s2
load(s2, 2)					# s2 = 2.0
negate(s2, s2)				# s2 = -s2
load(s5, 2)					# s5 = 2.0
getComponent(v0, s3, s5)	# s3 = v0.z
multiply(s2, s3, s2)		# s2 = s2 * s3
multiply(s2, s0, s2)		# s2 = s2 * s0

# Compute c (from quadratic formula), stored in s3
load(s3, 1)					# s3 = 1.0
negate(s3, s3)				# s3 = -s3
multiply(s0, s0, s4)		# s4 = s0 * s0
add(s4, s3, s3)				# s3 = s4 + s3

# Compute discriminant, store in s0
multiply(s2, s2, s4)		# s4 = s2 * s2
load(s5, 4)					# s5 = 4.0
multiply(s5, s1, s5)		# s5 = s5 * s1
multiply(s5, s3, s5)		# s5 = s5 * s3
negate(s5, s5)				# s5 = -s5
add(s4, s5, s0)				# s0 = s4 + s5

# Using: max(a, b) = 0.5 * (a + b + |a - b|)

# Clamp discriminant first time
# Set s1 = max(s0, 0.0)
sign(s0, s1)				# s1 = sign(s0)
multiply(s1, s0, s1)		# s1 = s1 * s0
add(s1, s0, s1)				# s1 = s1 + s0
load(s2, 2)					# s2 = 2.0
reciprocal(s2, s2)			# s2 = 1.0 / 2.0
multiply(s2, s1, s1)		# s1 = s2 * s1

# Set s2 = min(s1, 1.0) = -max(-s1, -1.0)
load(s4, 1)					# s4 = 1.0
negate(s1, s1)				# s1 = -s1
add(s4, s1, s3)				# s3 = s4 + s1
sign(s3, s2)				# s2 = sign(s3)
multiply(s2, s3, s2)		# s2 = s2 * s3
negate(s4, s4)				# s4 = -s4
add(s4, s2, s3)				# s3 = s4 + s2
add(s3, s1, s3)				# s3 = s3 + s1
move(s3, s0)				# s0 = s3
negate(s0, s0)				# s0 = -s0
load(s1, 2)					# s1 = 2.0
reciprocal(s1, s1)			# s1 = 1.0 / s1
multiply(s0, s1, s0)		# s0 = s0 * s1

# Take all values greater then 0, and ensure they are greater then 1
load(s1, 100)				# s1 = 100.0
multiply(s0, s1, s0)		# s0 = s0 * s1

# Clamp discriminant second time
# Set s1 = max(s0, 0.0)
sign(s0, s1)				# s1 = sign(s0)
multiply(s1, s0, s1)		# s1 = s1 * s0
add(s1, s0, s1)				# s1 = s1 + s0
load(s2, 2)					# s2 = 2.0
reciprocal(s2, s2)			# s2 = 1.0 / 2.0
multiply(s2, s1, s1)		# s1 = s2 * s1

# Set s2 = min(s1, 1.0) = -max(-s1, -1.0)
load(s4, 1)					# s4 = 1.0
negate(s1, s1)				# s1 = -s1
add(s4, s1, s3)				# s3 = s4 + s1
sign(s3, s2)				# s2 = sign(s3)
multiply(s2, s3, s2)		# s2 = s2 * s3
negate(s4, s4)				# s4 = -s4
add(s4, s2, s3)				# s3 = s4 + s2
add(s3, s1, s3)				# s3 = s3 + s1
move(s3, s0)				# s0 = s3
negate(s0, s0)				# s0 = -s0
load(s1, 2)					# s1 = 2.0
reciprocal(s1, s1)			# s1 = 1.0 / s1
multiply(s0, s1, s0)		# s0 = s0 * s1

load(s1, 1)					# s1 = 1.0
load(s2, 2)					# s2 = 2.0
load(s3, 3)					# s3 = 3.0

setComponent(c, s0, z)		# c.x = s0
setComponent(c, s0, s1)		# c.y = s0
setComponent(c, s0, s2)		# c.y = s0
setComponent(c, s1, s3)		# c.y = s1

######################################################################################
# What follows are the GLSL shaders I wrote while writing the above assembly version #
######################################################################################


# Initial GLSL version:
# vec2 uv = registers[reg_pc].xy / registers[reg_s].xy;
# // Adjust the uv to center around the origin
# uv = uv * 2.0;
# uv = uv - 1.0;
# 
# float aspectRatio = registers[reg_s].x / registers[reg_s].y;
# uv.x *= aspectRatio;
# 
# vec3 rayDirection = vec3(uv.x, uv.y, -1.0);
# rayDirection = normalize(rayDirection);
# vec3 rayOrigin = vec3(0.0, 0.0, 0.0);
# vec3 spherePosition = vec3(0.0, 0.0, -2.0);
# float sphereRadius = 1.0;
# float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
# float b = -2.0 * (spherePosition.x * rayDirection.x + spherePosition.y * rayDirection.y + spherePosition.z * rayDirection.z);
# float c = (spherePosition.x * spherePosition.x + spherePosition.y * spherePosition.y + spherePosition.z * spherePosition.z) - (sphereRadius * sphereRadius);
# float discriminant = (b * b) - (4 * a * c);
# 
# discriminant = clamp(discriminant, 0.0, 1.0);
# discriminant = discriminant * 100.0; // Take all values greater then 0, and ensure they are greater then 1
# discriminant = clamp(discriminant, 0.0, 1.0);
# outFragColor = vec4(discriminant, discriminant, discriminant, 1.0);


# Verbose GLSL version:
# vec4 uv;
# uv.x = registers[reg_pc].x;
# uv.y = registers[reg_pc].y;
# uv.z = 0.0;
# uv.w = 0.0;
# 
# float inverseSizeX = 1.0 / registers[reg_s].x;
# float inverseSizeY = 1.0 / registers[reg_s].y;
# 
# uv.x = uv.x * inverseSizeX;
# uv.y = uv.y * inverseSizeY;
# uv.x = uv.x * 2.0;
# uv.y = uv.y * 2.0;
# 
# uv.x = uv.x - 1.0;
# uv.y = uv.y - 1.0;
# 
# float inverseYPixels = 1.0 / registers[reg_s].y;
# float aspectRatio = registers[reg_s].x * inverseYPixels;
# uv.x = uv.x * aspectRatio;
# 
# vec4 rayDirection;
# rayDirection.x = uv.x;
# rayDirection.y = uv.y;
# rayDirection.z = -1.0;
# rayDirection.w = 0.0;
# 
# float sphereZ = -2.0;
# float sphereRadius = 1.0;
# float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
# float b = -2.0 * (sphereZ * rayDirection.z);
# float c = (sphereZ * sphereZ) - (sphereRadius * sphereRadius);
# float discriminant = (b * b) - (4 * a * c);
# 
# discriminant = min(max(discriminant, 0.0), 1.0);
# discriminant = discriminant * 100.0; // Take all values greater then 0, and ensure they are greater then 1
# discriminant = min(max(discriminant, 0.0), 1.0);
# 
# outFragColor.x = discriminant;
# outFragColor.y = discriminant;
# outFragColor.z = discriminant;
# outFragColor.w = 1.0;


# Ultra verbose GLSL version:
# // Compute uv, store it in v0
# s0 = registers[reg_pc][int(floor(z))];
# s1 = 1.0;
# s1 = registers[reg_pc][int(floor(s1))];
# v0[int(floor(z))] = s0;
# s2 = 1.0;
# v0[int(floor(s2))] = s1;
# s2 = 2.0;
# v0[int(floor(s2))] = z;
# s2 = 3.0;
# v0[int(floor(s2))] = z;
# 
# s0 = registers[reg_s][int(floor(z))];
# s5 = 1.0;
# s1 = registers[reg_s][int(floor(s5))];
# s0 = 1.0 / s0;
# s1 = 1.0 / s1;
# 
# s2 = v0[int(floor(z))];
# s5 = 1.0;
# s3 = v0[int(floor(s5))];
# s0 = s2 * s0;
# s1 = s3 * s1;
# v0[int(floor(z))] = s0;
# s5 = 1.0;
# v0[int(floor(s5))] = s1;
# 
# // Adjust the uv to center around the origin
# // Multiply by 2.0
# s0 = v0[int(floor(z))];
# s5 = 1.0;
# s1 = v0[int(floor(s5))];
# s2 = 2.0;
# s3 = s0 * s2;
# s4 = s1 * s2;
# v0[int(floor(z))] = s3;
# s5 = 1.0;
# v0[int(floor(s5))] = s4;
# 
# // Subtract 1.0
# s0 = v0[int(floor(z))];
# s5 = 1.0;
# s1 = v0[int(floor(s5))];
# s2 = 1.0;
# s2 = -s2;
# s0 = s0 + s2;
# s1 = s1 + s2;
# v0[int(floor(z))] = s0;
# s5 = 1.0;
# v0[int(floor(s5))] = s1;
# 
# // Adjust for aspect ratio
# s5 = 1.0;
# s0 = registers[reg_s][int(floor(s5))];
# s0 = 1.0 / s0;
# s1 = registers[reg_s][int(floor(z))];
# s0 = s1 * s0;
# s1 = v0[int(floor(z))];
# s0 = s1 * s0;
# v0[int(floor(z))] = s0;
# 
# // Initialize the ray direction, reuse the uv as the ray direction
# s5 = 2.0;
# v0[int(floor(s5))] = -1.0;
# s5 = 3.0;
# v0[int(floor(s5))] = z;
# 
# // Define sphere, s0 is the z coordinate
# s0 = 2.0;
# s0 = -s0;
# 
# // Compute a (from quadratic formula), stored in s1
# s1 = v0[int(floor(z))];
# s1 = s1 * s1;
# s5 = 1.0;
# s2 = v0[int(floor(s5))];
# s2 = s2 * s2;
# s5 = 2.0;
# s3 = v0[int(floor(s5))];
# s3 = s3 * s3;
# s1 = s1 + s2;
# s1 = s1 + s3;
# 
# // Compute b (from quadratic formula), stored in s2
# s2 = 2.0;
# s2 = -s2;
# s5 = 2.0;
# s3 = v0[int(floor(s5))];
# s2 = s2 * s3;
# s2 = s2 * s0;
# 
# // Compute c (from quadratic formula), stored in s3
# s3 = 1.0;
# s3 = -s3;
# s4 = s0 * s0;
# s3 = s4 + s3;
# 
# // Compute discriminant, store in s0
# s4 = s2 * s2;
# s5 = 4.0;
# s5 = s5 * s1;
# s5 = s5 * s3;
# s5 = -s5;
# s0 = s4 + s5;
# 
# // Using: max(a, b) = 0.5 * (a + b + |a - b|)
# 
# // Clamp discriminant first time
# // Set s1 = max(s0, 0.0)
# s1 = sign(s0);
# s1 = s1 * s0;
# s1 = s1 + s0;
# s2 = 2.0;
# s2 = 1.0 / 2.0;
# s1 = s2 * s1;
# 
# // Set s2 = min(s1, 1.0) = -max(-s1, -1.0)
# //s0 = -max(-s1, -1.0);
# s4 = 1.0;
# s1 = -s1;
# s3 = s4 + s1;
# s2 = sign(s3);
# s2 = s2 * s3;
# s4 = -s4;
# s3 = s4 + s2;
# s3 = s3 + s1;
# s0 = s3;
# s0 = -s0;
# s1 = 2.0;
# s1 = 1.0 / s1;
# s0 = s0 * s1;
# 
# // Take all values greater then 0, and ensure they are greater then 1
# s1 = 100.0;
# s0 = s0 * s1;
# 
# // Clamp discriminant second time
# // Set s1 = max(s0, 0.0)
# s1 = sign(s0);
# s1 = s1 * s0;
# s1 = s1 + s0;
# s2 = 2.0;
# s2 = 1.0 / 2.0;
# s1 = s2 * s1;
# 
# // Set s2 = min(s1, 1.0) = -max(-s1, -1.0)
# //s0 = -max(-s1, -1.0);
# s4 = 1.0;
# s1 = -s1;
# s3 = s4 + s1;
# s2 = sign(s3);
# s2 = s2 * s3;
# s4 = -s4;
# s3 = s4 + s2;
# s3 = s3 + s1;
# s0 = s3;
# s0 = -s0;
# s1 = 2.0;
# s1 = 1.0 / s1;
# s0 = s0 * s1;
# 
# s1 = 1.0;
# s2 = 2.0;
# s3 = 3.0;
# 
# outFragColor[int(floor(z))] = s0;
# outFragColor[int(floor(s1))] = s0;
# outFragColor[int(floor(s2))] = s0;
# outFragColor[int(floor(s3))] = 1.0;