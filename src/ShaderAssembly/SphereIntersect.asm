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