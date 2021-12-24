Assignment 4 - Wei Lin Chen

I have implemented all parts of the assignment. The output resembles visuals provided by professor.

I have one shader without lighting and shading, one shader with lighting and shading, and one for the fireworks

I use a global GLuint variable called program to be either the shading or nonshading
program, and pass "program" to objects that might need lighting and shading (sphere and floor)

For objects that do not require lighting (shadow, axes), I always pass in the nonshading program.

For the shading program, I calculate the global ambient light, directional light, and point source/ spotlight
in this order and added the result to color.

For fireworks, I always use the firework_program.